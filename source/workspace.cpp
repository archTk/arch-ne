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

#include "applicationstatus.h"
#include "graph.h"
#include "graphlayout.h"
#include "graphproperties.h"
#include "graphmesh.h"
#include "unraveller.h"
#include "workspace.h"

#include <QPushButton>
#include <QTimer>
#include <QVector>
#include <QSettings>

#include <iostream>
#include <QTextStream>
using namespace std;

#include <math.h>

QTextStream wsout(stdout);

Workspace::Workspace(QObject *parent) :
    QObject(parent)
{
    graph = new Graph;
    graphLayout = new GraphLayout;
    graphProperties = new GraphProperties;
    graphMesh = new GraphMesh;

    connect(graph, SIGNAL(nodeDeleted(int)), graphLayout, SLOT(deleteNodeFromGraph(int)));
    connect(graph, SIGNAL(nodeDeleted(int)), graphProperties, SLOT(deleteNodeFromGraph(int)));

    connect(graph, SIGNAL(edgeDeleted(int)), graphLayout, SLOT(deleteEdgeFromGraph(int)));
    connect(graph, SIGNAL(edgeDeleted(int)), graphProperties, SLOT(deleteEdgeFromGraph(int)));

    connect(graphMesh, SIGNAL(setEdgeType(int,QString)), graphProperties, SLOT(setEdgeType(int,QString)));

    unraveller = new Unraveller(graph, graphLayout, this, this);
    connect(unraveller, SIGNAL(screenOriginChanged(QPointF)), this, SLOT(setScreenOrigin(QPointF)));

    gridStatus = true;
    snapToGridStatus = true;
    gridSpacing = 20;

    distanceTolerance = gridSpacing / 2;
    selectedTool = selectE;

    edgeMinLength = 15;

    mouseButtonPressed = false;
    shiftPressed = false;

    moveSelectedEls = false;
    showSelectingArea = false;
    modifyEdgeD = false;
    lengthCheck = true;

    screenOrigin.setX(0);
    screenOrigin.setY(0);
    zoomFactor = 100.0;

    currentStatus = 0;

    inizializeHistory();

    unravelTimer = new QTimer(this);
    connect(unravelTimer, SIGNAL(timeout()), this, SLOT(unravelNet()));

    unravelCount = 0;
    unravelIter = 100;
}

void Workspace::addSegment()
{
    //QSettings settings("archTk", "ARCHNetworkEditor");
    //QString pythonPath = settings.value("pythonPath", QString()).toString();
    //QString scriptPath = settings.value("scriptPath", QString()).toString();
    //wsout << "ecco:" << endl << pythonPath << endl << scriptPath << endl;

    selectedTool = addS;

    selectedNodes.clear();
    selectedEdges.clear();
    emit updateSignal();
}

void Workspace::removeSegment()
{
    selectedTool = removeS;

    deleteSelectedEdges();
    selectedNodes.clear();
    selectedEdges.clear();
    emit updateSignal();
}

void Workspace::splitSegment()
{
    selectedTool = splitS;
    selectedNodes.clear();
    selectedEdges.clear();
    emit updateSignal();
}

void Workspace::superEdge()
{
    selectedTool = superE;
    selectedNodes.clear();
    selectedEdges.clear();
    emit updateSignal();
}

void Workspace::selectElements()
{
    selectedTool = selectE;
    selectedNodes.clear();
    selectedEdges.clear();
    emit updateSignal();
}

void Workspace::info()
{
    selectedTool = information;
    selectedNodes.clear();
    selectedEdges.clear();
    emit updateSignal();
}

void Workspace::undo()
{
    if (currentStatus > 0) {
        QVector<int> tempNodes = (*statusVector[currentStatus - 1]).getGraph().data()->getNodes();
        QMap<int, QPoint> tempEdges = (*statusVector[currentStatus - 1]).getGraph().data()->getEdges();
        (*graph).setNodes(tempNodes);
        (*graph).setEdges(tempEdges);

        QMap<int, QPointF> tempNodesLayout = (*statusVector[currentStatus - 1]).getGraphLayout().data()->getNodesLayout();
        QMap<int, QPointF> tempEdgesLayout = (*statusVector[currentStatus - 1]).getGraphLayout().data()->getEdgesLayout();
        (*graphLayout).setNodesLayout(tempNodesLayout);
        (*graphLayout).setEdgesLayout(tempEdgesLayout);

        QMap<int, QString> tempNodesProperties = (*statusVector[currentStatus - 1]).getGraphProperties().data()->getNodesProperties();
        QMap<int, QString> tempEdgesProperties = (*statusVector[currentStatus - 1]).getGraphProperties().data()->getEdgesProperties();
        QString tempCase = (*statusVector[currentStatus - 1]).getGraphProperties().data()->getCase();
        QString tempSuperedges = (*statusVector[currentStatus - 1]).getGraphProperties().data()->getSuperedges();
        QString tempTrans = (*statusVector[currentStatus - 1]).getGraphProperties().data()->getTransformations();
        (*graphProperties).setNodesProperties(tempNodesProperties);
        (*graphProperties).setEdgesProperties(tempEdgesProperties);
        (*graphProperties).setCase(tempCase);
        (*graphProperties).setSuperedges(tempSuperedges);
        (*graphProperties).setTransformations(tempTrans);

        QMap<int, QString> tempNodesMType = (*statusVector[currentStatus - 1]).getGraphMesh().data()->getNodesMType();
        QMap<int, QString> tempNodesMParameters = (*statusVector[currentStatus - 1]).getGraphMesh().data()->getNodesMParameters();
        QMap<int, QVector<int> > tempNodesMMeshnodesId = (*statusVector[currentStatus - 1]).getGraphMesh().data()->getNodesMMeshnodesId();
        QMap<int, QString> tempNodesMElementId = (*statusVector[currentStatus - 1]).getGraphMesh().data()->getNodesMElementId();
        QMap<int, QVector<float> > tempEdgesMS = (*statusVector[currentStatus - 1]).getGraphMesh().data()->getEdgesMS();
        QMap<int, QVector<QString> > tempEdgesMTypes = (*statusVector[currentStatus - 1]).getGraphMesh().data()->getEdgesMTypes();
        QMap<int, QVector<QString> > tempEdgesMParameters = (*statusVector[currentStatus - 1]).getGraphMesh().data()->getEdgesMParameters();
        QMap<int, QVector<QPoint> > tempEdgesMMeshnodesId = (*statusVector[currentStatus - 1]).getGraphMesh().data()->getEdgesMMeshnodesId();
        QMap<int, QVector<int> > tempEdgesMElementsId = (*statusVector[currentStatus - 1]).getGraphMesh().data()->getEdgesMElementsId();
        (*graphMesh).setNodesMType(tempNodesMType);
        (*graphMesh).setNodesMParameters(tempNodesMParameters);
        (*graphMesh).setNodesMMeshnodesId(tempNodesMMeshnodesId);
        (*graphMesh).setNodesMElementId(tempNodesMElementId);
        (*graphMesh).setEdgesMS(tempEdgesMS);
        (*graphMesh).setEdgesMTypes(tempEdgesMTypes);
        (*graphMesh).setEdgesMParameters(tempEdgesMParameters);
        (*graphMesh).setEdgesMMeshnodesId(tempEdgesMMeshnodesId);
        (*graphMesh).setEdgesMElementsId(tempEdgesMElementsId);

        screenOrigin = (*statusVector[currentStatus - 1]).getScreenOrigin();
        zoomFactor = (*statusVector[currentStatus - 1]).getZoom();
        gridStatus = (*statusVector[currentStatus - 1]).getGridStatus();
        snapToGridStatus = (*statusVector[currentStatus - 1]).getSnapToGridStatus();
        gridSpacing = (*statusVector[currentStatus - 1]).getGridSpacing();
        selectedNodes = (*statusVector[currentStatus - 1]).getSelectedNodes();
        selectedEdges = (*statusVector[currentStatus - 1]).getSelectedEdges();

        currentStatus--;
        supportNodes.remove("highlightingNode");
        supportEdges.remove("highlightingEdge");
        checkTooCloseNodes();
        emit updateSignal();
    }
}

