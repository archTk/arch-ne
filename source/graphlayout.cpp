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

#include "graphlayout.h"
#include "graph.h"

GraphLayout::GraphLayout(QObject *parent) :
    QObject(parent)
{
}

void GraphLayout::setNodesLayout(QMap<int, QPointF> theNodesLayout)
{
    nodesLayout = theNodesLayout;
    QMapIterator<int, QPointF> nodesIterator(nodesLayout);
    while (nodesIterator.hasNext()) {
        nodesIterator.next();
        nodesMovable.insert(nodesIterator.key(), true);
    }
}

void GraphLayout::setEdgesLayout(QMap<int, QPointF> theEdgesLayout)
{
    edgesLayout = theEdgesLayout;
}

QMap<int, QPointF> GraphLayout::getNodesLayout()
{
    return nodesLayout;
}

QMap<int, QPointF> GraphLayout::getEdgesLayout()
{
    return edgesLayout;
}

QPointF GraphLayout::getEdgeLayoutParameters(int edgeId)
{
    return edgesLayout.value(edgeId);
}

void GraphLayout::createNode(int nodeId, QPointF pos)
{
   nodesLayout.insert(nodeId, pos);
   nodesMovable.insert(nodeId, true);
}

void GraphLayout::insertNodeFromXML(QString nodeIdString, QString xCoord, QString yCoord)
{
    int nodeKey = nodeIdString.toInt();
    float x = xCoord.toFloat();
    float y = yCoord.toFloat();
    QPointF temp(x,y);

    nodesLayout.insert(nodeKey, temp);
    nodesMovable.insert(nodeKey, true);
}

void GraphLayout::createEdge(int edgeId)
{
    QPointF temp(0.5, 0);
    edgesLayout.insert(edgeId, temp);
}

void GraphLayout::insertEdgeFromXML(QString edgeIdString, QString attrS, QString attrD)
{
    int edgeKey = edgeIdString.toInt();
    float s = attrS.toFloat();
    float d = attrD.toFloat();
    QPointF temp(s,d);

    edgesLayout.insert(edgeKey, temp);
}

void GraphLayout::deleteNode(int nodeId)
{
    nodesLayout.remove(nodeId);
    nodesMovable.remove(nodeId);
}

void GraphLayout::deleteNodeFromGraph(int nodeId)
{
    deleteNode(nodeId);
}

void GraphLayout::deleteEdge(int edgeId)
{
    edgesLayout.remove(edgeId);
}

void GraphLayout::deleteEdgeFromGraph(int edgeId)
{
    deleteEdge(edgeId);
}

QPointF GraphLayout::getNodePosition(int nodeId)
{
    return nodesLayout.value(nodeId);
}

bool GraphLayout::getNodeMovStatus(int nodeId)
{
    return nodesMovable.value(nodeId);
}

void GraphLayout::setNodePosition(int nodeId, QPointF pos)
{
    nodesLayout[nodeId] = pos;
}

void GraphLayout::setNodeMovStatus(int nodeId, bool movStatus)
{
    nodesMovable[nodeId] = movStatus;
}

void GraphLayout::setEdgeLayoutParameters(int edgeId,float v, float d)
{
    edgesLayout[edgeId].setX(v);
    edgesLayout[edgeId].setY(d);
}

void GraphLayout::clear()
{
    nodesLayout.clear();
    edgesLayout.clear();
    nodesMovable.clear();
}
