////////////////////////////////////////////////////////////////////////////////
//
//   Copyright 2011 Orobix Srl & Mario Negri Institute
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

#include <QtXml/QDomNode>
#include <QtXml/QDomElement>

#include "graphproperties.h"
#include "graph.h"
#include "graphlayout.h"
#include "graphproperties.h"

#include <iostream>
#include <QTextStream>
using namespace std;

QTextStream graphProOut(stdout);


GraphProperties::GraphProperties(QObject *parent) :
    QObject(parent)
{
}

void GraphProperties::clear()
{
    nodesNames.clear();
    nodesProperties.clear();
    edgesNames.clear();
    edgesProperties.clear();
    //caseProp.clear();
    superedges.clear();
    transformations.clear();
}

void GraphProperties::createNode(int nodeId, QString attrName, QString nodeProperties)
{
    nodesNames.insert(nodeId, attrName);
    if (nodeProperties.isEmpty()) {
        QString temp;
        QString id;
        id.setNum(nodeId);
        temp = "<node id=\"" + id + "\"/>\n";
        nodesProperties.insert(nodeId, temp);
    } else {
        nodesProperties.insert(nodeId, nodeProperties);
    }
}

void GraphProperties::createEdge(int edgeId, QString node1_idString, QString node2_idString)
{
    QString temp;
    QString id;
    id.setNum(edgeId);
    temp = "<edge id=\"" + id + "\" node1_id=\"" + node1_idString + "\" node2_id=\"" + node2_idString + "\"/>\n";
    edgesProperties.insert(edgeId, temp);
}

void GraphProperties::insertEdgeFromXML(QString edgeId, QString attrName, QString edgeProperties)
{
    int id;
    id = edgeId.toInt();
    edgesNames.insert(id, attrName);
    edgesProperties.insert(id, edgeProperties);
}


void GraphProperties::deleteNode(int nodeId)
{
    nodesProperties.remove(nodeId);
    nodesNames.remove(nodeId);
}

void GraphProperties::deleteNodeFromGraph(int nodeId)
{
    deleteNode(nodeId);
}

void GraphProperties::deleteEdge(int edgeId)
{
    edgesProperties.remove(edgeId);
    edgesNames.remove(edgeId);
}

void GraphProperties::deleteEdgeFromGraph(int edgeId)
{
    deleteEdge(edgeId);
}

void GraphProperties::setNodeProperties(int nodeId, QString nodeProperties)
{
    nodesProperties[nodeId] = nodeProperties;
}

void GraphProperties::setEdgeProperties(int edgeId, QString edgeProperties)
{
    edgesProperties[edgeId] = edgeProperties;
}

void GraphProperties::setNodesProperties(QMap<int, QString> theNodesProperties)
{
    nodesProperties = theNodesProperties;
}

void GraphProperties::setEdgesProperties(QMap<int, QString> theEdgesProperties)
{
    edgesProperties = theEdgesProperties;
}

//void GraphProperties::setCase(QString theCase)
//{
//    caseProp = theCase;
//}

void GraphProperties::setSuperedges(QString theSuperedges)
{
    superedges = theSuperedges;
}

void GraphProperties::setTransformations(QString theTransformations)
{
    transformations = theTransformations;
}

QString GraphProperties::getNodeProperties(int nodeId)
{
    return nodesProperties.value(nodeId);
}

QString GraphProperties::getNodeName(int nodeId)
{
    return nodesNames.value(nodeId);
}

void GraphProperties::setNodeName(int nodeId, QString theName)
{
    nodesNames.insert(nodeId, theName);
}

QString GraphProperties::getEdgeName(int edgeId)
{
    return edgesNames.value(edgeId);
}

void GraphProperties::setEdgeName(int edgeId, QString theName)
{
    edgesNames.insert(edgeId, theName);
}

void GraphProperties::setEdgeType(int edgeId, QString theType)
{
    QDomDocument doc("element");
    doc.setContent(edgesProperties.value(edgeId));

    QDomNodeList edgeList = doc.elementsByTagName("edge");
    QDomNode edgeDomNode = edgeList.at(0);

    QDomElement edge = edgeDomNode.toElement();
    if (!edge.isNull()) {
        edge.setAttribute("type", theType);
    }

    QString modifiedDom = doc.toString(4);

    edgesProperties.insert(edgeId, modifiedDom);
}

QString GraphProperties::getEdgeProperties(int edgeId)
{
    return edgesProperties.value(edgeId);
}

//QString GraphProperties::getCase()
//{
//    return caseProp;
//}

QString GraphProperties::getSuperedges()
{
    return superedges;
}

QString GraphProperties::getTransformations()
{
    return transformations;
}

QMap<int, QString> GraphProperties::getNodesProperties()
{
    return nodesProperties;
}

//QMap<int, QString> GraphProperties::getEdgesNames()
//{
//    return edgesNames;
//}

//QMap<int, QString> GraphProperties::getEdgesTypes()
//{
//    return edgesTypes;
//}

QMap<int, QString> GraphProperties::getEdgesProperties()
{
    return edgesProperties;
}
