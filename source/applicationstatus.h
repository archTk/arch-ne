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

#ifndef APPLICATIONSTATUS_H
#define APPLICATIONSTATUS_H

#include <QMap>
#include <QSharedPointer>
#include <QObject>
#include <QPointF>
#include <QVector>
#include "graph.h"
#include "graphlayout.h"
#include "graphproperties.h"
#include "graphmesh.h"
#include "networkproperties.h"

class ApplicationStatus : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationStatus(QObject* parent = 0);
    ~ApplicationStatus();

    void setGraph(QSharedPointer<Graph> theGraphSharedPt);
    void setGraphLayout(QSharedPointer<GraphLayout> theGraphLayoutSharedPt);
    void setGraphProperties(QSharedPointer<GraphProperties> theGraphPropertiesSharedPt);
    void setGraphMesh(QSharedPointer<GraphMesh> theGraphMeshSharedPt);
    void setNetworkProperties(QSharedPointer<NetworkProperties> theNetworkPropertiesSharedPt);
    void setScreenOrigin(QPointF theScreenOrigin);
    void setZoom(float theZoomFactor);
    void setGridStatus(bool theGridStatus);
    void setGridSpacing (int theGridSpacing);
    void setSnapToGridStatus(bool theSnapToGridStatus);
    void setShowLabelsStatus(bool theShowLabelsStatus);
    void setShowMeshStatus(bool theShowMeshStatus);
    void setSelectedNodes(QVector<int> theSelectedNodes);
    void setSelectedEdges(QVector<int> theSelectedEdges);

    QSharedPointer<Graph> getGraph();
    QSharedPointer<GraphLayout> getGraphLayout();
    QSharedPointer<GraphProperties> getGraphProperties();
    QSharedPointer<GraphMesh> getGraphMesh();
    QSharedPointer<NetworkProperties> getNetworkProperties();
    QPointF getScreenOrigin();
    float getZoom();
    bool getGridStatus();
    bool getSnapToGridStatus();
    int getGridSpacing();
    bool getShowLabelsStatus();
    bool getShowMeshStatus();
    QVector<int> getSelectedNodes();
    QVector<int> getSelectedEdges();

signals:

public slots:

private:
    QSharedPointer<Graph> graph;
    QSharedPointer<GraphLayout> graphLayout;
    QSharedPointer<GraphProperties> graphProperties;
    QSharedPointer<GraphMesh> graphMesh;
    QSharedPointer<NetworkProperties> networkProperties;
    QPointF screenOrigin;
    float zoomFactor;
    bool gridStatus;
    int gridSpacing;
    bool snapToGridStatus;
    bool showLabelsStatus;
    bool showMeshStatus;
    QVector<int> selectedNodes;
    QVector<int> selectedEdges;
};

#endif // APPLICATIONSTATUS_H