void Workspace::redo()
{
    if (currentStatus == statusVector.size() - 1) {
        return;
    } else {
        currentStatus++;
        QVector<int> tempNodes = (*statusVector[currentStatus]).getGraph().data()->getNodes();
        QMap<int, QPoint> tempEdges = (*statusVector[currentStatus]).getGraph().data()->getEdges();
        (*graph).setNodes(tempNodes);
        (*graph).setEdges(tempEdges);

        QMap<int, QPointF> tempNodesLayout = (*statusVector[currentStatus]).getGraphLayout().data()->getNodesLayout();
        QMap<int, QPointF> tempEdgesLayout = (*statusVector[currentStatus]).getGraphLayout().data()->getEdgesLayout();
        (*graphLayout).setNodesLayout(tempNodesLayout);
        (*graphLayout).setEdgesLayout(tempEdgesLayout);

        QMap<int, QString> tempNodesProperties = (*statusVector[currentStatus]).getGraphProperties().data()->getNodesProperties();
        QMap<int, QString> tempEdgesProperties = (*statusVector[currentStatus]).getGraphProperties().data()->getEdgesProperties();
        QString tempCase = (*statusVector[currentStatus]).getGraphProperties().data()->getCase();
        QString tempSuperedges = (*statusVector[currentStatus]).getGraphProperties().data()->getSuperedges();
        QString tempTrans = (*statusVector[currentStatus]).getGraphProperties().data()->getTransformations();
        (*graphProperties).setNodesProperties(tempNodesProperties);
        (*graphProperties).setEdgesProperties(tempEdgesProperties);
        (*graphProperties).setCase(tempCase);
        (*graphProperties).setSuperedges(tempSuperedges);
        (*graphProperties).setTransformations(tempTrans);

        QMap<int, QString> tempNodesMType = (*statusVector[currentStatus]).getGraphMesh().data()->getNodesMType();
        QMap<int, QString> tempNodesMParameters = (*statusVector[currentStatus]).getGraphMesh().data()->getNodesMParameters();
        QMap<int, QVector<int> > tempNodesMMeshnodesId = (*statusVector[currentStatus]).getGraphMesh().data()->getNodesMMeshnodesId();
        QMap<int, QString> tempNodesMElementId = (*statusVector[currentStatus]).getGraphMesh().data()->getNodesMElementId();
        QMap<int, QVector<float> > tempEdgesMS = (*statusVector[currentStatus]).getGraphMesh().data()->getEdgesMS();
        QMap<int, QVector<QString> > tempEdgesMTypes = (*statusVector[currentStatus]).getGraphMesh().data()->getEdgesMTypes();
        QMap<int, QVector<QString> > tempEdgesMParameters = (*statusVector[currentStatus]).getGraphMesh().data()->getEdgesMParameters();
        QMap<int, QVector<QPoint> > tempEdgesMMeshnodesId = (*statusVector[currentStatus]).getGraphMesh().data()->getEdgesMMeshnodesId();
        QMap<int, QVector<int> > tempEdgesMElementsId = (*statusVector[currentStatus]).getGraphMesh().data()->getEdgesMElementsId();
        (*graphMesh).setNodesMType(tempNodesMType);
        (*graphMesh).setNodesMParameters(tempNodesMParameters);
        (*graphMesh).setNodesMMeshnodesId(tempNodesMMeshnodesId);
        (*graphMesh).setNodesMElementId(tempNodesMElementId);
        (*graphMesh).setEdgesMS(tempEdgesMS);
        (*graphMesh).setEdgesMTypes(tempEdgesMTypes);
        (*graphMesh).setEdgesMParameters(tempEdgesMParameters);
        (*graphMesh).setEdgesMMeshnodesId(tempEdgesMMeshnodesId);
        (*graphMesh).setEdgesMElementsId(tempEdgesMElementsId);

        screenOrigin = (*statusVector[currentStatus]).getScreenOrigin();
        zoomFactor = (*statusVector[currentStatus]).getZoom();
        gridStatus = (*statusVector[currentStatus]).getGridStatus();
        snapToGridStatus = (*statusVector[currentStatus]).getSnapToGridStatus();
        gridSpacing = (*statusVector[currentStatus]).getGridSpacing();
        selectedNodes = (*statusVector[currentStatus]).getSelectedNodes();
        selectedEdges = (*statusVector[currentStatus]).getSelectedEdges();

        supportNodes.remove("highlightingNode");
        supportEdges.remove("highlightingEdge");
        checkTooCloseNodes();
        emit updateSignal();
    }
}

void Workspace::showGrid()
{
    if (gridStatus) {
        gridStatus = false;
    } else {
        gridStatus = true;
    }

    emit updateSignal();
}

void Workspace::snapToGrid()
{
    if (snapToGridStatus) {
        snapToGridStatus = false;
    } else {
        snapToGridStatus = true;
    }

    emit updateSignal();
}

void Workspace::showLabels()
{
    if (showLabelsStatus) {
        showLabelsStatus = false;
    } else {
        showLabelsStatus = true;
    }

    emit updateSignal();
}

void Workspace::translate()
{
    selectedTool = trans;
    selectedNodes.clear();
    selectedEdges.clear();
    emit updateSignal();
}

void Workspace::homeView()
{
    screenOrigin.setX(0);
    screenOrigin.setY(0);
    zoomFactor = 100;
    insertActInHistory(true);
    emit updateSignal();
}

void Workspace::mousePressed(QPointF pos)
{
    QPointF validPos;
    validPos = pos;

    int nodeAId;
    int nodeBId;
    int edgeId;

    mouseButtonPressed = true;

    if (snapToGridStatus) {
        validPos = position2grid(validPos);
    }

    selectingElementsStart = pos;                                   // selectingArea first corner.
    selectingElementsSupport = selectingElementsStart;              // selectingElementsSupport is used in movingSelectedElements().

    switch (selectedTool) {
        case - 1:
            break;
        case addS:
            if (hitEl[0].x() == -1) {  // No near element.

                nodeAId = createNode(validPos);
                nodeBId = createNode(validPos);    // This node is a "support" node used for mouseMove event.

                supportNodes.insert("firstNode", nodeAId);
                supportNodes.insert("movingNode", nodeBId);

                edgeId = createEdge(nodeAId, nodeBId);
                supportEdges.insert("movingEdge", edgeId);
            } else if (hitEl[0].x() == 1) {             // Hit elements are nodes.
                nodeBId = createNode(validPos);     // This node is a "support" node use for mouseMove event.
                supportNodes.insert("movingNode", nodeBId);

                edgeId = createEdge(hitEl[0].y(), nodeBId);
                supportEdges.insert("movingEdge", edgeId);
            } else {                                    // Hit elements are edges.
                modifyEdgeD = true;
                curvingEdgeId = hitEl[0].y();
            }
            break;
        case removeS:
            if (hitEl[0].x() == 2) {              // Hit elements are edges.
                supportEdges.insert("removingEdge", hitEl[0].y());
            }
            break;
        case splitS:
            if (hitEl[0].x() == 2) {            // Hit elements are edges.
                supportEdges.insert("splittingEdge", hitEl[0].y());
            }
            break;
        case superE:
            if (hitEl[0].x() == 2) {      // Hit elements are edges.
                supportEdges.insert("superEdging", hitEl[0].y());
            }
            break;
        case selectE:
            if (!shiftPressed) {
                if (hitEl[0].x() == -1) {         // No hit elements.
                    selectedNodes.clear();
                    selectedEdges.clear();
                    showSelectingArea = true;
                } else if (hitEl[0].x() == 1) {   // Hit elements are nodes.
                    supportNodes.insert("selMovingNode", hitEl[0].y());
                    supportEdges.insert("selMovingEdge", graph->getEdgeWithNode(supportNodes.value("selMovingNode")));
                    if (selectedNodes.contains(hitEl[0].y())) {   // Node has been already selected.
                        moveSelectedEls = true;
                    } else {
                        selectedNodes.clear();
                        selectedEdges.clear();
                        selectedNodes.append(hitEl[0].y());
                        moveSelectedEls = true;
                    }
                } else {                          // Hit elements are edges.
                    supportEdges.insert("selMovingEdge", hitEl[0].y());
                    if (selectedEdges.contains(hitEl[0].y() )) {  // Edges has been already selected.
                        moveSelectedEls = true;
                    } else {
                        selectedNodes.clear();
                        selectedEdges.clear();
                        selectedEdges.append(hitEl[0].y());
                        moveSelectedEls = true;
                    }
                }
            } else {    // shiftPressed.
                showSelectingArea = true;
                if (hitEl[0].x() == 1) {            // Hit elements are nodes.
                    if (selectedNodes.contains(hitEl[0].y())) {
                        selectedNodes.remove(selectedNodes.indexOf(hitEl[0].y()));
                    } else {
                        selectedNodes.append(hitEl[0].y());
                    }
                } else if (hitEl[0].x() == 2) {     // Hit elements are edges.
                    if (selectedEdges.contains(hitEl[0].y())) {
                        selectedEdges.remove(selectedEdges.indexOf(hitEl[0].y()));
                    } else {
                        selectedEdges.append(hitEl[0].y());
                    }
                }
            }
            break;
        case information:
            break;
    }
}

