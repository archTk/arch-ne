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

#ifndef NETWORKPROPERTIES_H
#define NETWORKPROPERTIES_H

#include <QObject>

class NetworkProperties : public QObject
{
    Q_OBJECT
public:
    explicit NetworkProperties(QObject *parent = 0);

    void clear();
    void setBCXML(QString theBCXML);
    //void setSPXML(QString theSPXML);
    void setCaseInfoXML(QString theCaseInfoXML);

    QString getBCXML();
    //QString getSPXML();
    QString getCaseInfoXML();

signals:

public slots:

private:
    QString BCXML;
    //QString SPXML;
    QString caseInfoXML;

};

#endif // NETWORKPROPERTIES_H
