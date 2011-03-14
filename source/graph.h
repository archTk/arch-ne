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

#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QMap>
#include <QPoint>
#include <QVector>

class Graph : public QObject
{
    Q_OBJECT

public:
    explicit Graph(QObject *parent = 0);

    bool dataInGraph();
    void setNodes(QVector<int> theNodes);
    void setEdges(QMap<int, QPoint> theEdges);
    QVector<int> getNodes();
    QMap<int, QPoint> getEdges();

    int createNode();
    void insertNodeFromXML(QString nodeIdString);

    int createEdge(int nodeA, int nodeB);
    void insertEdgeFromXML(QString edgeIdString, QString node1_idString, QString node2_idString);

    void deleteNode(int node);
    void deleteEdge(int edge);
    void substituteNodeInEdges(int oldNode, int newNode);
    QVector<QPoint> edgesWithNode(int node);

    void createEdgeFromXML(QString edgeIdString, QString node1_idString, QString node2_idString);

    QPoint getNodesOfEdge(int idEdge);
    int getEdgeWithNode(int nodeId);
    void setEdgeFirstNode(int idEdge, int idNode);
    void setEdgeSecondNode(int idEdge, int idNode);

    QVector<int> getNodesIds();
    QVector<int> getEdgesIds();

    void clear();

    QString getEdgeNode1_id(int edgeId);
    QString getEdgeNode2_id(int edgeId);

signals:
    void nodeDeleted(int nodeId);
    void edgeDeleted(int edgeId);

public slots:

private:
    int uniqueNodeKey();
    int uniqueEdgeKey();
    int nodesOccurrence(int node);

    QMap<int, int> nodes;
    QMap<int, QPoint> edges;

    int incrementalNode;
    int incrementalEdge;
};

#endif // GRAPH_H
