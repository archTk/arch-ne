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

#ifndef GRAPHLAYOUT_H
#define GRAPHLAYOUT_H

#include <QObject>
#include <QMap>
#include <QPointF>
#include <QSharedPointer>

class Graph;

class GraphLayout : public QObject
{
    Q_OBJECT

public:
    explicit GraphLayout(QObject *parent = 0);

    void setNodesLayout(QMap<int, QPointF> theNodesLayout);
    void setEdgesLayout(QMap<int, QPointF> theEdgesLayout);
    QMap<int, QPointF> getNodesLayout();
    QMap<int, QPointF> getEdgesLayout();
    QPointF getEdgeLayoutParameters(int edgeId);

    void createNode(int nodeId, QPointF pos);
    void insertNodeFromXML(QString nodeIdString, QString xCoord, QString yCoord);

    void createEdge(int edgeId);
    void insertEdgeFromXML(QString edgeIdString, QString attrS, QString attrD);

    void deleteNode(int nodeId);
    void deleteEdge(int edgeId);

    QPointF getNodePosition(int nodeId);
    bool getNodeMovStatus(int nodeId);

    void setNodePosition(int nodeId, QPointF pos);
    void setNodeMovStatus(int nodeId, bool movStatus);
    void setEdgeLayoutParameters(int edgeId, float v, float d);

    void clear();

signals:

public slots:
    void deleteNodeFromGraph(int nodeId);

private:
    QMap<int, QPointF> nodesLayout;
    QMap<int, QPointF> edgesLayout;
    QMap<int, bool> nodesMovable;
};

#endif // GRAPHLAYOUT_H