void Workspace::mouseMoved(QPointF pos)
{
    QVector<int> nodesIds = getNodesIds();
    QVector<int> edgesIds = getEdgesIds();

    QVector<int> movingNodes;
    QVector<QPointF> offsets;

    QPointF selMovNodePos;
    QPointF temp;
    QPointF accepPos;

    QVector<QPoint> edgesWNode;

    QPointF nodePosition;
    QPoint nodesOfEdge;

    QPointF edgeFirstNode;
    QPointF edgeSecondNode;
    QPointF edgeMidPoint;

    QPointF A;
    QPointF B;

    QPointF posA;
    QPointF nBA;
    QPointF Xp;
    QPointF D;
    float d;
    float v;

    QPointF nodePos;
    QPainterPath path;

    QPointF validPos = pos;

    selectingElementsEnd = validPos;
    QRectF selectingRect(selectingElementsStart, selectingElementsEnd);   // selectingRect acts on the screen.

    if (snapToGridStatus) {
        validPos = position2grid(validPos);
    }

    if (!mouseButtonPressed) {
        switch (selectedTool) {
            case - 1:
                break;
            case addS:
            case selectE:
            case information:
                if (hitEl[0].x() == 1) {            // Hit elements are nodes.
                    supportEdges.remove("highlightingEdge");
                    supportNodes.insert("highlightingNode", hitEl[0].y());
                } else if (hitEl[0].x() == 2) {     // Hit elements are edges.
                    supportNodes.remove("highlightingNode");
                    supportEdges.insert("highlightingEdge", hitEl[0].y());
                } else {                            // No hit elements.
                    supportNodes.remove("highlightingNode");
                    supportEdges.remove("highlightingEdge");
                }

                emit updateSignal();
                break;
            case removeS:
            case superE:
                if (hitEl[0].x() == 2) {            // Hit elements are edges.
                    supportEdges.insert("highlightingEdge", hitEl[0].y());
                } else {
                    supportEdges.remove("highlightingEdge");
                }
                emit updateSignal();
                break;
            case splitS:
                if (hitEl[0].x() == 2) {             // nearestElement is an edge
                    nodesOfEdge = getNodesOfEdge(hitEl[0].y());

                    edgeFirstNode = getNodePosition(nodesOfEdge.x());
                    edgeSecondNode = getNodePosition(nodesOfEdge.y());
                    edgeMidPoint = getThirdPointPos(hitEl[0].y());

                    path.moveTo(edgeFirstNode);
                    path.quadTo(edgeMidPoint, edgeSecondNode);
                    path.quadTo(edgeMidPoint, edgeFirstNode);

                    if (path.length() > edgeMinLength * 2) {
                        supportEdges.insert("highlightingEdge", hitEl[0].y());
                    }
                } else {
                    supportEdges.remove("highlightingEdge");
                }
                emit updateSignal();
                break;
            }
        } else {    // mouseButtonPressed
        switch (selectedTool) {
            case - 1:
               break;
            case addS:
               if (modifyEdgeD) {
                   nodesOfEdge = graph->getNodesOfEdge(curvingEdgeId);
                   A = graphLayout->getNodePosition(nodesOfEdge.x());
                   B = graphLayout->getNodePosition(nodesOfEdge.y());

                   posA = pos - A;
                   nBA = (B - A) / norm(B - A);
                   Xp = A + nBA * dot(nBA, posA);
                   D = posA - nBA * dot(nBA, posA);
                   d = norm(D);
                   v = dot((Xp-A), nBA) / norm(B - A);

                   if (((B - A).x() * (pos - A).y() - (B - A).y() * (pos - A).x()) > 0 ) {
                       graphLayout->setEdgeLayoutParameters(curvingEdgeId, v, -d);
                   } else {
                       graphLayout->setEdgeLayoutParameters(curvingEdgeId, v, d);
                   }
               } else { // !modifyEdgeD
                   setNodePosition(supportNodes.value("movingNode"), validPos);

                   if (hitEl[0].x() == 1) { // Hit elements are nodes.
                       if (supportNodes.contains("movingNode")) {
                           if (hitEl.size() > 1) {
                               if (hitEl[0].y() != supportNodes.value("selMovingNode")) {
                                   supportNodes.insert("highlightingNode", hitEl[0].y());
                               } else {
                                   supportNodes.insert("highlightingNode", hitEl[1].y());
                               }
                           } else {
                               supportNodes.remove("highlightingNode");
                           }

                       } else { // !supportNodes.contains("movingNode")
                           supportNodes.insert("highlightingNode", hitEl[0].y());
                       }
                   }
                   else {
                       supportNodes.remove("highlightingNode");
                   }

                   if (getNodesOfEdge(supportEdges.value("movingEdge")).x() == supportNodes.value("movingNode")) {
                       if (getNodesOfEdge(supportEdges.value("movingEdge")).y() == hitEl[0].y()) {
                           supportNodes.remove("highlightingNode");
                       }

                   } else if (getNodesOfEdge(supportEdges.value("movingEdge")).y() == supportNodes.value("movingNode")) {
                       if (getNodesOfEdge(supportEdges.value("movingEdge")).x() == hitEl[0].y()) {
                           supportNodes.remove("highlightingNode");
                       }
                   }
               }
               emit updateSignal();
               break;
         case removeS:
            if (hitEl[0].x() == 2) {      // Hit elements are edges.
                 supportEdges.insert("highlightingEdge", hitEl[0].y());
                 supportEdges.insert("removingEdge", hitEl[0].y());
            } else {
                supportEdges.remove("highlightingEdge");
            }
            emit updateSignal();
            break;
         case splitS:
           if (hitEl[0].x() == 2) {      // Hit elements are edges.
               supportEdges.insert("highlightingEdge", hitEl[0].y());
               supportEdges.insert("splittingEdge", hitEl[0].y());
           } else {
               supportEdges.remove("highlightingEdge");
           }
           emit updateSignal();
           break;
         case superE:
            if (hitEl[0].x() == 2) {  // Hit elements are edges.
                supportEdges.insert("highlightingEdge", hitEl[0].y());
            } else {
                supportEdges.remove("highlightingEdge");
            }
            emit updateSignal();
            break;
         case selectE:
            if (!shiftPressed) {
                if (showSelectingArea) {  // mousePressEvent on no element.
                    for (int i = 0; i < nodesIds.size(); i++) {
                        nodePosition = getNodePosition(nodesIds[i]);
                        if (selectingRect.contains(nodePosition)) {
                            if (!selectedNodes.contains(nodesIds[i])) {
                                selectedNodes.append(nodesIds[i]);
                            }
                        } else {
                            if (selectedNodes.contains(nodesIds[i])) {
                                selectedNodes.remove(selectedNodes.indexOf(nodesIds[i]));
                            }
                        }
                    }

                    for (int j = 0; j < edgesIds.size(); j++) {
                        nodesOfEdge = getNodesOfEdge(edgesIds[j]);
                        edgeFirstNode = getNodePosition(nodesOfEdge.x());
                        edgeSecondNode = getNodePosition(nodesOfEdge.y());
                        edgeMidPoint = getThirdPointPos(edgesIds[j]);

                        path.moveTo(edgeFirstNode);
                        path.quadTo(edgeMidPoint, edgeSecondNode);
                        path.quadTo(edgeMidPoint, edgeFirstNode);
                        path.closeSubpath();
                        path = QPainterPath();

                        if (path.boundingRect().width() == 0 || path.boundingRect().height() == 0) {
                            if (selectedNodes.contains(nodesOfEdge.x()) && selectedNodes.contains(nodesOfEdge.y())) {
                                if (!selectedEdges.contains(edgesIds[j])) {
                                    selectedEdges.append(edgesIds[j]);
                                }
                            } else {
                                if (selectedEdges.contains(edgesIds[j])) {
                                    selectedEdges.remove(selectedEdges.indexOf(edgesIds[j]));
                                }
                            }
                        } else {
                            if (selectingRect.contains(path.boundingRect())) {
                                if (!selectedEdges.contains(edgesIds[j])) {
                                    selectedEdges.append(edgesIds[j]);
                                }
                            } else {
                                if (selectedEdges.contains(edgesIds[j])) {
                                    selectedEdges.remove(selectedEdges.indexOf(edgesIds[j]));
                                }
                            }
                        }
                    }
                } else if (moveSelectedEls) {  // mousePressEvent on an element.
                    if (supportNodes.contains("selMovingNode")) {  // mousePressEvent on a node.
                        if (hitEl[0].x() == 1) {    // Hit elements are nodes.
                            if (hitEl.size() > 1) {
                                if (hitEl[0].y() != supportNodes.value("selMovingNode")) {
                                    supportNodes.insert("highlightingNode", hitEl[0].y());
                                } else {
                                    supportNodes.insert("highlightingNode", hitEl[1].y());
                                }
                            } else {
                                supportNodes.remove("highlightingNode");
                            }

                            edgesWNode = graph->edgesWithNode(supportNodes.value("selMovingNode")); // This identifies the
                                                                                                    // edges owning the selMovingNode.
                            for (int i=0; i < edgesWNode.size(); i++ ) {
                                if (edgesWNode[i].y() == 1) {
                                    if (graph->getNodesOfEdge(edgesWNode[i].x()).y() == supportNodes.value("highlightingNode")) {
                                        supportNodes.remove("highlightingNode"); // To not highlight the other node of the edge owning
                                    }                                            // the selMovingNode.
                                } else {
                                    if (graph->getNodesOfEdge(edgesWNode[i].x()).x() == supportNodes.value("highlightingNode")) {
                                        supportNodes.remove("highlightingNode");
                                    }
                                }
                            }
                        } else {
                            supportNodes.remove("highlightingNode");
                        }
                    }
                    moveSelectedElements();

                    if (supportNodes.contains("selMovingNode")) {
                        edgesWNode = graph->edgesWithNode(supportNodes.value("selMovingNode")); // This identifies the edges
                                                                                                // owning the selMovingNode.
                        movingNodes = selectedNodes;                         // movingNodes are the selectedNodes +
                                                                             // the ones in selectedEdges if not
                        for (int i=0; i < selectedEdges.size(); i++) {       // already in movingNodes.
                            if (!movingNodes.contains(getNodesOfEdge(selectedEdges[i]).x())) {
                                movingNodes.append(getNodesOfEdge(selectedEdges[i]).x());
                            }
                            if (!movingNodes.contains(getNodesOfEdge(selectedEdges[i]).y())) {
                                movingNodes.append(getNodesOfEdge(selectedEdges[i]).y());
                            }
                        }

                        for (int j=0; j < selectedNodes.size(); j++) {
                            selMovNodePos = getNodePosition(supportNodes.value("selMovingNode"));
                            temp = getNodePosition(selectedNodes[j]) - selMovNodePos;
                            offsets.append(temp);
                        }

                        for (int i=0; i < edgesWNode.size(); i++ ) {
                            nodesOfEdge = getNodesOfEdge(edgesWNode[i].x());
                            if (distance(getNodePosition(nodesOfEdge.x()), getNodePosition(nodesOfEdge.y())) < edgeMinLength) {
                                if (edgesWNode[i].y() == 1) {
                                    accepPos = movingNodeAcceptablePosition(getNodePosition(nodesOfEdge.y()), pos);
                                    setNodePosition(supportNodes.value("selMovingNode"), accepPos);
                                } else {
                                    accepPos = movingNodeAcceptablePosition(getNodePosition(nodesOfEdge.x()), pos);
                                    setNodePosition(supportNodes.value("selMovingNode"), accepPos);
                                }
                            }
                        }

                        for (int k=0; k < selectedNodes.size(); k++) {
                            setNodePosition(selectedNodes[k], getNodePosition(supportNodes.value("selMovingNode")) + offsets[k]);
                        }
                    }
                    checkTooCloseNodes();
                }
            } else {    // shiftPressed
                if (showSelectingArea) {  // mousePressEvent on no element
                    for (int k = 0; k < nodesIds.size(); k++) {
                        nodePos = getNodePosition(nodesIds[k]);
                        if (selectingRect.contains(nodePos) /*&& !selectedNodes.contains(nodesIds[k])*/) {
                            if (!supportNodes4selecting.contains(nodesIds[k])) {
                                supportNodes4selecting.append(nodesIds[k]);
                            }
                        } else {
                            if (supportNodes4selecting.contains(nodesIds[k])) {
                                supportNodes4selecting.remove(supportNodes4selecting.indexOf(nodesIds[k]));
                            }
                        }
                    }
                    for (int f = 0; f < edgesIds.size(); f++) {
                        nodesOfEdge = getNodesOfEdge(edgesIds[f]);
                        edgeFirstNode = getNodePosition(nodesOfEdge.x());
                        edgeSecondNode = getNodePosition(nodesOfEdge.y());
                        edgeMidPoint = getThirdPointPos(edgesIds[f]);

                        path.moveTo(edgeFirstNode);
                        path.quadTo(edgeMidPoint, edgeSecondNode);
                        path.quadTo(edgeMidPoint, edgeFirstNode);
                        path.closeSubpath();
                        path = QPainterPath();

                        if (path.boundingRect().width() == 0 || path.boundingRect().height() == 0) {
                            if (supportNodes4selecting.contains(nodesOfEdge.x()) && supportNodes4selecting.contains(nodesOfEdge.y())) {
                                if (!supportEdges4selecting.contains(edgesIds[f])) {
                                    supportEdges4selecting.append(edgesIds[f]);
                                }
                            } else {
                                if (supportEdges4selecting.contains(edgesIds[f])) {
                                    supportEdges4selecting.remove(supportEdges4selecting.indexOf(edgesIds[f]));
                                }
                            }
                        } else {
                            if (selectingRect.contains(path.boundingRect())) {
                                if (!supportEdges4selecting.contains(edgesIds[f])) {
                                    supportEdges4selecting.append(edgesIds[f]);
                                }
                            } else {
                                if (supportEdges4selecting.contains(edgesIds[f])) {
                                    supportEdges4selecting.remove(supportEdges4selecting.indexOf(edgesIds[f]));
                                }
                            }
                        }
                    }
                }
            }
            emit updateSignal();
            break;
         case information:
            if (hitEl[0].x() == 2) {      // Hit elements are edges.
                supportNodes.remove("highlightingNode");
                supportEdges.insert("highlightingEdge", hitEl[0].y());
            } else if (hitEl[0].x() == 1) {    // Hit elements are nodes.
                supportEdges.remove("highlightingEdge");
                supportNodes.insert("highlightingNode", hitEl[0].y());
            } else {
               supportEdges.remove("highlightingEdge");
               supportNodes.remove("highlightingNode");
           }
           emit updateSignal();
           break;
        }
    }
}

