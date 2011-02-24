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

#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include <QPointF>
#include <QVector>
#include <QString>
#include <QSharedPointer>

class ApplicationStatus;
class InputOutput;
class Graph;
class GraphLayout;
class GraphProperties;
class GraphMesh;
class NetworkProperties;
class Unraveller;

class Workspace : public QObject
{
    Q_OBJECT

public:
    explicit Workspace(QObject *parent = 0);

    void clear();

    QVector<float> getEdgeMs(int edgeId);
    QString getEdgeName(int edgeId);
    QString getEdgeProperties(int edgeId);
    QVector<int> getEdgesIds();

    QString getNetworkString();

    QPointF getNodePosition(int nodeId);
    QString getNodeProperties(int nodeId);

    QString getNodeName(int nodeId);
    void setNodeName(int nodeId, QString theName);

    void setEdgeName(int edgeId, QString theName);
    void setEdgeType(int edgeId, QString theType);

    QVector<int> getNodesIds();
    QPoint getNodesOfEdge(int edgeId);
    QMap<QString, int> getSupportEdges();
    QMap<QString, int> getSupportNodes();
    QPointF getThirdPointPos(int edgeId);

    QVector<QString> getEdgeMTypes(int edgeId);
    QString getNodeMType (int nodeId);

    void setNodeProperties(int nodeId, QString nodeProp);
    void setEdgeProperties(int edgeId, QString edgeProp);
    void setBCXML(QString theBCXML);
    void setSPXML(QString theSPXML);
    void setPatientInfoXML(QString thePatientInfoXML);
    QString getBCXML();
    QString getSPXML();
    QString getPatientInfoXML();

    void setNodePosition(int nodeId, QPointF pos);

    bool getGridStatus();
    bool getSnapToGridStatus();
    bool getShowLabelsStatus();
    int getGridSpacing();
    int getSelectedTool();
    bool getShowSelectingArea();
    bool dataInGraph();
    bool dataInGraphMesh();
    float getZoomFactor();
    QPointF getScreenOrigin();
    bool getMouseButtonPressed();

    QVector<int> getSelectedEdges();
    QVector<int> getSelectedNodes();
    QVector<int> getSupportNodes4selecting();
    QVector<int> getSupportEdges4selecting();

    bool getNodeMov(int nodeId);

    QPointF getSelectingElementsStart();
    QPointF getSelectingElementsEnd();

    float distance(const QPointF& A, const QPointF& B);

    Graph* getGraph();
    GraphLayout* getGraphLayout();
    GraphProperties* getGraphProperties();
    GraphMesh* getGraphMesh();
    NetworkProperties* getNetworkProperties();

    ApplicationStatus* getCurrentStatus();

signals:
    void restoreCurs();
    void updateSignal();
    void contentsChanged();
    void dataRequest(QPoint elementRequest);
    void showResults(QPoint elementRequest);

public slots:
    void addSegment();
    void applyDefaultMesh();
    void blockNodes();
    void homeView();
    void info();
    void resultsRequest();

    void redo();
    void removeSegment();
    void selectElements();
    void showGrid();
    void showLabels();
    void snapToGrid();
    void splitSegment();
    void superEdge();
    void translate();
    void undo();

    void mousePressed(QPointF pos);
    void mouseMoved(QPointF pos);
    void mouseReleased(QPointF pos);
    void setZoomFactor(float theZoomFactor);
    void setScreenOrigin(QPointF theScreenOrigin);
    void setShiftPressed(bool shiftStatus);
    void updateHistory();
    void Key_Cancel_Pressed();
    void Key_CTRL_A_Pressed();
    void elementsBeenHit(QVector<QPoint> hitElements);
    void netToBeUnravelled();
    void unravelNet();

    void setHighlightingEl(QPoint element);

protected:

private:
    int createNode(QPointF pos);
    int createEdge(int nodeAId, int nodeBId);
    void clearSelectedAndUpdate();
    void deleteNode(int nodeId);
    void deleteEdge(int edgeId);

    void deleteSelectedEdges();
    void moveSelectedElements();

    void removeNodeFromSelected(int node);
    void removeEdgeFromSelected(int edge);

    void inizializeHistory();
    void insertActInHistory(bool graphChanged);

    QPointF movingNodeAcceptablePosition(const QPointF& A,const  QPointF& B);
    void checkTooCloseNodes();

    QPointF position2grid(QPointF pos);

    float dot(const QPointF& A, const QPointF& B);
    float norm(const QPointF& A);

    QMap<QString, int> supportNodes;
    QMap<QString, int> supportEdges;

    QVector<int> selectedNodes;
    QVector<int> selectedEdges;
    QVector<int> supportNodes4selecting;
    QVector<int> supportEdges4selecting;

    float zoomFactor;
    QPointF screenOrigin;
    bool gridStatus;
    int gridSpacing;
    bool snapToGridStatus;
    bool showLabelsStatus;

    Graph* graph;
    GraphLayout* graphLayout;
    GraphProperties* graphProperties;
    GraphMesh* graphMesh;
    NetworkProperties* networkProperties;
    Unraveller* unraveller;

    QVector<ApplicationStatus*> statusVector;

    QPointF selectingElementsStart;
    QPointF selectingElementsSupport;
    QPointF selectingElementsEnd;

    int edgeMinLength;

    int curvingEdgeId;
    QVector<QPoint> hitEl;

    int selectedTool;
    float distanceTolerance;
    float dist;

    int currentStatus;

    bool shiftPressed;
    bool showSelectingArea;
    bool moveSelectedEls;
    bool modifyEdgeD;
    bool lengthCheck;
    bool mouseButtonPressed;

    QTimer* unravelTimer;
    int unravelCount;

    int unravelIter;
};

enum tool {
    addS,
    removeS,
    splitS,
    superE,
    selectE,
    information,
    trans,
    results,
};

#endif // WORKSPACE_H
