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
    DataCollector(QString &cookie,QString &XMLString,QVector<QString> &hiddenItems,QVector<QString> &readOnlyItems,QWidget *parent=0);
    void setRequestSchemaPath(QString &path);

public slots:
    void editDomToggled();
    void buttonApplyClicked();
    void buttonOkClicked();
    void resetClicked();
    void undoClicked();
    void redoClicked();
    void cancelClicked();
    void showDomToggled();
    void updateTools();
    void treeViewDataChanged();
    void viewClicked(const QModelIndex &index);

signals:
    void applyClicked(QString cookie,QString XMLString);  
    void okButtonClicked(QString cookie,QString XMLString);  
    void deleteItself();
    void setErrorLine(int line);

private:
    bool getConfirmation();
    void setupData();
    void setRequestSchema();
    void updateDomStatus(bool newStack);
    void setQueryStrings();
    void updateHistory();
    void setElementDom(QString &theElementDom);
    bool validateDom(QString &DomString);
    void TreeToDom(QDomDocument *doc,QDomElement iDomElement, QModelIndex index);
    void DomToTree(QDomElement iDomElement,QString iParentType,QModelIndex index,bool required=false);
    void removeRow(QModelIndex index);
    int insertChild(bool isAttribute,bool required,QModelIndex index,QString rowFirstCol,
                    QString type,QStringList members,QStringList enumeration,QString value="");
    QList<QStringList> getElementInfo(QString elementName,QString parentType);
    QStringList fixElementInfo(QList<QString> info);
    QStringList getEnumeration(const QString &iType);
    void loadStack(const int &stackId);
    void updateLeftMembers(QModelIndex &index);
    int getInsertionRow(QModelIndex &index, QString &iName);
    void highlightError(const int &line);

    QVector<QString> requestHiddenItems;
    QVector<QString> requestReadOnlyItems;
    QString requestCookie;
    QString requestDom;   
    QString requestSchemaPath;
    QString workingDom;   
    QString rootName;   
    QString rootType;   
    QString getElementInfoXqString;
    QString getEnumerationXqString;
    QXmlSchema requestSchema;
    bool domStatus;
    bool avoidUpdateFlag;
    QStringList undoStack;
    int currentStack;
    bool autoFill;
    int lineCount;
};

#endif