void Workspace::mouseReleased(QPointF pos)
{
    mouseButtonPressed = false;

    //QVector<int> nodesIds = getNodesIds();
    //QVector<int> edgesIds = getEdgesIds();
    //QMapIterator<QString, int> supportNodesIterator(supportNodes);
    //QMapIterator<QString, int> supportEdgesIterator(supportEdges);

    QPointF midPos;
    QPoint nodesOfSplittingEdge;

    QPoint nodesOfEdge;
    QPointF edgeFirstNode;
    QPointF edgeSecondNode;
    QPointF edgeMidPoint;

    QPainterPath path;

    //QVector<QPoint> edgesWNode;
    //QPointF oldPos;
    //QPointF newPos;
    //QString key;

    //QPoint edgesElement;

    QPointF validPos = pos;

    int newNode;

    supportEdges.remove("highlightingEdge");
    supportNodes.remove("highlightingNode");

    selectingElementsEnd = validPos;

    if (snapToGridStatus) {
        validPos = position2grid(validPos);
    }

    switch (selectedTool) {
        case - 1:
            break;
        case addS:
            if (modifyEdgeD) {
                modifyEdgeD = false;
            } else {    // !modifyEdgeD.
                graphLayout->setNodePosition(supportNodes.value("movingNode"), validPos);

                if (getNodePosition(getNodesOfEdge(supportEdges.value("movingEdge")).x()) ==        // mousePress and mouseRelease
                        getNodePosition(getNodesOfEdge(supportEdges.value("movingEdge")).y()) ) {   // in same position.
                    deleteEdge(supportEdges.value("movingEdge"));
                    supportEdges.remove("movingEdge");
                    supportNodes.remove("firstNode");
                    supportNodes.remove("movingNode");
                    emit updateSignal();
                    break;
                }

                if (hitEl[0].x() == 1) {      // Hit Elements are nodes.
                    if (hitEl.size() > 1) {
                        if (hitEl[0].y() != supportNodes.value("movingNode")) {
                            graph->setEdgeSecondNode(supportEdges.value("movingEdge"), hitEl[0].y());
                            graph->deleteNode(supportNodes.value("movingNode"));
                            supportNodes.remove("movingNode");
                        }
                    }
                }

                supportEdges.remove("movingEdge");
                supportNodes.remove("firstNode");
                supportNodes.remove("movingNode");
                checkTooCloseNodes();
            }

            insertActInHistory(true);
            emit contentsChanged();
            emit updateSignal();
            break;
        case removeS:
            if (hitEl[0].x() != 2) {        // Hit elements are not edges.
                break;
            }
            graph->deleteEdge(supportEdges.value("removingEdge"));
            supportEdges.remove("removingEdge");
            insertActInHistory(true);
            emit contentsChanged();
            emit updateSignal();
            break;
        case splitS:
            if (hitEl[0].x() != 2) {  // Hit elements are not edges.
                break;
            }

            nodesOfEdge = getNodesOfEdge(hitEl[0].y());
            edgeFirstNode = getNodePosition(nodesOfEdge.x());
            edgeSecondNode = getNodePosition(nodesOfEdge.y());
            edgeMidPoint = getThirdPointPos(hitEl[0].y());

            path.moveTo(edgeFirstNode);
            path.quadTo(edgeMidPoint, edgeSecondNode);
            path.quadTo(edgeMidPoint, edgeFirstNode);

            if (path.length() < edgeMinLength * 2 + 1 ) {
                break;
            }

            nodesOfSplittingEdge = getNodesOfEdge(supportEdges.value("splittingEdge"));

            midPos = pos;

            newNode = createNode(midPos);

            createEdge(newNode, nodesOfSplittingEdge.y());
            graph->setEdgeSecondNode(supportEdges.value("splittingEdge"), newNode);

            supportEdges.remove("splittingEdge");
            checkTooCloseNodes();
            insertActInHistory(true);
            emit contentsChanged();
            emit updateSignal();
            break;
        case selectE:
            if (!shiftPressed) {
                if (selectingElementsEnd == selectingElementsStart) {     // mousePressEvent and mouseReleaseEvent in the same position.
                    showSelectingArea = false;
                    moveSelectedEls = false;
                    selectedNodes.clear();
                    selectedEdges.clear();
                    if (hitEl[0].x() == 1) { // nearestElement is a node.
                        selectedNodes.append(hitEl[0].y());
                    } else if (hitEl[0].x() == 2) { // nearestElement is an edge.
                        selectedEdges.append(hitEl[0].y());
                    } else {
                        emit updateSignal();
                        break;
                    }
                } else {                                            // mousePressEvent and mouseReleaseEvent NOT in the same position.
                    if (showSelectingArea) {
                        showSelectingArea = false;
                    } else if (moveSelectedEls) {
                        if (hitEl[0].x() == 1) { // Hit elements are nodes.
                            if (hitEl.size() > 1) {

                                if (hitEl[0].y() != supportNodes.value("selMovingNode")) {
                                    graph->substituteNodeInEdges(supportNodes.value("selMovingNode"), hitEl[0].y());
                                    graph->deleteNode(supportNodes.value("selMovingNode"));
                                    supportNodes.remove("selMovingNode");
                                    supportEdges.remove("movingEdge");
                                } else {
                                    graph->substituteNodeInEdges(supportNodes.value("selMovingNode"), hitEl[1].y());
                                    graph->deleteNode(supportNodes.value("selMovingNode"));
                                    supportNodes.remove("selMovingNode");
                                    supportEdges.remove("movingEdge");
                                }
                            }
                        }
                    }
                }
            } else {    //shiftPressed.
                if (showSelectingArea) {
                    showSelectingArea = false;
                }
            }

            for (int i=0; i < supportNodes4selecting.size(); i++) {
                selectedNodes.append(supportNodes4selecting[i]);
            }
            supportNodes4selecting.clear();

            for (int k=0; k < supportEdges4selecting.size(); k++) {
                selectedEdges.append(supportEdges4selecting[k]);
            }
            supportEdges4selecting.clear();

            supportNodes.remove("selMovingNode");
            supportEdges.remove("selMovingEdge");
            checkTooCloseNodes();

            //if (supportNodes4selecting.size() == 0 && supportEdges4selecting.size() == 0) {
            //    emit updateSignal();
            //    break;
            //}

            //if (selectedNodes.size() == 0 && selectedEdges.size() == 0) {
            //    emit updateSignal();
            //    break;
            //}

            insertActInHistory(true);
            emit contentsChanged();
            emit updateSignal();
            break;
        case trans:
            break;
        case information:
            /*for (int i = 0; i < edgesIds.size(); i++) {
                edgesElement = getNodesOfEdge(edgesIds[i]);
                wsout << "edge" << edgesIds[i] << " : pointA= " << edgesElement.x() << " pointB= " << edgesElement.y() << endl;
            }
            for (int i = 0; i < nodesIds.size(); i++) {
                wsout << "node " << nodesIds[i] << endl;
            }
            while (supportNodesIterator.hasNext()) {
                supportNodesIterator.next();
                key = supportNodesIterator.key();
                wsout << key << "SupNode value: " << supportNodesIterator.value() << endl;
            }
            while (supportEdgesIterator.hasNext()) {
                supportEdgesIterator.next();
                key = supportEdgesIterator.key();
                wsout << key << "SupEdge value: " << supportEdgesIterator.value() << endl;
            }*/

            if (hitEl[0].x() != -1) {
                emit dataRequest(hitEl[0]);
            }

            break;
    }
}

