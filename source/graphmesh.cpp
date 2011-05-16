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

#include "graphmesh.h"

#include <iostream>
#include <QTextStream>
using namespace std;

QTextStream graphMout(stdout);

GraphMesh::GraphMesh(QObject *parent) :
    QObject(parent)
{
    //QString temp;
    //temp.clear();
    //nodesMType.insert(1, temp);

    incrementalNode = 1;
    incrementalEdge = 1;
}

bool GraphMesh::dataInGraphMesh()
{
    if (!nodesMType.isEmpty() || !edgesMS.isEmpty()) {
        return true;
    }

    return false;
}

void GraphMesh::createNodeFromXML(QString attrNodeId, QString attrId, QString attrType, QString attrMeshnode1,
                                  QString attrMeshnode2, QString attrMeshnode3, QString parameters)
{
    int nodeId = attrNodeId.toInt();

    if (!nodesMType.contains(nodeId)) {
        QString type;
        nodesMType.insert(nodeId, type);        // These are QString: e.g. at line 38 "append" works on QString.
        QString param;
        nodesMParameters.insert(nodeId, param);
        QVector<int> meshnodes;
        nodesMMeshnodesId.insert(nodeId, meshnodes);
        int nodeMeshId = attrId.toInt();
        nodesMElementId.insert(nodeId, nodeMeshId);
    }

    //nodesMElementId[nodeId].append(attrId);
    nodesMType[nodeId].append(attrType);
    int meshnode1 = attrMeshnode1.toInt();
    int meshnode2 = attrMeshnode2.toInt();
    nodesMMeshnodesId[nodeId] << meshnode1 << meshnode2;
    if (!attrMeshnode3.isEmpty()) {
        int meshnode3 = attrMeshnode3.toInt();
        nodesMMeshnodesId[nodeId] << meshnode3;
    }
    nodesMParameters[nodeId].append(parameters);
}

void GraphMesh::createEdgeFromXML(QString attrEdgeId, QString attrId, QString attrType, QString attrMeshnode1,
                                  QString attrMeshnode2, QString s1String, QString parameters)
{
    int edgeId = attrEdgeId.toInt();

    if (!edgesMElementsId.contains(edgeId)) {
        QVector<int> elementId;
        edgesMElementsId.insert(edgeId, elementId);
        QVector<QString> type;
        edgesMTypes.insert(edgeId, type);
        QVector<QPoint> meshnodes;
        edgesMMeshnodesId.insert(edgeId, meshnodes);
        QVector<float> s;
        edgesMS.insert(edgeId, s);
        QVector<QString> param;
        edgesMParameters.insert(edgeId, param);
    }

    edgesMElementsId[edgeId].append(attrId.toInt());
    edgesMTypes[edgeId].append(attrType);
    QPoint temp(attrMeshnode1.toInt(), attrMeshnode2.toInt());
    edgesMMeshnodesId[edgeId].append(temp);
    edgesMS[edgeId].append(s1String.toFloat());
    edgesMParameters[edgeId].append(parameters);
}

