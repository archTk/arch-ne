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

#include "appcontroller.h"
#include "mainwindow.h"
#include "workspace.h"
#include "editorarea.h"
#include "inputoutput.h"
#include "DataCollector/datacollector.h"

#include <QDialog>
#include <QMessageBox>
#include <QWidget>
#include <QGridLayout>

#include <iostream>
#include <QTextStream>
using namespace std;

QTextStream appout(stdout);

AppController::AppController(QObject *parent) :
    QObject(parent)
{
    incrementalDataRequest = 0;

    workspace = new Workspace;
}

void AppController::setMainWindow(MainWindow* mainWin)
{
    mainWindow = mainWin;
}

void AppController::setEditorArea(EditorArea* theEditorArea)
{
    editorArea = theEditorArea;
}

void AppController::createConnections()
{
    editorArea->setWorkspace(workspace);

    connect(mainWindow, SIGNAL(addSegmentPressed()), workspace, SLOT(addSegment()));
    connect(mainWindow, SIGNAL(blockNodesPressed()), workspace, SLOT(blockNodes()));
    connect(mainWindow, SIGNAL(defaultMeshPressed()), workspace, SLOT(applyDefaultMesh()));
    connect(mainWindow, SIGNAL(dockClosedSig()), this, SLOT(dockClosed()));
    connect(mainWindow, SIGNAL(editingEl2Ws(QPoint)), workspace, SLOT(setHighlightingEl(QPoint)));
    connect(mainWindow, SIGNAL(homeViewPressed()), workspace, SLOT(homeView()));
    connect(mainWindow, SIGNAL(infoPressed()), workspace, SLOT(info()));
    connect(mainWindow, SIGNAL(meshToBeGenerated(QString)), this, SLOT(generateMesh(QString)));
    connect(mainWindow, SIGNAL(redoPressed()), workspace, SLOT(redo()));
    connect(mainWindow, SIGNAL(removeSegmentPressed()), workspace, SLOT(removeSegment()));
    connect(mainWindow, SIGNAL(selectElementsPressed()), workspace, SLOT(selectElements()));
    connect(mainWindow, SIGNAL(setPrefPressed()), this, SLOT(setPreferences()));
    connect(mainWindow, SIGNAL(showGridPressed()), workspace, SLOT(showGrid()));
    connect(mainWindow, SIGNAL(showLabelsPressed()), workspace, SLOT(showLabels()));
    connect(mainWindow, SIGNAL(snapToGridPressed()), workspace, SLOT(snapToGrid()));
    connect(mainWindow, SIGNAL(splitSegmentPressed()), workspace, SLOT(splitSegment()));
    connect(mainWindow, SIGNAL(superEdgePressed()), workspace, SLOT(superEdge()));
    connect(mainWindow, SIGNAL(translatePressed()), workspace, SLOT(translate()));
    connect(mainWindow, SIGNAL(undoPressed()), workspace, SLOT(undo()));
    connect(mainWindow, SIGNAL(unravelNetPressed()), workspace, SLOT(netToBeUnravelled()));
    connect(mainWindow, SIGNAL(zoomInPressed()), editorArea, SLOT(zoomIn()));
    connect(mainWindow, SIGNAL(zoomOutPressed()), editorArea, SLOT(zoomOut()));

    connect(mainWindow, SIGNAL(loadGraphFromLayout()), this, SLOT(loadGraphFromLayout()));
    connect(mainWindow, SIGNAL(loadGraphFromGraph()), this, SLOT(loadGraphFromGraph()));
    connect(mainWindow, SIGNAL(meshToBeLoaded()), this, SLOT(loadMesh()));
    connect(mainWindow, SIGNAL(saveNetwork(QString)), this, SLOT(saveNetwork(QString)));

    connect(workspace, SIGNAL(contentsChanged()), mainWindow, SLOT(documentWasModified()));
    connect(workspace, SIGNAL(dataRequest(QPoint)), this, SLOT(dataRequest(QPoint)));
    connect(workspace, SIGNAL(updateSignal()), editorArea, SLOT(updateRender()));

    connect(editorArea, SIGNAL(mouseBeenPressed(QPointF)), workspace, SLOT(mousePressed(QPointF)));
    connect(editorArea, SIGNAL(mouseMoved(QPointF)), workspace, SLOT(mouseMoved(QPointF)));
    connect(editorArea, SIGNAL(mouseReleased(QPointF)), workspace, SLOT(mouseReleased(QPointF)));
    connect(editorArea, SIGNAL(zoomFactorChanged(float)), workspace, SLOT(setZoomFactor(float)));
    connect(editorArea, SIGNAL(screenOriginChanged(QPointF)), workspace, SLOT(setScreenOrigin(QPointF)));
    connect(editorArea, SIGNAL(shiftPressedChanged(bool)), workspace, SLOT(setShiftPressed(bool)));
    connect(editorArea, SIGNAL(updateHistory()), workspace, SLOT(updateHistory()));
    connect(editorArea, SIGNAL(updateMainWindow()), mainWindow, SLOT(updateMainWindow()));
    connect(editorArea, SIGNAL(Key_Cancel_Pressed()), workspace, SLOT(Key_Cancel_Pressed()));
    connect(editorArea, SIGNAL(Key_CTRL_A_Pressed()), workspace, SLOT(Key_CTRL_A_Pressed()));
    connect(editorArea, SIGNAL(elementsBeenHit(QVector<QPoint>)), workspace, SLOT(elementsBeenHit(QVector<QPoint>)));
    connect(editorArea, SIGNAL(elementsBeenHit(QVector<QPoint>)), mainWindow, SLOT(elementsBeenHit(QVector<QPoint>)));

    connect(this, SIGNAL(messageToBeDisplayed(QString)), mainWindow, SLOT(showStatusBarMessage(QString)));
    connect(this, SIGNAL(setCurs()), mainWindow, SLOT(setCurs()));
    connect(this, SIGNAL(restoreCurs()), mainWindow, SLOT(restoreCurs()));
    connect(this, SIGNAL(updateSignal()), editorArea, SLOT(updateRender()));
}