void Workspace::deleteSelectedEdges()
{
    for (int i = 0; i < selectedEdges.size(); i++) {
        graph->deleteEdge(selectedEdges[i]);
    }

    emit updateSignal();

    selectedNodes.clear();
    selectedEdges.clear();
    insertActInHistory(true);
}

void Workspace::moveSelectedElements()
        // It identifies the nodes to be moved as the ones in selectedNodes plus the ones in selectedEdges
        // if not already in movingNodes.
{
    QPointF graphTrans;
    QVector<int> movingNodes;

    movingNodes = selectedNodes;                         // movingNodes are the selectedNodes +
                                                         // the ones in selectedEdges if not
    for (int i=0; i < selectedEdges.size(); i++) {       // already in movingNodes.
        QPoint nodesOfEdge = getNodesOfEdge(selectedEdges[i]);
        if (!movingNodes.contains(nodesOfEdge.x())) {
            movingNodes.append(nodesOfEdge.x());
        }
        if (!movingNodes.contains(nodesOfEdge.y())) {
            movingNodes.append(nodesOfEdge.y());
        }
    }

    if (supportNodes.contains("selMovingNode")) {           // mousePressEvent on a node.
        if (snapToGridStatus) {
            QPointF offset;
            if (lengthCheck == true) {
                offset = position2grid(selectingElementsEnd) - getNodePosition(supportNodes.value("selMovingNode"));
            } else {        // !lengthCheck
                offset.setX(0);
                offset.setY(0);
            }

            setNodePosition(supportNodes.value("selMovingNode"), position2grid(selectingElementsEnd));

            for (int j=0; j < movingNodes.size(); j++) {
                if (movingNodes.value(j) == supportNodes.value("selMovingNode")) {
                    continue;
                }
                setNodePosition(movingNodes.value(j), getNodePosition(movingNodes.value(j)) + offset);
            }
        } else {
            graphTrans = selectingElementsEnd - selectingElementsSupport;
            for (int j=0; j < movingNodes.size(); j++) {
                setNodePosition(movingNodes.value(j), getNodePosition(movingNodes.value(j)) + graphTrans);
            }
        }
    } else if (supportEdges.contains("selMovingEdge")) {    // mousePressEvent on an edge.
        graphTrans = selectingElementsEnd - selectingElementsSupport;
        for (int j=0; j < movingNodes.size(); j++) {
            setNodePosition(movingNodes.value(j), getNodePosition(movingNodes.value(j)) + graphTrans);
        }
    }

    selectingElementsSupport = selectingElementsEnd;
    lengthCheck = true;
}

