/////////////////////////////////////////////////////////////////////////////////////////////
//
//   Copyright 2011 - ARCH vascular Netwrok Editor workgroup
//
//                    Orobix Srl -- www.orobix.com
//
//   Licensed under the ARCH vascular Netwrok Editor License, Version 1.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       https://github.com/archTk/arch-ne/licenses/LICENSE-1.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include <QtGui>
#include <QtGui/QTreeView>
#include <QtXmlPatterns>

#include "datacollector.h"
#include "queryexecuter.h"
#include "treemodel.h"

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

DataCollector::DataCollector(QString cookie,QString XMLString,QVector<QString> hiddenItems,QVector<QString> readOnlyItems,QWidget *parent)
   : QWidget(parent)
{
    Q_INIT_RESOURCE(DataCollector);
    setupUi(this);

    workingDomText->setReadOnly(true);
    actionShowDom->setChecked(true);
    mainComboBox->setEnabled(false);
    actionOkCombo->setEnabled(false);
    actionCancelCombo->setEnabled(false);

    avoidUpdateFlag = false;
    setQueryStrings();
    setHiddenItems(hiddenItems);
    setElementDom(XMLString);
    setElementCookie(cookie);
    setReadOnlyItems(readOnlyItems);
    setRequestSchemaPath(":xml/vascular_network_v3.2.xsd");

    TreeModel *model = new TreeModel();
    treeView->setModel(model);
    treeView->setColumnHidden(2,true);
    treeView->setColumnHidden(3,true);
    treeView->setColumnHidden(4,true);
    treeView->setColumnHidden(5,true);
    treeView->setColumnHidden(6,true);
    setupData();

    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(treeViewDataChanged()));
    connect(treeView->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),this, SLOT(updateTools()));
    connect(treeView,SIGNAL(activated(QModelIndex)),this, SLOT(itemActivated(QModelIndex)));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(buttonApplyClicked()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(buttonOkClicked()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    connect(actionShowDom,SIGNAL(triggered()),this, SLOT(showDomToggled()));
    connect(actionUndo,SIGNAL(triggered()),this, SLOT(undoClicked()));
    connect(actionRedo,SIGNAL(triggered()),this, SLOT(redoClicked()));
    connect(actionRemoveElement,SIGNAL(triggered()),this, SLOT(removeElement()));
    connect(actionAddChild,SIGNAL(triggered()),this, SLOT(addChild()));
    connect(actionOkCombo,SIGNAL(triggered()),this, SLOT(okCombo()));
    connect(actionCancelCombo,SIGNAL(triggered()),this, SLOT(cancelCombo()));
    connect(actionReset, SIGNAL(triggered()), this, SLOT(resetClicked()));
      
}

void DataCollector::setupData()
{
    undoStack.append(requestDom);
    QDomDocument domDoc;
    domDoc.setContent(undoStack.at(0));    

    rootName = domDoc.firstChild().nodeName();
    rootSchemaType = rootName + "Type";
    rootParent = rootName + "sType";
    setRequestSchema();
    currentStack=0;
    loadStack(0);
    undoStack.replace(0,workingDom);
}

void DataCollector::loadStack(int stackId)
{
    avoidUpdateFlag = true;
    autoFill=false;
    QAbstractItemModel *model = treeView->model();
    removeRow(model->index(0,0,treeView->rootIndex())); 
    QDomDocument domDoc;
    domDoc.setContent(undoStack.at(stackId));    
    QDomElement rootElement = domDoc.documentElement();

    DomToTree(rootElement,rootParent,treeView->rootIndex(),true);
    avoidUpdateFlag = false;
    treeView->setColumnWidth(0,200);
    newStack=false; 
    updateDomStatus();
    newStack=true;
    updateTools();
    autoFill=true;
}

void DataCollector::itemActivated(QModelIndex index)
{
    if (index.column()!=1)
        return;
    
    QAbstractItemModel *model = treeView->model();
    if(!model->data(model->index(index.row(),2,index.parent())).toBool())
       return;

    QStringList list = model->data(model->index(index.row(),4,index.parent())).toStringList();
    if(list.isEmpty())
       return;
    list.prepend("...");  
 
    treeView->setEnabled(false);
    mainComboBox->clear();
    mainComboBox->setEnabled(true);
    mainComboBox->insertItems(0,list);
    mainComboBox->setFocus();
    actionOkCombo->setEnabled(true);
    actionCancelCombo->setEnabled(true);
    actionAddChild->setEnabled(false);
    actionRemoveElement->setEnabled(false);
    actionReset->setEnabled(false);
}

void DataCollector::removeRow(QModelIndex index)
{
    QAbstractItemModel *model = treeView->model();
    model->removeRow(index.row(), index.parent());
    updateLeftMembers(index.parent());
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

QStringList DataCollector::getEnumeration(QString iType)
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

void DataCollector::okCombo()
{
    QModelIndex index = treeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = treeView->model();
    QString inputString = mainComboBox->currentText();
    if(model->data(model->index(index.row(),2,index.parent())).toBool()){
        if (inputString=="...")
            inputString.clear();
        model->setData(index,QVariant(inputString), Qt::EditRole);
    } else {
        QDomDocument *doc = new QDomDocument(); 
        QDomElement addedElement = doc->createElement(inputString);
        avoidUpdateFlag=true;
        DomToTree(addedElement,model->data(model->index(index.row(),3,index.parent())).toString(),index);
        avoidUpdateFlag=false;
        updateDomStatus();
    }
    treeView->setEnabled(true);
    mainComboBox->clear();
    mainComboBox->setEnabled(false);
    actionOkCombo->setEnabled(false);
    actionCancelCombo->setEnabled(false);
    actionReset->setEnabled(true);
    updateTools();
}

void DataCollector::cancelCombo()
{
    treeView->setEnabled(true);
    mainComboBox->clear();
    mainComboBox->setEnabled(false);
    actionOkCombo->setEnabled(false);
    actionCancelCombo->setEnabled(false);
    actionReset->setEnabled(true);
    updateTools();
}


QList<QStringList> DataCollector::getElementInfo(QString elementName, QString parentName)
{
    QFile docFile(requestSchemaPath);
    docFile.open(QIODevice::ReadOnly);

    QXmlQuery query;
    query.bindVariable("inputDocument", &docFile);
    query.bindVariable("rootName", QVariant(elementName));
    query.bindVariable("rootParent", QVariant(parentName));
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

QStringList DataCollector::fixElementInfo(QStringList info)
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

void DataCollector::removeElement()
{
    QModelIndex index = treeView->selectionModel()->currentIndex();
    removeRow(index);
}

int DataCollector::insertChild(bool isAttribute,bool required,QModelIndex index,QString rowFirstCol,QString type,QStringList members,QString value)
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
    if (isAttribute){
        model->setData(model->index(iRow, 2, index), QVariant("1"), Qt::EditRole);
    } else {
        QStringList splitList;
        for (int i=0;i<members.size();++i)
            splitList.append(members.at(i).split(",")[0]);
        model->setData(model->index(iRow, 5, index), QVariant(splitList), Qt::EditRole);
        updateLeftMembers(index);
    }
    treeView->setExpanded(index,true);
    return iRow;
}

void DataCollector::updateHistory()
{
    undoStack = undoStack.mid(0,currentStack+1);
    undoStack.append(workingDom);
    currentStack = undoStack.size()-1;
}

int DataCollector::getInsertionRow(QModelIndex index,QString iName)
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

void DataCollector::updateLeftMembers(QModelIndex index)
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

void DataCollector::addChild()
{
    QModelIndex index = treeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = treeView->model();
    QStringList mNames = model->data(model->index(index.row(),5,index.parent())).toStringList();
    QString inputString;
    if (mNames.size()==1){
        inputString = mNames.at(0);
        QDomDocument *doc = new QDomDocument(); 
        QDomElement addedElement = doc->createElement(inputString);
        avoidUpdateFlag=true;
        DomToTree(addedElement,model->data(model->index(index.row(),3,index.parent())).toString(),index);
        avoidUpdateFlag=false;
        updateDomStatus();
        updateTools();
    } else {
        treeView->setEnabled(false);
        mainComboBox->clear();
        mainComboBox->setEnabled(true);
        mainComboBox->insertItems(0,mNames);
        mainComboBox->setFocus();
        actionOkCombo->setEnabled(true);
        actionCancelCombo->setEnabled(true);
        actionAddChild->setEnabled(false);
        actionRemoveElement->setEnabled(false);
        actionReset->setEnabled(false);
    }
}

void DataCollector::updateTools()
{
     actionUndo->setEnabled(currentStack > 0);
     actionRedo->setEnabled(currentStack < undoStack.size()-1);

     bool viewSelected = !treeView->selectionModel()->selection().isEmpty();
     bool validSelection = treeView->selectionModel()->currentIndex().isValid();

     if (viewSelected and validSelection) {
         QModelIndex index = treeView->selectionModel()->currentIndex();
         QAbstractItemModel *model = treeView->model();
         bool isAttribute = model->data(model->index(index.row(),2,index.parent())).toBool();
         bool hasMembers = !model->data(model->index(index.row(),5,index.parent())).toStringList().isEmpty();
         bool required = model->data(model->index(index.row(),6,index.parent())).toBool();
         actionAddChild->setEnabled(!isAttribute and hasMembers);
         actionRemoveElement->setEnabled(!required);
     } else {
         actionAddChild->setEnabled(false);
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
    currentStack++;
    loadStack(currentStack);
}

void DataCollector::undoClicked()
{
    currentStack--;
    loadStack(currentStack);
}

void DataCollector::cancelClicked()
{
    emit(okButtonClicked(requestCookie,requestDom));
    emit deleteItself();
}

void DataCollector::setHiddenItems(QVector<QString> theHiddenItems)
{
    requestHiddenItems = theHiddenItems;
}

void DataCollector::setRequestSchemaPath(QString path)
{
    requestSchemaPath = path;
}

void DataCollector::setElementDom(QString theElementDom)
{
    requestDom = theElementDom;
}

void DataCollector::setElementCookie(QString theElementCookie)
{
    requestCookie = theElementCookie;    
}

void DataCollector::setReadOnlyItems(QVector<QString> theReadOnlyItems)
{
    requestReadOnlyItems = theReadOnlyItems;
}

void DataCollector::buttonApplyClicked()
{
    newStack=false;
    updateDomStatus();
    newStack=true;

    if(!domStatus){
       QMessageBox confirmationBox;
       confirmationBox.setText("You are about to apply non valid data");
       confirmationBox.setInformativeText("Do you wish to continue?");
       confirmationBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
       confirmationBox.setDefaultButton(QMessageBox::No);
       int answerValue = confirmationBox.exec();
       if (answerValue == QMessageBox::No) 
           return;
    }

    emit(applyClicked(requestCookie,workingDom));
}

void DataCollector::buttonOkClicked()
{
    newStack=false;
    updateDomStatus();
    newStack=true;

    if(!domStatus){
       QMessageBox confirmationBox;
       confirmationBox.setText("You are about to submit non valid data");
       confirmationBox.setInformativeText("Do you wish to continue?");
       confirmationBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
       confirmationBox.setDefaultButton(QMessageBox::No);
       int answerValue = confirmationBox.exec();
       if (answerValue == QMessageBox::No) 
           return;
    }

    emit(okButtonClicked(requestCookie,workingDom));
    emit deleteItself();
}

void DataCollector::treeViewDataChanged()
{
    if (avoidUpdateFlag)
       return;

    updateDomStatus();
    updateTools();
} 

void DataCollector::updateDomStatus()
{
    QAbstractItemModel *model = treeView->model();
    QModelIndex mainIndex = model->index(0,0,treeView->rootIndex());
    QString irootName = model->data(mainIndex).toString();
    QDomDocument *doc = new QDomDocument(); 
    QDomElement rootElement = doc->createElement(irootName);
    doc->appendChild(rootElement);
    TreeToDom(doc,rootElement,mainIndex);
    workingDom = doc->toString();
    domStatus = validateDom(workingDom);
    if (newStack)
        updateHistory();
    workingDomText->setPlainText(workingDom);
}
    
void DataCollector::TreeToDom(QDomDocument *doc,QDomElement iDomElement,QModelIndex index)
{
    QAbstractItemModel *model = treeView->model();

    int rowCount = model->rowCount(index);
    for (int i=0;i<rowCount;++i){
        QString iName = model->data(model->index(i,0,index)).toString();
        QString iValue = model->data(model->index(i,1,index)).toString();
        QString iIsAttribute = model->data(model->index(i,2,index)).toString();
        if (iIsAttribute == "1"){
            if (!iValue.isEmpty())
                iDomElement.setAttribute(iName,iValue);
        } else {
            if(!iValue.isEmpty()){
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
        }
    }
}

void DataCollector::DomToTree(QDomElement iDomElement,QString iParentType,QModelIndex index,bool required)
{
    QAbstractItemModel *model = treeView->model();

    QString childText = "";

    if(iDomElement.firstChild().isText())
        childText.append(iDomElement.text());

    QList<QStringList> iInfo = getElementInfo(iDomElement.tagName(),iParentType);
    QStringList anyMember = iInfo.at(2);
    anyMember << iInfo.at(3);
    int iRow = insertChild(false,required,index,iDomElement.tagName(),iInfo.at(0).at(0),anyMember,childText);
    QDomNamedNodeMap attributeMap = iDomElement.attributes();

    if(!iInfo.at(1).isEmpty()){
        for (int i = 0; i < iInfo.at(1).count(); ++i) {
            QString iName = iInfo.at(1).at(i).split("=")[0];
            QString iType = iInfo.at(1).at(i).split("=")[1];
            QStringList iMembers = getEnumeration(iType);
            QString iVal = "";
            if(!attributeMap.namedItem(iName).isNull())
               iVal.append(attributeMap.namedItem(iName).nodeValue()); 
            insertChild(true,true,model->index(iRow,0,index),iName,iType,iMembers,iVal);
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

    QFile file(QDir::tempPath()+"/vascular_network-"+rootName+".xsd");
    if (file.open(QIODevice::ReadOnly)) {
            resultString.append(QString::fromLatin1(file.readAll()));    
            file.close();
            cachedSchema = true;
    }   
    if(!cachedSchema){
       QueryExecuter *query = new QueryExecuter; 
       query->setQueryFile(":xml/getSubSchema.xq");
       query->setDocFile(requestSchemaPath);
       query->addVariable("rootName",rootName);
       query->addVariable("rootParent",rootParent);
       query->evaluateQuery();
       resultString.append(query->getQueryResult());
       QFile ofile(QDir::tempPath()+"/vascular_network-"+rootName+".xsd");
       if (ofile.open(QIODevice::WriteOnly)) {
           ofile.write(resultString);
           ofile.close();
       }   
    }

    requestSchema.load(resultString);
}

bool DataCollector::validateDom(QString DomString)
{
    if(!requestSchema.isValid())
        return false;
 
    MessageHandler messageHandler;
    requestSchema.setMessageHandler(&messageHandler);

    QByteArray XMLByteContent;
    XMLByteContent.append(DomString);
    QXmlSchemaValidator validator(requestSchema);
    bool valid = false;
    if (validator.validate(XMLByteContent))
        valid = true;
    QString stylebase="QLabel {border-radius:3px;padding-left:2px;border:0px solid #515151;";
    const QString styleSheet = QString(stylebase + "background: %1;}").arg(valid ? "#51b151" : "#b15151");
    domStatusLabel->setStyleSheet(styleSheet);
    if(valid){
        domStatusLabel->setText("Data is valid");
    } else {
        domStatusLabel->setText(messageHandler.statusMessage());

    }
    return valid;
} 
