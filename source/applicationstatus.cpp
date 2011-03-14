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

#include "applicationstatus.h"

ApplicationStatus::ApplicationStatus(QObject* parent) :
    QObject(parent)
{
}

ApplicationStatus::~ApplicationStatus()
{
    /*
    if (ownGraph)
    {
        delete graph;
        graph = NULL;
    }

    if (ownGraphLayout)
    {
        delete graphLayout;
        graphLayout = NULL;
    }
    */
}

void ApplicationStatus::setGraph(QSharedPointer<Graph> theGraphSharedPt)
{
    graph = theGraphSharedPt;
}

void ApplicationStatus::setGraphLayout(QSharedPointer<GraphLayout> theGraphLayoutSharedPt)
{
    graphLayout = theGraphLayoutSharedPt;
}

void ApplicationStatus::setGraphProperties(QSharedPointer<GraphProperties> theGraphPropertiesSharedPt)
{
    graphProperties = theGraphPropertiesSharedPt;
}

void ApplicationStatus::setGraphMesh(QSharedPointer<GraphMesh> theGraphMeshSharedPt)
{
    graphMesh = theGraphMeshSharedPt;
}

void ApplicationStatus::setNetworkProperties(QSharedPointer<NetworkProperties> theNetworkPropertiesSharedPt)
{
    networkProperties = theNetworkPropertiesSharedPt;
}

void ApplicationStatus::setScreenOrigin(QPointF theScreenOrigin)
{
    screenOrigin = theScreenOrigin;
}

void ApplicationStatus::setZoom(float theZoomFactor)
{
    zoomFactor = theZoomFactor;
}

void ApplicationStatus::setGridStatus(bool theGridStatus)
{
    gridStatus = theGridStatus;
}

void ApplicationStatus::setGridSpacing(int theGridSpacing)
{
    gridSpacing = theGridSpacing;
}

void ApplicationStatus::setSnapToGridStatus(bool theSnapToGridStatus)
{
    snapToGridStatus = theSnapToGridStatus;
}

void ApplicationStatus::setShowLabelsStatus(bool theShowLabelsStatus)
{
    showLabelsStatus = theShowLabelsStatus;
}

void ApplicationStatus::setShowMeshStatus(bool theShowMeshStatus)
{
    showMeshStatus = theShowMeshStatus;
}

void ApplicationStatus::setSelectedNodes(QVector<int> theSelectedNodes)
{
    selectedNodes = theSelectedNodes;
}

void ApplicationStatus::setSelectedEdges(QVector<int> theSelectedEdges)
{
    selectedEdges = theSelectedEdges;
}

QSharedPointer<Graph> ApplicationStatus::getGraph()
{
    return graph;
}

QSharedPointer<GraphLayout> ApplicationStatus::getGraphLayout()
{
    return graphLayout;
}

QSharedPointer<GraphProperties> ApplicationStatus::getGraphProperties()
{
    return graphProperties;
}

QSharedPointer<GraphMesh> ApplicationStatus::getGraphMesh()
{
    return graphMesh;
}

QSharedPointer<NetworkProperties> ApplicationStatus::getNetworkProperties()
{
    return networkProperties;
}

QPointF ApplicationStatus::getScreenOrigin()
{
    return screenOrigin;
}

float ApplicationStatus::getZoom()
{
    return zoomFactor;
}

bool ApplicationStatus::getGridStatus()
{
    return gridStatus;
}

bool ApplicationStatus::getSnapToGridStatus()
{
    return snapToGridStatus;
}

int ApplicationStatus::getGridSpacing()
{
    return gridSpacing;
}

bool ApplicationStatus::getShowLabelsStatus()
{
    return showLabelsStatus;
}

bool ApplicationStatus::getShowMeshStatus()
{
    return showMeshStatus;
}

QVector<int> ApplicationStatus::getSelectedNodes()
{
    return selectedNodes;
}

QVector<int> ApplicationStatus::getSelectedEdges()
{
    return selectedEdges;
}
