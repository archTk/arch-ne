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

#ifndef DATACOLLECTOR_H
#define DATACOLLECTOR_H

#include <QWidget>
#include <QDomDocument>
#include <QtXmlPatterns>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include "ui_datacollector.h"

class QLabel;
class QTreeView;
class QDomElement;
class QDomDocument;
class QXmlSchema;

class DataCollector : public QWidget, private Ui::DataCollector
{
    Q_OBJECT

public:
    DataCollector(QString cookie,QString XMLString,QVector<QString> hiddenItems,QVector<QString> readOnlyItems,QWidget *parent=0);
    void setHiddenItems(QVector<QString> theHiddenItems);
    void setElementDom(QString theElementDom);
    void setRequestSchemaPath(QString path);
    void setElementCookie(QString theElementCookie);
    void setReadOnlyItems(QVector<QString> theReadOnlyItems);

public slots:
    void editDomToggled();
    void buttonApplyClicked();
    void buttonOkClicked();
    void resetClicked();
    void undoClicked();
    void redoClicked();
    void cancelClicked();
    void showDomToggled();
    void removeElement();
    void updateTools();
    void treeViewDataChanged();
    void viewClicked(QModelIndex index);

signals:
    void applyClicked(QString cookie,QString XMLString);  
    void okButtonClicked(QString cookie,QString XMLString);  
    void deleteItself();

private:
    void setupData();
    void setRequestSchema();
    bool validateDom(QString DomString);
    void TreeToDom(QDomDocument *doc,QDomElement iDomElement, QModelIndex index);
    void DomToTree(QDomElement iDomElement,QString iParentType,QModelIndex index,bool required=false);
    void removeRow(QModelIndex index);
    int insertChild(bool isAttribute,bool required,QModelIndex index,QString rowFirstCol,QString type,QStringList members,QString value="");
    QList<QStringList> getElementInfo(QString elementName,QString parentName);
    QStringList fixElementInfo(QStringList info);
    QStringList getEnumeration(QString iType);
    void updateDomStatus();
    void loadStack(int stackId);
    void setQueryStrings();
    void updateLeftMembers(QModelIndex index);
    int getInsertionRow(QModelIndex index, QString iName);
    void updateHistory();

    QVector<QString> requestHiddenItems;
    QVector<QString> requestReadOnlyItems;
    QString requestCookie;
    QString requestDom;   
    QString requestSchemaPath;

    QString workingDom;   
    QString rootName;   
    QString rootSchemaType;   
    QString rootParent;   
    QString getElementInfoXqString;
    QString getEnumerationXqString;
    QXmlSchema requestSchema;
    bool domStatus;
    bool avoidUpdateFlag;
    bool newStack;
    QStringList undoStack;
    int currentStack;
    bool autoFill;
};

#endif