void AppController::loadGraphFromLayout()
{
    InputOutput* inputOutput = new InputOutput();
    connect(inputOutput, SIGNAL(curFileName(QString)), mainWindow, SLOT(setFileName(QString)));

    emit setCurs();

    clear();

    workspace->clear();

    inputOutput->loadGraphFromLayout(workspace->getGraph(), workspace->getGraphLayout(), workspace->getGraphProperties());

    QString theMessage(tr("Network opened"));
    emit updateSignal();
    emit messageToBeDisplayed(theMessage);
    emit restoreCurs();
}

void AppController::loadGraphFromGraph()
{
    InputOutput* inputOutput = new InputOutput();
    connect(inputOutput, SIGNAL(curFileName(QString)), mainWindow, SLOT(setFileName(QString)));

    emit setCurs();

    clear();

    workspace->clear();

    inputOutput->loadGraphFromGraph(workspace->getGraph(), workspace->getGraphLayout(), workspace->getGraphProperties());

    QString theMessage(tr("Network imported"));
    emit updateSignal();
    emit messageToBeDisplayed(theMessage);
    emit restoreCurs();
}

void AppController::loadMesh()
{
    InputOutput* inputOutput = new InputOutput();
    connect(inputOutput, SIGNAL(curFileName(QString)), mainWindow, SLOT(setFileName(QString)));

    emit setCurs();

    inputOutput->loadMesh(workspace->getGraphMesh());

    QString theMessage(tr("Mesh imported"));
    emit updateSignal();
    emit messageToBeDisplayed(theMessage);
    emit restoreCurs();
}

void AppController::generateMesh(const QString &fileName)
{
    if (!workspace->dataInGraph()) {
        return;
    }

    emit setCurs();

    QVector<int> nodes = workspace->getNodesIds();
    QVector<int> edges = workspace->getEdgesIds();

    InputOutput* inputOutput = new InputOutput();
    connect(inputOutput, SIGNAL(graphSaved(QString)), this, SLOT(graphHasBeenSaved(QString)));

    inputOutput->saveGraph(fileName, workspace->getGraph(), workspace->getGraphProperties(), nodes, edges);

    emit restoreCurs();
}

void AppController::graphHasBeenSaved(const QString &fileName)
{
    InputOutput* inputOutput = new InputOutput();

    inputOutput->generateMesh(fileName);
    connect(inputOutput, SIGNAL(meshFileReady(QString)), this, SLOT(meshHasBeenGenerated(QString)));
}

void AppController::meshHasBeenGenerated(const QString &fileName)
{
    InputOutput* inputOutput = new InputOutput();
    inputOutput->loadMeshAfterGenerating(fileName, workspace->getGraphMesh());
    emit updateSignal();
}