void GraphMesh::applyDefaultMesh(QVector<int> edgesIds)
{
    for (int i = 0; i < edgesIds.size(); i++) {
        if (!edgesMElementsId.contains(edgesIds[i])) {
            QVector<int> elementId;
            edgesMElementsId.insert(edgesIds[i], elementId);
            QVector<QString> type;
            edgesMTypes.insert(edgesIds[i], type);
            QVector<QPoint> meshnodes;
            edgesMMeshnodesId.insert(edgesIds[i], meshnodes);
            QVector<float> s;
            edgesMS.insert(edgesIds[i], s);
            QVector<QString> param;
            edgesMParameters.insert(edgesIds[i], param);
        }
    }

    QString type = "0D_FiveDofsV2";
    QString param = "<length unit=\"m\">"
                    "<scalar>0.04</scalar>"
                  "</length>"
                  "<radius unit=\"m\">"
                    "<scalar s=\"0.0\">0.1</scalar>"
                    "<scalar s=\"1.0\">0.1</scalar>"
                  "</radius>"
                  "<wall_thickness unit=\"m\">"
                  " <scalar s=\"0.0\">0.1</scalar>"
                  " <scalar s=\"1.0\">0.1</scalar>"
                  "</wall_thickness>"
                  "<young_modulus unit=\"Pa\">"
                  " <scalar s=\"0.0\">0.1</scalar>"
                  " <scalar s=\"1.0\">0.1</scalar>"
                  "</young_modulus>"
                  "<Resistance>"
                  " <scalar>1.0</scalar>"
                  "</Resistance>";

    if (edgesMMeshnodesId.isEmpty()) {
        QPoint tempP;
        QVector<QPoint> tempV;
        tempV << tempP;
        edgesMMeshnodesId.insert(0, tempV);
    }

    for (int i = 0; i < edgesIds.size(); i++) {
        emit setEdgeType(edgesIds[i], type);
        for (int j = 0; j < 3; j++) {
            int meshedgeId;
            meshedgeId = uniqueEdgeMKey();
            edgesMElementsId[edgesIds[i]].append(meshedgeId);
            edgesMTypes[edgesIds[i]].append(type);
            int meshnode1 = 1;
            meshnode1 = uniqueNodeMKey();
            int meshnode2 = 2;
            meshnode2 = uniqueNodeMKey();
            QPoint temp(meshnode1, meshnode2);
            edgesMMeshnodesId[edgesIds[i]].append(temp);
            edgesMS[edgesIds[i]].append(float(j) * 0.3);
            edgesMParameters[edgesIds[i]].append(param);
        }
    }
graphMout << "LOG@_GraphMesh::applyDefaultMesh()" << endl;
}

void GraphMesh::clear()
{
    nodesMType.clear();
    nodesMParameters.clear();
    nodesMMeshnodesId.clear();
    nodesMElementId.clear();

    edgesMS.clear();
    edgesMTypes.clear();
    edgesMParameters.clear();
    edgesMMeshnodesId.clear();;
    edgesMElementsId.clear();
}

QMap<int, QString> GraphMesh::getNodesMType()
{
    return nodesMType;
}

QMap<int, QString> GraphMesh::getNodesMParameters()
{
    return nodesMParameters;
}

QMap<int, QVector<int> > GraphMesh::getNodesMMeshnodesId()
{
    return nodesMMeshnodesId;
}

QMap<int, int> GraphMesh::getNodesMElementId()
{
    return nodesMElementId;
}

int GraphMesh::getNodeMElementId(int nodeId)
{
    return nodesMElementId.value(nodeId);
}

QMap<int, QVector<float> > GraphMesh::getEdgesMS()
{
    return edgesMS;
}

QMap<int, QVector<QString> > GraphMesh::getEdgesMTypes()
{
    return edgesMTypes;
}

QMap<int, QVector<QString> > GraphMesh::getEdgesMParameters()
{
    return edgesMParameters;
}

QMap<int, QVector<QPoint> > GraphMesh::getEdgesMMeshnodesId()
{
    return edgesMMeshnodesId;
}

QMap<int, QVector<int> > GraphMesh::getEdgesMElementsId()
{
    return edgesMElementsId;
}

QVector<int> GraphMesh::getEdgeMElementsId(int edgeId)
{
    return edgesMElementsId.value(edgeId);
}

QString GraphMesh::getNodeMType(int nodeId)
{
    return nodesMType.value(nodeId);
}

QVector<float> GraphMesh::getEdgeMs(int edgeId)
{
    return edgesMS.value(edgeId);
}

void GraphMesh::setNodesMType(QMap<int, QString> theNodesMType)
{
    nodesMType = theNodesMType;
}

QVector<QString> GraphMesh::getEdgeMTypes(int edgeId)
{
    return edgesMTypes.value(edgeId);
}