void Workspace::removeNodeFromSelected(int node)
{
    selectedNodes.remove(selectedNodes.indexOf(node));
}

void Workspace::removeEdgeFromSelected(int edge)
{
    selectedEdges.remove(selectedEdges.indexOf(edge));
}

void Workspace::inizializeHistory()
{
    Graph* newGraph = new Graph(this);
    (*newGraph).setNodes(graph->getNodes());
    (*newGraph).setEdges(graph->getEdges());
    QSharedPointer<Graph> graphSharedPt(newGraph);

    GraphLayout* newGraphLayout = new GraphLayout(this);
    (*newGraphLayout).setNodesLayout(graphLayout->getNodesLayout());
    (*newGraphLayout).setEdgesLayout(graphLayout->getEdgesLayout());
    QSharedPointer<GraphLayout> graphLayoutSharedPt(newGraphLayout);

    GraphProperties* newGraphProperties = new GraphProperties(this);
    (*newGraphProperties).setNodesProperties(graphProperties->getNodesProperties());
    (*newGraphProperties).setEdgesProperties(graphProperties->getEdgesProperties());
    (*newGraphProperties).setCase(graphProperties->getCase());
    (*newGraphProperties).setSuperedges(graphProperties->getSuperedges());
    (*newGraphProperties).setTransformations(graphProperties->getTransformations());
    QSharedPointer<GraphProperties> graphPropertiesSharedPt(newGraphProperties);

    GraphMesh* newGraphMesh = new GraphMesh(this);
    (*newGraphMesh).setNodesMType(graphMesh->getNodesMType());
    (*newGraphMesh).setNodesMParameters(graphMesh->getNodesMParameters());
    (*newGraphMesh).setNodesMMeshnodesId(graphMesh->getNodesMMeshnodesId());
    (*newGraphMesh).setNodesMElementId(graphMesh->getNodesMElementId());

    (*newGraphMesh).setEdgesMS(graphMesh->getEdgesMS());
    (*newGraphMesh).setEdgesMTypes(graphMesh->getEdgesMTypes());
    (*newGraphMesh).setEdgesMParameters(graphMesh->getEdgesMParameters());
    (*newGraphMesh).setEdgesMMeshnodesId(graphMesh->getEdgesMMeshnodesId());
    (*newGraphMesh).setEdgesMElementsId(graphMesh->getEdgesMElementsId());
    QSharedPointer<GraphMesh> graphMeshSharedPt(newGraphMesh);

    ApplicationStatus* newApplicationStatus = new ApplicationStatus(this);

    newApplicationStatus->setGraph(graphSharedPt);
    newApplicationStatus->setGraphLayout(graphLayoutSharedPt);
    newApplicationStatus->setGraphProperties(graphPropertiesSharedPt);
    newApplicationStatus->setGraphMesh(graphMeshSharedPt);

    newApplicationStatus->setScreenOrigin(screenOrigin);
    newApplicationStatus->setZoom(zoomFactor);
    newApplicationStatus->setGridStatus(gridStatus);
    newApplicationStatus->setGridSpacing(gridSpacing);
    newApplicationStatus->setSnapToGridStatus(snapToGridStatus);
    newApplicationStatus->setSelectedNodes(selectedNodes);
    newApplicationStatus->setSelectedEdges(selectedEdges);

    statusVector.append(newApplicationStatus);
}

