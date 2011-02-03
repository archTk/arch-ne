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

#ifndef QUERYEXECUTER_H
#define QUERYEXECUTER_H

#include <QWidget>
#include <QMap>

class QString;

class QueryExecuter : public QWidget
{
    Q_OBJECT

public:
    QueryExecuter(QWidget *parent=0);
    void setQueryFile(const QString queryFileName);
    void setQueryString(const QString theQueryString);
    void setDocFile(const QString docFileName);
    void addVariable(const QString varName, const QString varValue);
    void evaluateQuery();
    void useQueryFromFile();
    void useQueryFromString();
    QString getQueryResult();

public slots:

signals:

private:
    QMap<QString, QString > queryFiles;
    QMap<QString, QString > queryVariables;
    QString queryResult;
    QString queryString;
    bool queryFromFile;
};

#endif