void GraphMesh::setNodesMParameters(QMap<int, QString> theNodesMParameters)
{
    nodesMParameters = theNodesMParameters;
}

void GraphMesh::setNodesMMeshnodesId(QMap<int, QVector<int> > theNodesMMeshnodesId)
{
    nodesMMeshnodesId = theNodesMMeshnodesId;
}

void GraphMesh::setNodesMElementId(QMap<int, int> theNodesMElementId)
{
    nodesMElementId = theNodesMElementId;
}

void GraphMesh::setEdgesMS(QMap<int, QVector<float> > theEdgesMS)
{
    edgesMS = theEdgesMS;
}

void GraphMesh::setEdgesMTypes(QMap<int, QVector<QString> > theEdgesMTypes)
{
    edgesMTypes = theEdgesMTypes;
}

void GraphMesh::setEdgesMParameters(QMap<int, QVector<QString> > theEdgesMParameters)
{
    edgesMParameters = theEdgesMParameters;
}

void GraphMesh::setEdgesMMeshnodesId(QMap<int, QVector<QPoint> > theEdgesMMeshnodesId)
{
    edgesMMeshnodesId = theEdgesMMeshnodesId;
}

void GraphMesh::setEdgesMElementsId(QMap<int, QVector<int> > theEdgesMElementsId)
{
    edgesMElementsId = theEdgesMElementsId;
}

int GraphMesh::getEdgeIdFromMElId(int meshElId)
{
    QMapIterator<int, QVector<int> > edgesMElementsIdIter(edgesMElementsId);
    while (edgesMElementsIdIter.hasNext()) {
        edgesMElementsIdIter.next();
        QVector<int> meshElsId = edgesMElementsIdIter.value();
        if (meshElsId.contains(meshElId)) {
            return edgesMElementsIdIter.key();
        }
    }

    return -1000;
}

void GraphMesh::deleteNode(int nodeId)
{
    nodesMType.remove(nodeId);
    nodesMParameters.remove(nodeId);
    nodesMMeshnodesId.remove(nodeId);
    nodesMElementId.remove(nodeId);
}

void GraphMesh::deleteEdge(int edgeId)
{
    edgesMS.remove(edgeId);
    edgesMTypes.remove(edgeId);
    edgesMParameters.remove(edgeId);
    edgesMMeshnodesId.remove(edgeId);
    edgesMElementsId.remove(edgeId);
}

int GraphMesh::uniqueNodeMKey()
{
    QMapIterator<int, QVector<int> > iter1(nodesMMeshnodesId);

    while (iter1.hasNext()) {
        iter1.next();
        QVector<int> temp1;
        temp1 = iter1.value();
        for (int i = 0; i < temp1.size(); i++) {
            if (incrementalNode <= temp1[i]) {
                incrementalNode = temp1[i] + 1;
            }
        }
    }

    QMapIterator<int, QVector<QPoint> > iter2(edgesMMeshnodesId);

    while (iter2.hasNext()) {
        iter2.next();
        QVector<QPoint> temp2;
        temp2 = iter2.value();
        for (int i = 0; i < temp2.size(); i++) {
            if (incrementalNode <= temp2[i].x()) {
                incrementalNode = (temp2[i].x() + 1);
            }
            if (incrementalNode <= temp2[i].y()) {
                incrementalNode = (temp2[i].y() + 1);
            }
        }
    }

    return incrementalNode;
}

int GraphMesh::uniqueEdgeMKey()
{
    QMapIterator<int, QVector<int> > iter(edgesMElementsId);

    while (iter.hasNext()) {
        iter.next();
        QVector<int> temp;
        temp = iter.value();
        for (int i = 0; i < temp.size(); i++) {
            if (incrementalEdge <= temp[i]) {
                incrementalEdge = (temp[i] + 1);
            }
        }
    }

    return incrementalEdge;
}
