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

#include "networkproperties.h"

NetworkProperties::NetworkProperties(QObject *parent) :
    QObject(parent)
{
}

void NetworkProperties::clear()
{
    BCXML.clear();
    //SPXML.clear();
    //patientInfoXML.clear();
}

void NetworkProperties::setBCXML(QString theBCXML)
{
    BCXML = theBCXML;
}

QString NetworkProperties::getBCXML()
{
    return (BCXML);
}

/*void NetworkProperties::setSPXML(QString theSPXML)
{
    SPXML = theSPXML;
}*/

/*QString NetworkProperties::getSPXML()
{
    return SPXML;
}*/

/*void NetworkProperties::setPatientInfoXML(QString thePatientInfoXML)
{
    patientInfoXML = thePatientInfoXML;
}*/

/*QString NetworkProperties::getPatientInfoXML()
{
    return patientInfoXML;
}*/
