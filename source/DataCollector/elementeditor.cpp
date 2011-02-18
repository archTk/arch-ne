#include <QtGui>
#include <QtGui/QTreeView>

#include <QStandardItemModel>
#include <QStandardItem>

#include "elementeditor.h"
#include "queryexecuter.h"

ElementEditor::ElementEditor(QWidget *parent)
   : QWidget(parent)
{
    QTreeView *treeView = new QTreeView;
    QStandardItemModel *standardModel = new QStandardItemModel ;
    QStandardItem *rootNode = standardModel->invisibleRootItem();

    QStringList networkGraphNodes = getElementsList("edgeType");
    QListIterator<QString> i(networkGraphNodes);
    while (i.hasNext()){
        QString itemName = i.next();
        QStandardItem *standardItem = new QStandardItem(itemName);
        rootNode->appendRow(standardItem);
        QString itemType = itemName.append("Type");
    
        QStringList subAttributes = getElementsList(itemType);
        QListIterator<QString> j(subAttributes);
        while (j.hasNext()){
             QString subItemName = j.next(); 
             QStandardItem *subStandardItem = new QStandardItem(subItemName);
             standardItem->appendRow(subStandardItem);
        }
        QStringList subElements = getAttributesList(itemType);
        QListIterator<QString> k(subElements);
        while (k.hasNext()){
             QString subItemName = k.next(); 
             QStandardItem *subStandardItem = new QStandardItem(subItemName);
             standardItem->appendRow(subStandardItem);
        }
     
    }

    treeView->setModel(standardModel);
    treeView->expandAll();

    QVBoxLayout *layout = new QVBoxLayout;  
    layout->addWidget(treeView);

    setLayout(layout);
}

QStringList ElementEditor::getElementsList(QString itemName)
{
    QueryExecuter *query = new QueryExecuter; 
    query->setQueryFile(":xml/getelementslist.xq");
    query->setDocFile(":xml/schema.xsd");
    query->addVariable("itemName",itemName);
    query->evaluateQuery();
    
    QString resultString = query->getQueryResult();
    QStringList resultList = resultString.split(" ");
    return resultList;
}

QStringList ElementEditor::getAttributesList(QString itemName)
{
    QueryExecuter *query = new QueryExecuter; 
    query->setQueryFile(":xml/getattributeslist.xq");
    query->setDocFile(":xml/schema.xsd");
    query->addVariable("itemName",itemName);
    query->evaluateQuery();
    
    QString resultString = query->getQueryResult();
    QStringList resultList = resultString.split(" ");
    return resultList;
}