void Workspace::insertActInHistory(bool graphChanged)
{
    while (currentStatus < statusVector.size() - 1) {
        ApplicationStatus* temp = statusVector.last();
        statusVector.remove(statusVector.size() - 1);
        delete temp;
    }

    if (statusVector.size() == 10 ) {
        ApplicationStatus* temp = statusVector[0];
        statusVector.remove(0);
        delete temp;
    } else {
    currentStatus++;
    }

    ApplicationStatus* newApplicationStatus = new ApplicationStatus(this);

    if (graphChanged) {
        Graph* newGraph = new Graph(this);
        (*newGraph).setNodes(graph->getNodes());
        (*newGraph).setEdges(graph->getEdges());
        QSharedPointer<Graph> graphSharedPt(newGraph);

        GraphLayout* newGraphLayout = new GraphLayout(this);
        (*newGraphLayout).setNodesLayout(graphLayout->getNodesLayout());
        (*newGraphLayout).setEdgesLayout(graphLayout->getEdgesLayout());
        QSharedPointer<GraphLayout> graphLayoutSharedPt(newGraphLayout);

        GraphProperties* newGraphProperties = new GraphProperties(this);
        (*newGraphProperties).setNodesProperties(graphProperties->getNodesProperties());
        (*newGraphProperties).setEdgesProperties(graphProperties->getEdgesProperties());
        (*newGraphProperties).setCase(graphProperties->getCase());
        (*newGraphProperties).setSuperedges(graphProperties->getSuperedges());
        (*newGraphProperties).setTransformations(graphProperties->getTransformations());
        QSharedPointer<GraphProperties> graphPropertiesSharedPt(newGraphProperties);

        GraphMesh* newGraphMesh = new GraphMesh(this);
        (*newGraphMesh).setNodesMType(graphMesh->getNodesMType());
        (*newGraphMesh).setNodesMParameters(graphMesh->getNodesMParameters());
        (*newGraphMesh).setNodesMMeshnodesId(graphMesh->getNodesMMeshnodesId());
        (*newGraphMesh).setNodesMElementId(graphMesh->getNodesMElementId());

        (*newGraphMesh).setEdgesMS(graphMesh->getEdgesMS());
        (*newGraphMesh).setEdgesMTypes(graphMesh->getEdgesMTypes());
        (*newGraphMesh).setEdgesMParameters(graphMesh->getEdgesMParameters());
        (*newGraphMesh).setEdgesMMeshnodesId(graphMesh->getEdgesMMeshnodesId());
        (*newGraphMesh).setEdgesMElementsId(graphMesh->getEdgesMElementsId());
        QSharedPointer<GraphMesh> graphMeshSharedPt(newGraphMesh);

        newApplicationStatus->setGraph(graphSharedPt);
        newApplicationStatus->setGraphLayout(graphLayoutSharedPt);
        newApplicationStatus->setGraphProperties(graphPropertiesSharedPt);
        newApplicationStatus->setGraphMesh(graphMeshSharedPt);
    } else {
        QSharedPointer<Graph> graphSharedPt(statusVector[currentStatus - 1]->getGraph());
        newApplicationStatus->setGraph(graphSharedPt);

        QSharedPointer<GraphLayout> graphLayoutSharedPt(statusVector[currentStatus - 1]->getGraphLayout());
        newApplicationStatus->setGraphLayout(graphLayoutSharedPt);

        QSharedPointer<GraphProperties> graphPropertiesSharedPt(statusVector[currentStatus - 1]->getGraphProperties());
        newApplicationStatus->setGraphProperties(graphPropertiesSharedPt);

        QSharedPointer<GraphMesh> graphMeshSharedPt(statusVector[currentStatus - 1]->getGraphMesh());
        newApplicationStatus->setGraphMesh(graphMeshSharedPt);
    }

    newApplicationStatus->setScreenOrigin(screenOrigin);
    newApplicationStatus->setZoom(zoomFactor);
    newApplicationStatus->setGridStatus(gridStatus);
    newApplicationStatus->setGridSpacing(gridSpacing);
    newApplicationStatus->setSnapToGridStatus(snapToGridStatus);
    newApplicationStatus->setShowLabelsStatus(showLabelsStatus);
    newApplicationStatus->setSelectedNodes(selectedNodes);
    newApplicationStatus->setSelectedEdges(selectedEdges);

    statusVector.append(newApplicationStatus);
}

QPointF Workspace::movingNodeAcceptablePosition(const QPointF &A, const QPointF &B)
{
    QPointF result;

    if (!snapToGridStatus) {
        if ((B - A).x() == 0 && (B - A).y() == 0) {
            result.setX(A.x());
            result.setY(A.y() - edgeMinLength);
        } else {
            result = (A + edgeMinLength * (B - A) / norm (B-A));
        }
    } else {
        if ((B - A).x() == 0 && (B - A).y() == 0) {
            result.setX(A.x());
            result.setY(A.y() - gridSpacing);
        } else {
            result = position2grid(A + edgeMinLength * (B - A) / norm (B-A));
        }
    }

    return result;
}

void Workspace::checkTooCloseNodes()
{
    /*supportNodes.remove("tooCloseNode");

    QVector<int> nodesIds = graph->getNodesIds();
    QMap<int, bool> tooCloseNodes;

    for (int i=0; i < nodesIds.size() - 1; i++) {
        for (int j = i + 1; j < nodesIds.size(); j++) {
            if (distance(getNodePosition(nodesIds[i]), getNodePosition(nodesIds[j])) < edgeMinLength * 0.8) {
                tooCloseNodes.insertMulti(nodesIds[i], true);
                tooCloseNodes.insertMulti(nodesIds[j], true);
            }
        }
    }

    QMapIterator<int, bool> tooCloseNodesIter(tooCloseNodes);
    while (tooCloseNodesIter.hasNext()) {
        tooCloseNodesIter.next();
        supportNodes.insertMulti("tooCloseNode", tooCloseNodesIter.key());
    }*/
}

QPointF Workspace::position2grid(QPointF pos)
{
    float A = (pos.x() - int(pos.x() / gridSpacing) * gridSpacing) / gridSpacing;
    float B = (pos.y() - int(pos.y() / gridSpacing) * gridSpacing) / gridSpacing;
    if (A < 0.5) {
        pos.setX(int(pos.x() / gridSpacing) * gridSpacing);
    } else {
        pos.setX(int(pos.x() / gridSpacing + 1) * gridSpacing);
    }
    if (B < 0.5) {
        pos.setY(int(pos.y() / gridSpacing) * gridSpacing);
    } else {
        pos.setY(int(pos.y() / gridSpacing + 1) * gridSpacing);
    }

    return pos;
}

float Workspace::distance(const QPointF& A, const QPointF& B)
{
    float ABx = B.x()-A.x();
    float ABy = B.y()-A.y();
    return sqrt( ABx*ABx + ABy*ABy );
}

float Workspace::dot(const QPointF& A, const QPointF& B)
{
    return A.x()*B.x() + A.y()*B.y();
}

float Workspace::norm(const QPointF &A)
{
    float Ax = A.x();
    float Ay = A.y();
    return sqrt( Ax*Ax + Ay*Ay );
}

QPointF Workspace::getNodePosition(int nodeId)
{
    return graphLayout->getNodePosition(nodeId);
}

void Workspace::setNodePosition(int nodeId, QPointF pos)
{
    graphLayout->setNodePosition(nodeId, pos);
}

QVector<int> Workspace::getNodesIds()
{
    return graph->getNodesIds();
}

QVector<int> Workspace::getEdgesIds()
{
    return graph->getEdgesIds();
}

QPoint Workspace::getNodesOfEdge(int edgeId)
{
    return graph->getNodesOfEdge(edgeId);
}

QMap<QString, int> Workspace::getSupportNodes()
{
    return supportNodes;
}

QMap<QString, int> Workspace::getSupportEdges()
{
    return supportEdges;
}

QPointF Workspace::getThirdPointPos(int edgeId)
{
    QPointF edgeLayoutParameters = graphLayout->getEdgeLayoutParameters(edgeId);

    QPoint nodesOfEdge = graph->getNodesOfEdge(edgeId);
    QPointF A = graphLayout->getNodePosition(nodesOfEdge.x());
    QPointF B = graphLayout->getNodePosition(nodesOfEdge.y());

    QPointF nBA = (B - A) / norm (B - A);
    QPointF nPos;
    nPos.setX(nBA.y());
    nPos.setY(-nBA.x());

    return (A + nBA * norm(B - A) * edgeLayoutParameters.x() + 2 * nPos * edgeLayoutParameters.y());
}

QString Workspace::getNodeProperties(int nodeId)
{
    return graphProperties->getNodeProperties(nodeId);
}

QString Workspace::getNodeName(int nodeId)
{
    return graphProperties->getNodeName(nodeId);
}

void Workspace::setNodeName(int nodeId, QString theName)
{
    graphProperties->setNodeName(nodeId, theName);
    emit updateSignal();
}

void Workspace::setEdgeName(int edgeId, QString theName)
{
    graphProperties->setEdgeName(edgeId, theName);
    emit updateSignal();
}

void Workspace::setEdgeType(int edgeId, QString theType)
{
    graphProperties->setEdgeType(edgeId, theType);
}

