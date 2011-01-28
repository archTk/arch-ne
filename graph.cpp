////////////////////////////////////////////////////////////////////////////////
//
//   Copyright 2011 - ARCH vascular Netwrok Editor workgroup
//
//                    Orobix Srl -- www.orobix.com
//
//   Licensed under the archTk License, Version 1.0 (the "License");
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
////////////////////////////////////////////////////////////////////////////////

#include "graph.h"
#include <QMapIterator>

#include <iostream>
#include <QTextStream>
using namespace std;

//QTextStream graphout(stdout);


Graph::Graph(QObject *parent) :
    QObject(parent)
{
    incrementalNode = 1;
    incrementalEdge = 1;
}

bool Graph::dataInGraph()
{
    if (!nodes.isEmpty()) {
        return true;
    }

    return false;
}

void Graph::setNodes(QVector<int> theNodes)
{
    nodes.clear();

    for (int i=0; i < theNodes.count(); i++) {
        nodes.insert(theNodes[i], 0);
    }
}

void Graph::setEdges(QMap<int, QPoint> theEdges)
{
    edges = theEdges;
}

QVector<int> Graph::getNodesIds()
{
    QVector<int> result;

    QMapIterator<int, int> nodesIterator(nodes);
    while (nodesIterator.hasNext()) {
        nodesIterator.next();
        result.append(nodesIterator.key());
    }

    return result;
}

QVector<int> Graph::getNodes()
{
    QVector<int> result;

    QMapIterator<int, int> nodesIterator(nodes);
    while (nodesIterator.hasNext()) {
        nodesIterator.next();
        result.append(nodesIterator.key());
    }

    return result;
}


QMap<int, QPoint> Graph::getEdges()
{
    return edges;
}

int Graph::createNode()
{
    int nodeKey = uniqueNodeKey();

    nodes.insert(nodeKey, 0);

    return nodeKey;
}

void Graph::insertNodeFromXML(QString nodeIdString)
{
    int nodeKey = nodeIdString.toInt();
    nodes.insert(nodeKey, 0);
}

int Graph::createEdge(int nodeA, int nodeB)
{
    int edgeKey = uniqueEdgeKey();
    QPoint temp(nodeA, nodeB);
    edges.insert(edgeKey, temp);

    return edgeKey;
}

void Graph::insertEdgeFromXML(QString edgeIdString, QString node1_idString, QString node2_idString)
{
    int edgeKey = edgeIdString.toInt();
    int node1_id = node1_idString.toInt();
    int node2_id = node2_idString.toInt();

    QPoint temp(node1_id, node2_id);
    edges.insert(edgeKey, temp);
}

int Graph::uniqueNodeKey()
{
    while (nodes.contains(incrementalNode)) {
               incrementalNode++;
           }
    return incrementalNode;
}

int Graph::uniqueEdgeKey()
{
    while (edges.contains(incrementalEdge)) {
                incrementalEdge++;
            }
    return incrementalEdge;
}

void Graph::deleteNode(int node)
{
    if (!nodes.contains(node)) {
        return;
    }

    nodes.remove(node);
}

void Graph::deleteEdge(int edge)
{
    if (nodesOccurrence((edges.value(edge)).x()) == 1) {
        deleteNode((edges.value(edge)).x());
        emit nodeDeleted((edges.value(edge)).x());
    }

    if (nodesOccurrence((edges.value(edge)).y()) == 1) {
        deleteNode((edges.value(edge)).y());
        emit nodeDeleted((edges.value(edge)).y());
    }

    edges.remove(edge);
}

int Graph::nodesOccurrence(int node)
{
    int occur;
    occur = 0;

    QMapIterator<int, QPoint> edgesIterator(edges);
    while (edgesIterator.hasNext()) {
        edgesIterator.next();
        if ((edgesIterator.value()).x() == node || (edgesIterator.value()).y() == node )
            occur++;
    }
    return occur;
}

QVector<QPoint> Graph::edgesWithNode(int node)
        //
        // It checks if node belongs to edges.
        //
        // Returns a QVector of QPoint(idE, xy):
        //      idE is the id of the edge owning the node
        //      xy is 1 if node is the first node of the edge, 2 if node is the second
        //
{
    QVector<QPoint> result;
    QMapIterator<int, QPoint> edgesIterator(edges);

    while (edgesIterator.hasNext()) {
        edgesIterator.next();
        if (edgesIterator.value().x() == node) {
            QPoint temp;
            temp.setX(edgesIterator.key());
            temp.setY(1);
            result.append(temp);
        } else if (edgesIterator.value().y() == node) {
            QPoint temp;
            temp.setX(edgesIterator.key());
            temp.setY(2);
            result.append(temp);
        }
    }

    return result;
}

void Graph::substituteNodeInEdges(int oldNode, int newNode)
{
    QMapIterator<int, QPoint> edgesIterator(edges);

    while (edgesIterator.hasNext()) {
        edgesIterator.next();
        if (edgesIterator.value().x() == oldNode) {
            edges[edgesIterator.key()].setX(newNode);
        } else if (edgesIterator.value().y() == oldNode) {
            edges[edgesIterator.key()].setY(newNode);
        }
    }
}

QPoint Graph::getNodesOfEdge(int idEdge)
{
    QPoint result;
    result.setX(edges.value(idEdge).x());
    result.setY(edges.value(idEdge).y());
    return result;
}

int Graph::getEdgeWithNode(int nodeId)
{
    QMapIterator<int, QPoint> edgesIterator(edges);
    bool found;
    found = false;
    int result;
    result = -1;

    while (edgesIterator.hasNext() && !found) {
        edgesIterator.next();
        if (edgesIterator.value().x() == nodeId || edgesIterator.value().y() == nodeId) {
            found = true;
            result = edgesIterator.key();
        }
    }

    return result;
}

void Graph::setEdgeFirstNode(int idEdge, int idNode)
{
    edges[idEdge].setX(idNode);
}

void Graph::setEdgeSecondNode(int idEdge, int idNode)
{
    edges[idEdge].setY(idNode);
}

QVector<int> Graph::getEdgesIds()
{
    QVector<int> result;
    
    QMapIterator<int, QPoint> edgesIterator(edges);

    while (edgesIterator.hasNext()) {
        edgesIterator.next();
        result.append(edgesIterator.key());
    }

    return result;
}

void Graph::clear()
{
    nodes.clear();
    edges.clear();
    incrementalNode = 1;
    incrementalEdge = 1;
}

QString Graph::getEdgeNode1_id(int edgeId)
{
    QString result;
    int node1_id = edges[edgeId].x();
    result.setNum(node1_id);

    return result;
}

QString Graph::getEdgeNode2_id(int edgeId)
{
    QString result;
    int node1_id = edges[edgeId].y();
    result.setNum(node1_id);

    return result;
}