void AppController::saveNetwork(const QString& fileName)
{
    InputOutput* inputOutput = new InputOutput();
    connect(inputOutput, SIGNAL(curFileName(QString)), mainWindow, SLOT(setFileName(QString)));

    emit setCurs();

    QVector<int> nodes = workspace->getNodesIds();
    QVector<int> edges = workspace->getEdgesIds();

    inputOutput->saveNetwork(fileName, workspace->getGraph(), workspace->getGraphLayout(), workspace->getGraphProperties(),
                             nodes, edges);

    QString theMessage(tr("Network saved"));
    emit messageToBeDisplayed(theMessage);
    emit restoreCurs();
}

void AppController::setPreferences()
{
    InputOutput* inputOutput = new InputOutput();
    inputOutput->setPreferences();
}

void AppController::clear()
{
    requestMap.clear();
    mainWindow->removeAllDataCollectorFromDock();
    mainWindow->hideDock();
    dataCollectorList.clear();
    incrementalDataRequest = 0;
}

int AppController::uniqueDataRequestKey()
{
    while (requestMap.contains(incrementalDataRequest)) {
        incrementalDataRequest++;
    }
    return incrementalDataRequest;
}

void AppController::dataRequest(QPoint elementRequest)
{
    if (requestMap.values().contains(elementRequest)) {
        mainWindow->setPageInTab(dataCollectorList.value(requestMap.key(elementRequest)));
        mainWindow->showDock();
        return;
    }

    int requestKey = uniqueDataRequestKey();
    requestMap.insert(requestKey, elementRequest);
    QString cookie;
    cookie.setNum(requestKey);

    QString XMLString;

    if (elementRequest.x() == 1) {  // Element is a node.
        XMLString = workspace->getNodeProperties(elementRequest.y());
    } else {                        // Element is an edge.
        XMLString = workspace->getEdgeProperties(elementRequest.y());
    }

    if (XMLString.isEmpty()) {
        QString id;
        id.setNum(elementRequest.y());

        if (elementRequest.x() == 1) {  // Element is a node.
            XMLString += "<node id=\"" + id + "\"/>";
        } else {
            QPoint temp = workspace->getNodesOfEdge(elementRequest.y());
            QString node1_id;
            node1_id.setNum(temp.x());
            QString node2_id;
            node2_id.setNum(temp.y());
            XMLString += "<edge node1_id=\"" + node1_id + "\" node2_id=\"" + node2_id + "\" id=\"" + id + "\">\n" + "</edge>";
        }
    }

    QVector<QString> hiddenItems;
    hiddenItems << "id";

    QVector<QString> readOnlyItems;
    readOnlyItems << "node1_id" << "node2_id";

    DataCollector* dataCollector = new DataCollector(cookie, XMLString, hiddenItems, readOnlyItems);
    connect(dataCollector, SIGNAL(applyClicked(QString,QString)), this, SLOT(dataConfirmed(QString,QString)));
    connect(dataCollector, SIGNAL(okButtonClicked(QString,QString)), this, SLOT(dataConfirAndClose(QString,QString)));
    connect(dataCollector, SIGNAL(deleteItself()), dataCollector, SLOT(close()));

    dataCollectorList.insert(requestKey, dataCollector);

    mainWindow->insertDataCollectorToDock(dataCollectorList[requestKey], elementRequest);
    mainWindow->showDock();
}

void AppController::dataConfirmed(QString cookie,QString elementData)
{
    int requestKey = cookie.toInt();
    QPoint temp = requestMap.value(requestKey);

    QDomDocument doc("elementModifiedXML");
    doc.setContent(elementData);

    if (temp.x() == 1) {    // Element is a node.
        workspace->setNodeProperties(temp.y(), elementData);
        QDomNodeList nodeList = doc.elementsByTagName("node");
        QDomNode nodeNode = nodeList.at(0);
        QDomElement node = nodeNode.toElement();
        QString attrName = node.attribute("name");
        if (!attrName.isNull()) {
            workspace->setNodeName(temp.y(), attrName);
        }
    } else {                // Element is an edge.
        workspace->setEdgeProperties(temp.y(), elementData);
        QDomNodeList edgeList = doc.elementsByTagName("edge");
        QDomNode edgeNode = edgeList.at(0);
        QDomElement edge = edgeNode.toElement();
        QString attrName = edge.attribute("name");
        if (!attrName.isNull()) {
            workspace->setEdgeName(temp.y(), attrName);
        }
    }
}

void AppController::dataConfirAndClose(QString cookie, QString elementData)
{
    int requestKey = cookie.toInt();

    dataConfirmed(cookie, elementData);

    requestMap.remove(requestKey);
    dataCollectorList.remove(requestKey);

    mainWindow->removeDataCollectorFromDock();
}

void AppController::dockClosed()
{
    requestMap.clear();
    dataCollectorList.clear();
}