QString Workspace::getEdgeProperties(int edgeId)
{
    return graphProperties->getEdgeProperties(edgeId);
}

QVector<float> Workspace::getEdgeMs(int edgeId)
{
    return graphMesh->getEdgeMs(edgeId);
}

QString Workspace::getEdgeName(int edgeId)
{
    return graphProperties->getEdgeName(edgeId);
}

QVector<QString> Workspace::getEdgeMTypes(int edgeId)
{
    return graphMesh->getEdgeMTypes(edgeId);
}

QString Workspace::getNodeMType(int nodeId)
{
    return graphMesh->getNodeMType(nodeId);
}

bool Workspace::getGridStatus()
{
    return gridStatus;
}

bool Workspace::getSnapToGridStatus()
{
    return snapToGridStatus;
}

bool Workspace::getShowLabelsStatus()
{
    return showLabelsStatus;
}

int Workspace::getGridSpacing()
{
    return gridSpacing;
}

int Workspace::getSelectedTool()
{
    return selectedTool;
}

bool Workspace::getShowSelectingArea()
{
    return showSelectingArea;
}

bool Workspace::dataInGraph()
{
    return graph->dataInGraph();
}

bool Workspace::dataInGraphMesh()
{
    return graphMesh->dataInGraphMesh();
}

float Workspace::getZoomFactor()
{
    return zoomFactor;
}

QPointF Workspace::getScreenOrigin()
{
    return screenOrigin;
}

bool Workspace::getMouseButtonPressed()
{
    return mouseButtonPressed;
}

QVector<int> Workspace::getSelectedNodes()
{
    return selectedNodes;
}

QVector<int> Workspace::getSelectedEdges()
{
    return selectedEdges;
}

QVector<int> Workspace::getSupportNodes4selecting()
{
    return supportNodes4selecting;
}

QVector<int> Workspace::getSupportEdges4selecting()
{
    return supportEdges4selecting;
}

bool Workspace::getNodeMov(int nodeId)
{
    return graphLayout->getNodeMovStatus(nodeId);
}

QPointF Workspace::getSelectingElementsStart()
{
    return selectingElementsStart;
}

QPointF Workspace::getSelectingElementsEnd()
{
    return selectingElementsEnd;
}

Graph* Workspace::getGraph()
{
    return graph;
}

GraphLayout* Workspace::getGraphLayout()
{
    return graphLayout;
}

GraphProperties* Workspace::getGraphProperties()
{
    return graphProperties;
}

GraphMesh* Workspace::getGraphMesh()
{
    return graphMesh;
}

ApplicationStatus* Workspace::getCurrentStatus()
{
    return statusVector[currentStatus];
}

void Workspace::setNodeProperties(int nodeId, QString nodeProp)
{
    graphProperties->setNodeProperties(nodeId, nodeProp);
}

void Workspace::setEdgeProperties(int edgeId, QString edgeProp)
{
    graphProperties->setEdgeProperties(edgeId, edgeProp);
}

void Workspace::setZoomFactor(float theZoomFactor)
{
    zoomFactor = theZoomFactor;
    insertActInHistory(true);
}

void Workspace::setScreenOrigin(QPointF theScreenOrigin)
{
    screenOrigin = theScreenOrigin;
}

void Workspace::setShiftPressed(bool shiftStatus)
{
    shiftPressed = shiftStatus;
}

void Workspace::updateHistory()
{
    insertActInHistory(true);
}

void Workspace::Key_Cancel_Pressed()
{
    deleteSelectedEdges();
}

void Workspace::blockNodes()
{
    if (!selectedNodes.isEmpty()) {
        bool status;
        if (graphLayout->getNodeMovStatus(selectedNodes.at(0))) {
            status = false;
        } else {
            status = true;
        }

        for (int i = 0; i < selectedNodes.size(); i++) {
            graphLayout->setNodeMovStatus(selectedNodes[i], status);
        }
    }

    selectedNodes.clear();
    updateSignal();
}

void Workspace::applyDefaultMesh()
{
    graphMesh->applyDefaultMesh(graph->getNodesIds());
    insertActInHistory(true);
    emit contentsChanged();
    emit updateSignal();
}

void Workspace::Key_CTRL_A_Pressed()
{
    selectedNodes.clear();
    selectedEdges.clear();
    QVector<int> nodesIds = graph->getNodesIds();
    QVector<int> edgesIds = graph->getEdgesIds();

    for (int i = 0; i < nodesIds.size(); i++) {
        selectedNodes << nodesIds[i];
    }

    for (int i = 0; i < edgesIds.size(); i++) {
        selectedEdges << edgesIds[i];
    }

    emit updateSignal();
}

void Workspace::elementsBeenHit(QVector<QPoint> hitElements)
{
    hitEl = hitElements;
}

void Workspace::netToBeUnravelled()
{
    if (unravelTimer->isActive()) {
        return;
    }

    unravelCount = 0;
    unravelTimer->start(1);
}

void Workspace::unravelNet()
{
    unravelCount++;

    if (unravelCount == unravelIter) {
        unravelTimer->stop();
        insertActInHistory(true);
        emit updateSignal();
        emit contentsChanged();
    }

    unraveller->unravelNet(screenOrigin);

    if (unravelCount % 3 == 0) {
        emit updateSignal();
    }
}

void Workspace::setHighlightingEl(QPoint element)
{
    if (element.x() == 1) { // element is a node.
        supportEdges.clear();
        supportNodes.insert("highlightingNode", element.y());
    } else if (element.x() == 2) {                // element is n edge.
        supportNodes.clear();
        supportEdges.insert("highlightingEdge", element.y());
    } else {
        supportEdges.clear();
        supportNodes.clear();
    }

    emit updateSignal();
}

int Workspace::createNode(QPointF pos)
{
    int idNewNode = graph->createNode();
    graphLayout->createNode(idNewNode, pos);
    QString temp;
    graphProperties->createNode(idNewNode, temp, temp);
    //graphMesh

    return idNewNode;
}

int Workspace::createEdge(int nodeAId, int nodeBId)
{
    int idNewEdge = graph->createEdge(nodeAId, nodeBId);
    graphLayout->createEdge(idNewEdge);
    QString nodeAIdString;
    nodeAIdString.setNum(nodeAId);
    QString nodeBIdString;
    nodeBIdString.setNum(nodeBId);
    graphProperties->createEdge(idNewEdge, nodeAIdString, nodeBIdString);

    return idNewEdge;
}

void Workspace::deleteNode(int nodeId)
{
    graph->deleteNode(nodeId);
    graphLayout->deleteNode(nodeId);
    graphProperties->deleteNode(nodeId);
}

void Workspace::deleteEdge(int edgeId)
        //
        // Graph will emit nodeDeleted signal if any nodes of edgeId does not belong
        // to any other edge.
        //
{
    graph->deleteEdge(edgeId);
    graphLayout->deleteEdge(edgeId);
    graphProperties->deleteEdge(edgeId);
}

void Workspace::clear()
{
    statusVector.clear();
    graph->clear();
    graphLayout->clear();
    graphProperties->clear();
    graphMesh->clear();

    supportNodes.clear();
    supportEdges.clear();
    selectedNodes.clear();
    selectedEdges.clear();
    supportNodes4selecting.clear();
    supportEdges4selecting.clear();
    hitEl.clear();

    gridStatus = true;
    snapToGridStatus = true;
    gridSpacing = 20;

    distanceTolerance = gridSpacing / 2;
    selectedTool = selectE;

    mouseButtonPressed = false;
    shiftPressed = false;

    moveSelectedEls = false;
    showSelectingArea = false;
    modifyEdgeD = false;
    lengthCheck = true;

    screenOrigin.setX(0);
    screenOrigin.setY(0);
    zoomFactor = 100.0;

    currentStatus = 0;
    inizializeHistory();

    emit updateSignal();
}
