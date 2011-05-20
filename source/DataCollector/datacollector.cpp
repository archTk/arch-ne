////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2011 Mario Negri Institute & Orobix Srl
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////

#include <QtGui>
#include <QtGui/QTreeView>
#include <QtXmlPatterns>

#include "datacollector.h"
#include "treemodel.h"
#include "treedelegate.h"
#include "textdecorator.h"
#include <QTextStream>
using namespace std;

QTextStream dataout(stdout);

class MessageHandler : public QAbstractMessageHandler
{
   public:
      MessageHandler()
         : QAbstractMessageHandler(0)
      {
      }

      QString statusMessage() const
      {
         return m_description;
      }

      int line() const
      {
         return m_sourceLocation.line();
      }

      int column() const
      {
         return m_sourceLocation.column();
      }

   protected:
      virtual void handleMessage(QtMsgType type, const QString &description,
                                  const QUrl &identifier, const QSourceLocation &sourceLocation)
      {
         Q_UNUSED(type);
         Q_UNUSED(identifier);

         m_messageType = type;
         m_description = description;
         m_sourceLocation = sourceLocation;
      }

   private:
      QtMsgType m_messageType;
      QString m_description;
      QSourceLocation m_sourceLocation;
};

DataCollector::DataCollector(QString &cookie,QString &XMLString,QVector<QString> &hiddenItems,QVector<QString> &readOnlyItems,QWidget *parent)
   : QWidget(parent)
{
    Q_INIT_RESOURCE(DataCollector);
    setupUi(this);

    requestCookie = cookie;    
    requestHiddenItems = hiddenItems;
    requestReadOnlyItems = readOnlyItems;

    workingDomText->setReadOnly(true);
    new TextDecorator(workingDomText->document());
    actionShowDom->setChecked(false);
    workingDomText->hide();
    detailStatusText->hide();
    actionOkCombo->setEnabled(false);
    actionCancelCombo->setEnabled(false);

    avoidUpdateFlag = false;
    setQueryStrings();
    setElementDom(XMLString);

    TreeModel *model = new TreeModel();
    TreeDelegate *delegate = new TreeDelegate();
    treeView->setItemDelegate(delegate);
    treeView->setModel(model);
    treeView->setColumnHidden(2,true);
    treeView->setColumnHidden(3,true);
    treeView->setColumnHidden(4,true);
    treeView->setColumnHidden(5,true);
    treeView->setColumnHidden(6,true);
    treeView->setColumnHidden(7,true);
    treeView->setColumnHidden(10,true);
    treeView->setColumnHidden(11,true);
    treeView->header()->moveSection(8,0);
    treeView->header()->moveSection(9,0);

    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(treeViewDataChanged()));
    connect(treeView->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),this, SLOT(updateTools()));
    connect(treeView,SIGNAL(clicked(const QModelIndex &)),this, SLOT(viewClicked(const QModelIndex &)));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(buttonApplyClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(buttonOkClicked()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    connect(actionShowDom,SIGNAL(triggered()),this, SLOT(showDomToggled()));
    connect(actionUndo,SIGNAL(triggered()),this, SLOT(undoClicked()));
    connect(actionRedo,SIGNAL(triggered()),this, SLOT(redoClicked()));
    connect(actionReset, SIGNAL(triggered()), this, SLOT(resetClicked()));
    connect(actionEditDom, SIGNAL(triggered()), this, SLOT(editDomToggled()));
    connect(this, SIGNAL(setErrorLine(int)), model, SLOT(setErrorLine(int)));
}

void DataCollector::viewClicked(const QModelIndex &index)
{
    if (index.column()==9 and actionRemoveElement->isEnabled()){
        removeRow(index);
        return;
    }
    QAbstractItemModel *model = treeView->model();
    QStringList leftMembers = model->data(model->index(index.row(),5,index.parent())).toStringList();
    if (index.column()==8 and leftMembers.size()>1){
        treeView->edit(treeView->selectionModel()->currentIndex());
        return;
    }
    if (index.column()==8 and leftMembers.size()==1){
        QDomDocument *doc = new QDomDocument(); 
        QDomElement addedElement = doc->createElement(leftMembers.at(0));
        avoidUpdateFlag=true;
        DomToTree(addedElement,model->data(model->index(index.row(),3,index.parent())).toString(),model->index(index.row(),0,index.parent()));
        avoidUpdateFlag=false;
        updateDomStatus(true);
        return;
    }
    if (index.column()==1 and !model->data(model->index(index.row(),7,index.parent())).toBool()){
        treeView->edit(treeView->selectionModel()->currentIndex());
        return;
    }
}

void DataCollector::setupData()
{
    undoStack.append(requestDom);
    QDomDocument domDoc;
    domDoc.setContent(undoStack.at(0));

    rootName = domDoc.firstChild().nodeName();
    QFile sf(requestSchemaPath);
    sf.open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("inputDocument", &sf);
    query.bindVariable("rootName", QVariant(rootName));
    query.setQuery("declare variable $inputDocument external;\ndeclare variable $rootName external;\nlet $type := data(doc($inputDocument)//xs:element[@name = $rootName]/@type)\nreturn string($type)");
    if (!query.isValid())
        return;
    QString result;
    if (!query.evaluateTo(&result))
        return;
    sf.close();
    rootType = result.trimmed();
    setRequestSchema();
    currentStack=0;
    loadStack(0);
    undoStack.replace(0,workingDom);
}

void DataCollector::editDomToggled()
{
    if(actionEditDom->isChecked()){
        actionShowDom->setChecked(true);
        workingDomText->show();
        workingDomText->setReadOnly(false);
        treeView->setEnabled(false);
    } else {
        actionShowDom->setChecked(false);
        workingDomText->hide();
        workingDomText->setReadOnly(true);
        treeView->setEnabled(true);
        workingDom = workingDomText->toPlainText();
        updateHistory();
        loadStack(undoStack.size()-1);
    }
}

void DataCollector::loadStack(const int &stackId)
{
   avoidUpdateFlag = true;
   autoFill=false;
   QAbstractItemModel *model = treeView->model();
   if(model->index(0,0,treeView->rootIndex()).isValid())
      removeRow(model->index(0,0,treeView->rootIndex()));
   QDomDocument domDoc;
   domDoc.setContent(undoStack.at(stackId));
   QDomElement rootElement = domDoc.documentElement();
   DomToTree(rootElement,"no_parent",treeView->rootIndex(),true);
   treeView->setColumnWidth(0,150);
   treeView->setColumnWidth(8,25);
   treeView->setColumnWidth(9,25);
   treeView->setExpanded(model->index(0,0,treeView->rootIndex()),true);
   QModelIndex mainIndex = model->index(0,0,treeView->rootIndex());
   for (int i=0;i<model->rowCount(mainIndex);++i){
       if(model->data(model->index(i,2,mainIndex)).toBool() and requestHiddenItems.indexOf(model->data(model->index(i,0,mainIndex)).toString()) >=0 )
           treeView->setRowHidden(i,mainIndex,true);

       if(model->data(model->index(i,2,mainIndex)).toBool() and requestReadOnlyItems.indexOf(model->data(model->index(i,0,mainIndex)).toString()) >=0 )
            model->setData(model->index(i,7,mainIndex),QVariant(true), Qt::EditRole);
   }
   avoidUpdateFlag = false;
   updateDomStatus(false);
   updateTools();
   autoFill=true;
}

void DataCollector::removeRow(QModelIndex index)
{
    QModelIndex parent = index.parent();
    if (!treeView->model()->removeRow(index.row(), index.parent()))
        return;
    updateLeftMembers(parent);
    updateTools();
}

void DataCollector::setQueryStrings()
{
    QFile queryFile(":xml/getElementInfo.xq");
    queryFile.open(QIODevice::ReadOnly);
    getElementInfoXqString = QString::fromLatin1(queryFile.readAll());
    queryFile.close();

    queryFile.setFileName(":xml/getEnumeration.xq");
    queryFile.open(QIODevice::ReadOnly);
    getEnumerationXqString = QString::fromLatin1(queryFile.readAll());
    queryFile.close();
}

QStringList DataCollector::getEnumeration(const QString &iType)
{
    QFile docFile(requestSchemaPath);
    docFile.open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("inputDocument", &docFile);
    query.bindVariable("attributeType", QVariant(iType));
    query.setQuery(getEnumerationXqString);
    if (!query.isValid())
        return QStringList();

    QStringList resultList;
    if (!query.evaluateTo(&resultList))
        return QStringList();

    docFile.close();
    return resultList;
}

QList<QStringList> DataCollector::getElementInfo(QString elementName,QString parentType)
{
    QFile docFile(requestSchemaPath);
    docFile.open(QIODevice::ReadOnly);

    QXmlQuery query;
    query.bindVariable("inputDocument", &docFile);
    query.bindVariable("rootName", QVariant(elementName));
    query.bindVariable("rootParent", QVariant(parentType));
    query.bindVariable("givenType", QVariant(rootType));
    query.setQuery(getElementInfoXqString);
    if (!query.isValid())
        return QList<QStringList>();

    QStringList resultList;
    if (!query.evaluateTo(&resultList))
        return QList<QStringList>();
    docFile.close();

    QList<QStringList> output;
    int secondHash = resultList.indexOf("#",resultList.indexOf("#")+1);
    output.append(resultList.mid(0,1));
    output.append(resultList.mid(resultList.indexOf("#")+1,(secondHash-resultList.indexOf("#"))-1));
    output.append(fixElementInfo(resultList.mid(secondHash+1,resultList.lastIndexOf("#")-secondHash-1)));
    output.append(fixElementInfo(resultList.mid(resultList.lastIndexOf("#")+1)));
    return output;
}

QStringList DataCollector::fixElementInfo(QList<QString> info)
{
    QStringList fixedMembers;
    QString tmp;
    for (int i=0;i<info.size();++i){
        tmp = info.at(i);
        if(tmp.contains(",,"))
           tmp.insert(tmp.lastIndexOf(","),"1");
        if(tmp.endsWith(","))
           tmp.append("1");
        fixedMembers.append(tmp);
    }
    return fixedMembers;
}

int DataCollector::insertChild(bool isAttribute,bool required,QModelIndex index,QString rowFirstCol,
                               QString type,QStringList members,QStringList enumeration,QString value)
{
   QAbstractItemModel *model = treeView->model();
   if (model->columnCount(index) == 0) {
      if (!model->insertColumn(0, index))
         return 0;
   }

   int iRow = model->rowCount(index);
   if(!isAttribute and iRow > 0){
       iRow = getInsertionRow(index,rowFirstCol);
   }

   model->insertRow(iRow, index);
   model->setData(model->index(iRow, 0, index), QVariant(rowFirstCol), Qt::EditRole);
   model->setData(model->index(iRow, 1, index), QVariant(value), Qt::EditRole);
   model->setData(model->index(iRow, 3, index), QVariant(type), Qt::EditRole);
   model->setData(model->index(iRow, 4, index), QVariant(members), Qt::EditRole);
   model->setData(model->index(iRow, 6, index), QVariant(required), Qt::EditRole);
   model->setData(model->index(iRow, 10, index), QVariant(enumeration), Qt::EditRole);
   if (isAttribute){
      model->setData(model->index(iRow, 2, index), QVariant(isAttribute), Qt::EditRole);
   } else {
      QStringList splitList;
      for (int i=0;i<members.size();++i)
         splitList.append(members.at(i).split(",")[0]);
      model->setData(model->index(iRow, 5, index), QVariant(splitList), Qt::EditRole);
      updateLeftMembers(index);
   }
   return iRow;
}

void DataCollector::updateHistory()
{
    undoStack = undoStack.mid(0,currentStack+1);
    undoStack.append(workingDom);
    currentStack = undoStack.size()-1;
}

int DataCollector::getInsertionRow(QModelIndex &index,QString &iName)
{
   QAbstractItemModel *model = treeView->model();
   QStringList currentMembers;
   for(int i=0;i<model->rowCount(index);++i){
      if (!model->data(model->index(i,2,index)).toBool())
         currentMembers.append(model->data(model->index(i,0,index)).toString());
   }
   if (currentMembers.isEmpty())
      return model->rowCount(index);

   QStringList list = model->data(model->index(index.row(),4,index.parent())).toStringList();
   QStringList splitList;
   for (int i=0;i<list.size();++i)
      splitList.append(list.at(i).split(",")[0]);

   int minIndex = model->rowCount(index) - currentMembers.size();
   for(int i=splitList.indexOf(iName);i>-1;--i){
      if(currentMembers.lastIndexOf(splitList.at(i))>=0){
         return minIndex + currentMembers.lastIndexOf(splitList.at(i))+1;
      }
   }
   return minIndex;
}

void DataCollector::updateLeftMembers(QModelIndex &index)
{
   QAbstractItemModel *model = treeView->model();
   QStringList list = model->data(model->index(index.row(),4,index.parent())).toStringList();
   QStringList splitList;
   for (int i=0;i<list.size();++i)
      splitList.append(list.at(i).split(",")[0]);
   if(model->rowCount(index) == 0){
      model->setData(model->index(index.row(), 5, index.parent()), QVariant(splitList), Qt::EditRole);
      return;
   }

   QStringList currentMembers;
   for(int i=0;i<model->rowCount(index);++i){
      if (!model->data(model->index(i,2,index)).toBool())
         currentMembers.append(model->data(model->index(i,0,index)).toString());
   }

   QStringList mNames;
   for(int i=0;i<list.size();++i){
      if (list.at(i).split(",")[2] == "unbounded" ){
         mNames.append(list.at(i).split(",")[0]);
      } else {
         int maxOccurs = list.at(i).split(",")[2].toInt();
         if(currentMembers.count(list.at(i).split(",")[0]) < maxOccurs)
             mNames.append(list.at(i).split(",")[0]);
      }
   }
   model->setData(model->index(index.row(), 5, index.parent()), QVariant(mNames), Qt::EditRole);
}

void DataCollector::updateTools()
{
     actionUndo->setEnabled(currentStack > 0);
     actionRedo->setEnabled(currentStack < undoStack.size()-1);
     actionReset->setEnabled(currentStack > 0);
     bool viewSelected = !treeView->selectionModel()->selection().isEmpty();
     bool validSelection = treeView->selectionModel()->currentIndex().isValid();
     if (viewSelected and validSelection) {
         QModelIndex index = treeView->selectionModel()->currentIndex();
         QAbstractItemModel *model = treeView->model();
         actionRemoveElement->setEnabled(!model->data(model->index(index.row(),6,index.parent())).toBool());
     } else {
         actionRemoveElement->setEnabled(false);
     }
}

void DataCollector::showDomToggled()
{
    if(actionShowDom->isChecked()){
        workingDomText->show();
    } else {
        workingDomText->hide();
    }
}

void DataCollector::resetClicked()
{
    undoStack.clear();
    undoStack.append(requestDom);
    currentStack=0;
    loadStack(0);
}

void DataCollector::redoClicked()
{
    loadStack(++currentStack);
}

void DataCollector::undoClicked()
{
    loadStack(--currentStack);
}

void DataCollector::cancelClicked()
{
    emit(okButtonClicked(requestCookie,requestDom));
    emit deleteItself();
    dataout << "LOG@_DataCollector::cancelClicked()" << endl;
}

void DataCollector::setRequestSchemaPath(QString &path)
{
    requestSchemaPath = path;
    setupData();
}

void DataCollector::setElementDom(QString &theElementDom)
{
    QByteArray byteArray;
    byteArray.append(theElementDom);
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("inputDocument", &buffer);
    query.setQuery("declare variable $inputDocument external;\ndoc($inputDocument)/*");
    if (!query.isValid())
        return;
    if (!query.evaluateTo(&requestDom))
        return;
    buffer.close();
}

bool DataCollector::getConfirmation()
{
   QMessageBox confirmationBox;
   confirmationBox.setText("You are about to send non valid data");
   confirmationBox.setInformativeText("Do you wish to continue?");
   confirmationBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
   confirmationBox.setDefaultButton(QMessageBox::No);
   return confirmationBox.exec()==QMessageBox::Yes;
}

void DataCollector::buttonApplyClicked()
{
    updateDomStatus(false);
    if(!domStatus and !getConfirmation()){
       return;
    }
    emit(applyClicked(requestCookie,workingDom));
}

void DataCollector::buttonOkClicked()
{
    updateDomStatus(false);
    if(!domStatus and !getConfirmation()){
       return;
    }
    emit(okButtonClicked(requestCookie,workingDom));
    emit deleteItself();
}

void DataCollector::treeViewDataChanged()
{
    if (avoidUpdateFlag)
        return;
    if (actionEditDom->isChecked())
        return;

    QModelIndex index = treeView->selectionModel()->currentIndex();
    if (index.column()==8) {
        QAbstractItemModel *model = treeView->model();
        QDomDocument *doc = new QDomDocument(); 
        QDomElement addedElement = doc->createElement(model->data(index).toString());
        avoidUpdateFlag=true;
        DomToTree(addedElement,model->data(model->index(index.row(),3,index.parent())).toString(),model->index(index.row(),0,index.parent()));
        model->setData(index,QVariant(""),Qt::EditRole);
        avoidUpdateFlag=false;
        updateDomStatus(true);
        return;
    }
    updateDomStatus(true);
    updateTools();
}

void DataCollector::updateDomStatus(bool newStack)
{
    QAbstractItemModel *model = treeView->model();
    QString irootName = model->data(model->index(0,0,treeView->rootIndex())).toString();
    QDomDocument *doc = new QDomDocument(); 
    QDomElement rootElement = doc->createElement(irootName);
    doc->appendChild(rootElement);
    lineCount=1;
    TreeToDom(doc,rootElement,model->index(0,0,treeView->rootIndex()));
    workingDom = doc->toString();
    workingDomText->setPlainText(workingDom);
    domStatus = validateDom(workingDom);
    if (newStack)
        updateHistory();
}

void DataCollector::TreeToDom(QDomDocument *doc,QDomElement iDomElement,QModelIndex index)
{
   QAbstractItemModel *model = treeView->model();
   int rowCount = model->rowCount(index);
   int elsCount=0;
   avoidUpdateFlag=true;
   model->setData(model->index(index.row(), 11, index.parent()), QVariant(lineCount), Qt::EditRole);
   avoidUpdateFlag=false;
   for (int i=0;i<rowCount;++i){
      QString iName = model->data(model->index(i,0,index)).toString();
      QString iValue = model->data(model->index(i,1,index)).toString();
      if (model->data(model->index(i,2,index)).toBool()){
         if (!iValue.isEmpty())
            iDomElement.setAttribute(iName,iValue);
      } else {
			++lineCount;
         if (!iValue.isEmpty()){
            QDomElement iTag = doc->createElement(iName);
            QDomText textNode = doc->createTextNode(iValue);
            iTag.appendChild(textNode);
            TreeToDom(doc, iTag,model->index(i,0,index));
            iDomElement.appendChild(iTag);
         } else {
            QDomElement iTag = doc->createElement(iName);
            TreeToDom(doc, iTag,model->index(i,0,index));
            iDomElement.appendChild(iTag);
         }
         ++elsCount;
      }
   }
   if (elsCount>0)
      ++lineCount;
}

void DataCollector::DomToTree(QDomElement iDomElement,QString iParentType,QModelIndex index,bool required)
{
    QAbstractItemModel *model = treeView->model();
    QString childText = "";

    if(iDomElement.firstChild().isText())
        childText.append(iDomElement.text().replace("\n",""));

    QList<QStringList> iInfo = getElementInfo(iDomElement.tagName(),iParentType);
    QStringList anyMember = iInfo.at(2);
    anyMember << iInfo.at(3);
    int iRow = insertChild(false,required,index,iDomElement.tagName(),iInfo.at(0).at(0),
                            anyMember,getEnumeration(iInfo.at(0).at(0)),childText);
    QDomNamedNodeMap attributeMap = iDomElement.attributes();

    if(!iInfo.at(1).isEmpty()){
        for (int i = 0; i < iInfo.at(1).count(); ++i) {
            QString iName = iInfo.at(1).at(i).split("=")[0];
            QString iType = iInfo.at(1).at(i).split("=")[1];
            QString iVal = "";
            if(!attributeMap.namedItem(iName).isNull())
               iVal.append(attributeMap.namedItem(iName).nodeValue());
            insertChild(true,true,model->index(iRow,0,index),iName,iType,QStringList(),getEnumeration(iType),iVal);
        }
    }

    if (!childText.isEmpty())
        return;

    QStringList currentChildren;
    if(!iDomElement.childNodes().isEmpty()){
        for(int i=0;i<iDomElement.childNodes().size();++i){
            currentChildren.append(iDomElement.childNodes().item(i).toElement().tagName());
        } 
    }

    if(autoFill or currentStack==0){
       QDomDocument *doc = new QDomDocument();
       for(int i=0;i<iInfo.at(2).size();++i){
          for(int j=0;j<iInfo.at(2).at(i).split(",")[1].toInt();++j){
            int k = currentChildren.indexOf(iInfo.at(2).at(i).split(",")[0]);
            if(k>-1){
                DomToTree(iDomElement.childNodes().item(k).toElement(),iInfo.at(0).at(0),model->index(iRow,0,index),true);
                currentChildren.replace(k,"#");
            } else {
                QDomElement jElement = doc->createElement(iInfo.at(2).at(i).split(",")[0]);
                DomToTree(jElement,iInfo.at(0).at(0),model->index(iRow,0,index),true); 
            }
          }
       }
    }

    for(int i=0;i<iDomElement.childNodes().size();++i){
        if(currentChildren.at(i)!="#")
            DomToTree(iDomElement.childNodes().item(i).toElement(),iInfo.at(0).at(0),model->index(iRow,0,index)); 
    }
}

void DataCollector::setRequestSchema()
{
    QByteArray resultString;
    bool cachedSchema = false;

    QFileInfo fi(requestSchemaPath);
    QFile file(QDir::tempPath()+"/"+fi.fileName()+"."+rootName+".dat");
    if (file.open(QIODevice::ReadOnly)) {
            resultString.append(QString::fromLatin1(file.readAll()));    
            file.close();
            cachedSchema = true;
    }
    if(!cachedSchema){
       QXmlQuery query;
       QFile sf(requestSchemaPath);
       sf.open(QIODevice::ReadOnly);
       QFile queryFile(":xml/getSubSchema.xq");
       queryFile.open(QIODevice::ReadOnly);
       query.bindVariable("inputDocument", &sf);
       query.bindVariable("rootName", QVariant(rootName));
       query.bindVariable("rootType", QVariant(rootType));
       query.setQuery(QString::fromLatin1(queryFile.readAll()));
       if (!query.isValid())
           return;
       QString result;
       if (!query.evaluateTo(&result))
           return;
       sf.close();
       queryFile.close();

       resultString.append(result);
       QFile ofile(QDir::tempPath()+"/"+fi.fileName()+"."+rootName+".dat");
       if (ofile.open(QIODevice::WriteOnly)) {
           ofile.write(resultString);
           ofile.close();
       }   
    }

    requestSchema.load(resultString);
}

bool DataCollector::validateDom(QString &DomString)
{
    if(!requestSchema.isValid())
        return false;
 
    MessageHandler messageHandler;
    QByteArray XMLByteContent;
    requestSchema.setMessageHandler(&messageHandler);
    QXmlSchemaValidator validator(requestSchema);

    emit(setErrorLine(0));
    XMLByteContent.append(DomString);
    workingDomText->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    if (validator.validate(XMLByteContent)){
        detailStatusText->hide();
        return true;
    }

    detailStatusText->setHtml(messageHandler.statusMessage());
    highlightError(messageHandler.line());
    detailStatusText->show();
    emit(setErrorLine(messageHandler.line()));
    return false;
}

void DataCollector::highlightError(const int &line)
{
    workingDomText->moveCursor(QTextCursor::Start);
    for (int i=1; i<line; ++i)
        workingDomText->moveCursor(QTextCursor::Down);

    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor("#ee8080"));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = workingDomText->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    workingDomText->setExtraSelections(extraSelections);
}
