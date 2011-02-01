////////////////////////////////////////////////////////////////////////////////
//
//   Copyright 2011 Mario Negri Institute & Orobix Srl
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////

#include <QtGui>
#include <QtXmlPatterns>

#include "queryexecuter.h"

QueryExecuter::QueryExecuter(QWidget *parent)
   : QWidget(parent)
{
    useQueryFromFile();
}

void QueryExecuter::setQueryFile(const QString queryFileName)
{
    queryFiles.insert("queryFileName", queryFileName);
}

void QueryExecuter::setQueryString(const QString theQueryString)
{
    queryString = theQueryString;
}

void QueryExecuter::useQueryFromFile()
{
    queryFromFile = 1;
}

void QueryExecuter::useQueryFromString()
{
    queryFromFile = 0;
}


void QueryExecuter::setDocFile(const QString docFileName)
{
    queryFiles.insert("docFileName", docFileName);
}

QString QueryExecuter::getQueryResult()
{
    return queryResult;
}

void QueryExecuter::addVariable(const QString varName, const QString varValue)
{
    queryVariables.insert(varName, varValue);
}

void QueryExecuter::evaluateQuery()
{
    if (queryFromFile){
        QFile queryFile;
        queryFile.setFileName(queryFiles.value("queryFileName"));
        queryFile.open(QIODevice::ReadOnly);
        setQueryString(QString::fromLatin1(queryFile.readAll()));
    } 

    QFile docFile ;
    docFile.setFileName(queryFiles.value("docFileName"));
    docFile.open(QIODevice::ReadOnly);

    QByteArray outArray;
    QBuffer buffer(&outArray);
    buffer.open(QIODevice::ReadWrite);

    QXmlQuery query;
    query.bindVariable("inputDocument", &docFile);

    QMapIterator<QString, QString > i(queryVariables);
    while (i.hasNext()) {
        i.next();
        query.bindVariable(i.key(), QVariant(i.value()));
    }

    query.setQuery(queryString);
    if (!query.isValid())
        return;

    QXmlFormatter formatter(query, &buffer);
    if (!query.evaluateTo(&formatter))
        return;

    buffer.close();
    queryResult = QString::fromUtf8(outArray.constData());
    docFile.close();
}
