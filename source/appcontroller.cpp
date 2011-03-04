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
#include "dialog.h"
#include "mainwindow.h"
#include "workspace.h"
#include "editorarea.h"
#include "inputoutput.h"
#include "DataCollector/datacollector.h"
#include "resultsview.h"

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
    incrementalResultsRequest = 0;

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
    connect(mainWindow, SIGNAL(BCPressed()), this, SLOT(BCPressed()));
    connect(mainWindow, SIGNAL(blockNodesPressed()), workspace, SLOT(blockNodes()));
    connect(mainWindow, SIGNAL(defaultMeshPressed()), workspace, SLOT(applyDefaultMesh()));
    connect(mainWindow, SIGNAL(dockClosedSig()), this, SLOT(dockClosed()));
    connect(mainWindow, SIGNAL(editingEl2Ws(QPoint)), workspace, SLOT(setHighlightingEl(QPoint)));
    connect(mainWindow, SIGNAL(homeViewPressed()), workspace, SLOT(homeView()));
    connect(mainWindow, SIGNAL(graphToBeCustomized(QString)), this, SLOT(customizeGraph(QString)));
    connect(mainWindow, SIGNAL(graphToBeSimulated(QString)), this, SLOT(simulateGraph(QString)));
    connect(mainWindow, SIGNAL(importBCPressed()), this, SLOT(importBC()));
    //connect(mainWindow, SIGNAL(importPatientInfoPressed()), this, SLOT(importPatientInfo()));
    //connect(mainWindow, SIGNAL(importSPPressed()), this, SLOT(importSP()));
    connect(mainWindow, SIGNAL(infoPressed()), workspace, SLOT(info()));
    connect(mainWindow, SIGNAL(meshToBeGenerated(QString)), this, SLOT(generateMesh(QString)));
    //connect(mainWindow, SIGNAL(patientInfoPressed()), this, SLOT(patientInfoPressed()));
    connect(mainWindow, SIGNAL(redoPressed()), workspace, SLOT(redo()));
    connect(mainWindow, SIGNAL(removeSegmentPressed()), workspace, SLOT(removeSegment()));
    connect(mainWindow, SIGNAL(resultsDockClosedSig()), this, SLOT(resultsDockClosed()));
    connect(mainWindow, SIGNAL(resultsPressed()), workspace, SLOT(resultsRequest()));
    connect(mainWindow, SIGNAL(selectElementsPressed()), workspace, SLOT(selectElements()));
    connect(mainWindow, SIGNAL(setPrefPressed()), this, SLOT(setPreferences()));
    connect(mainWindow, SIGNAL(showGridPressed()), workspace, SLOT(showGrid()));
    connect(mainWindow, SIGNAL(showLabelsPressed()), workspace, SLOT(showLabels()));
    connect(mainWindow, SIGNAL(showMeshPressed()), workspace, SLOT(showMesh()));
    connect(mainWindow, SIGNAL(snapToGridPressed()), workspace, SLOT(snapToGrid()));
    connect(mainWindow, SIGNAL(splitSegmentPressed()), workspace, SLOT(splitSegment()));
    //connect(mainWindow, SIGNAL(SPPressed()), this, SLOT(SPPressed()));
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
    connect(workspace, SIGNAL(showResults(QPoint)), this, SLOT(showResults(QPoint)));
    connect(workspace, SIGNAL(updateSignal()), editorArea, SLOT(updateRender()));
    connect(workspace, SIGNAL(noMeshHitEls()), editorArea, SLOT(resetMeshElToBeHigh()));

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
    connect(editorArea, SIGNAL(meshElsBeenHit(QVector<QPoint>)), workspace, SLOT(meshElsBeenHit(QVector<QPoint>)));
    connect(editorArea, SIGNAL(meshElsBeenHit(QVector<QPoint>)), mainWindow, SLOT(meshElsBeenHit(QVector<QPoint>)));

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

void AppController::importBC()
{
    InputOutput* inputOutput = new InputOutput();
    inputOutput->importBC(workspace->getNetworkProperties());

    emit messageToBeDisplayed("Boundary Conditions have been imported");
}

/*void AppController::importPatientInfo()
{
    InputOutput* inputOutput = new InputOutput();
    inputOutput->importPatientInfo(workspace->getNetworkProperties());

    emit messageToBeDisplayed("Patient Info have been imported");
}*/

/*void AppController::importSP()
{
    InputOutput* inputOutput = new InputOutput();
    inputOutput->importSP(workspace->getNetworkProperties());

    emit messageToBeDisplayed("Simulation Parameters have been imported");
}*/

void AppController::generateMesh(const QString &fileName)
{
    if (!workspace->dataInGraph()) {
        return;
    }

    emit setCurs();

    QVector<int> nodes = workspace->getNodesIds();
    QVector<int> edges = workspace->getEdgesIds();

    InputOutput* inputOutput = new InputOutput();
    connect(inputOutput, SIGNAL(graphSaved(QString)), this, SLOT(goMeshing(QString)));

    inputOutput->saveGraph(fileName, workspace->getGraphProperties(), nodes, edges);
}

void AppController::goMeshing(const QString &fileName)
{
    QSettings settings("archTk", "ARCHNetworkEditor");
    QString pythonPath = settings.value("pythonPath", QString()).toString();
    QString pyNSPath = settings.value("pyNSPath", QString()).toString();

    if (!checkPaths(pythonPath, pyNSPath)) {
        return;
    }

    meshOut = fileName;
    meshOut.remove("_graph.xml");
    meshOut.append("_mesh.xml");

    QString scriptPath;
    scriptPath = pyNSPath + "/MeshGenerator_Script.py";

    QStringList arguments;
    arguments << scriptPath << "-i" << fileName << "-o" << meshOut << "-v" << "5e-2";

    pyNS = new QProcess(this);

    connect(pyNS, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(meshHasBeenGenerated()));
    connect(pyNS, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorFromExternal(QProcess::ProcessError)));
    pyNS->start(pythonPath, arguments);
}

void AppController::errorFromExternal(QProcess::ProcessError)
{
    int err = pyNS->error();
    appout << "error from external process: " << err << endl;
}

void AppController::BCPressed()
{
    /*QString curFile = mainWindow->getFileName();
    if (curFile.isEmpty()) {
        showMessage("WARNING!", "The file has been saved.\n"
                                "Please save it first");
        return;
    }*/

    QPoint elementRequest(3, 0);
    QString XMLString;
    XMLString = workspace->getBCXML();

    if (XMLString.isEmpty()) {
        XMLString = "<boundary_condition/>";
    }

    QVector<QString> hiddenItems;
    hiddenItems.clear();

    QVector<QString> readOnlyItems;
    readOnlyItems.clear();

    QString XMLSchema(":XMLschema/boundary_conditions.xsd");

    collectData(elementRequest, XMLString, hiddenItems, readOnlyItems, XMLSchema);

    /*InputOutput* inputOutput = new InputOutput();
    inputOutput->saveBC(curFile, workspace->getBCXML());*/
}

/*void AppController::SPPressed()
{
    QPoint elementRequest(4, 0);
    QString XMLString;
    XMLString = workspace->getSPXML();
    //XMLString = workspace->getBCXML();

    if (XMLString.isEmpty()) {
        XMLString = "<simulation_parameters/>";
    }

    QVector<QString> hiddenItems;
    hiddenItems.clear();

    QVector<QString> readOnlyItems;
    readOnlyItems.clear();

    QString XMLSchema(":XMLschema/boundary_conditions.xsd");

    collectData(elementRequest, XMLString, hiddenItems, readOnlyItems, XMLSchema);
}*/

/*void AppController::patientInfoPressed()
{
    QPoint elementRequest(5, 0);
    QString XMLString;
    XMLString = workspace->getPatientInfoXML();

    if (XMLString.isEmpty()) {
        XMLString = "<patient_data/>";
    }

    QVector<QString> hiddenItems;
    hiddenItems.clear();

    QVector<QString> readOnlyItems;
    readOnlyItems.clear();

    QString XMLSchema(":XMLschema/boundary_conditions.xsd");

    collectData(elementRequest, XMLString, hiddenItems, readOnlyItems, XMLSchema);
}*/

void AppController::meshHasBeenGenerated()
{
    InputOutput* inputOutput = new InputOutput();
    inputOutput->loadMeshAfterGenerating(meshOut, workspace->getGraphMesh());
    emit updateSignal();
    emit restoreCurs();
    emit messageToBeDisplayed(tr("Mesh has been generated"));
}

void AppController::customizeGraph(const QString &fileName)
{
    if (!workspace->dataInGraph()) {
        return;
    }

    emit setCurs();

    QVector<int> nodes = workspace->getNodesIds();
    QVector<int> edges = workspace->getEdgesIds();

    InputOutput* inputOutput = new InputOutput();
    connect(inputOutput, SIGNAL(graphSaved(QString)), this, SLOT(goCustomizing(QString)));

    inputOutput->saveGraph(fileName, workspace->getGraphProperties(), nodes, edges);
}

void AppController::goCustomizing(const QString &fileName)
{
     appout << "AppC::goCustom fileName= " << fileName << endl;

    QSettings settings("archTk", "ARCHNetworkEditor");
    QString pythonPath = settings.value("pythonPath", QString()).toString();
    QString pyNSPath = settings.value("pyNSPath", QString()).toString();

    if (!checkPaths(pythonPath, pyNSPath)) {
        return;
    }

    QString scriptPath;
    scriptPath = pyNSPath + "/ModelAdaptor_Script.py";
    QFileInfo fileInfo(fileName);
    QString workDir = fileInfo.path() + "/";
    QString xmlNet = fileInfo.fileName();

    QStringList arguments;
    arguments << scriptPath << "--wdir" << workDir << "--xmlNet" << xmlNet << "--xmlBound" << workDir + "boundary_conditions.xml";

    appout << "AppController::goCustomizing check arguments" << endl;

    pyNS = new QProcess(this);

    connect(pyNS, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(graphHasBeenCustomized()));
    connect(pyNS, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorFromExternal(QProcess::ProcessError)));
    pyNS->start(pythonPath, arguments);
}

void AppController::graphHasBeenCustomized()
{
    appout << "AppController::graphHasBeenCustomized synchronize with DataCollectot (XML has changed)" << endl;
    emit restoreCurs();
    emit messageToBeDisplayed(tr("The graph has been customized"));
}

void AppController::simulateGraph(const QString &fileName)
{
    if (!workspace->dataInGraphMesh()) {
        return;
    }

    emit setCurs();

    QFileInfo fileInfo(fileName);
    QString workDir = fileInfo.path() + "/";
    QString outDir = workDir + "Output/";
    imagesDir = workDir + "Images/";
    QString xmlOut = fileName + "_results.xml";

    QSettings settings("archTk", "ARCHNetworkEditor");
    QString pythonPath = settings.value("pythonPath", QString()).toString();
    QString pyNSPath = settings.value("pyNSPath", QString()).toString();

    if (checkPaths(pythonPath, pyNSPath)) {
        return;
    }

    QString scriptPath;
    scriptPath = pyNSPath + "/Solver_Script.py";

    QStringList arguments;
    arguments << scriptPath << "-w" << workDir << "-o" << outDir <<  "-i" << imagesDir << "-m" << meshOut
            << "-t" << xmlOut << "-b" << workspace->getBCXML();

    /*
    appout << "AppC::simulateGraph" << endl << "scriptPath= " << scriptPath << endl << "workDir= " << workDir
            << endl << "outDir= " << outDir << endl << "imagesDir= " << imagesDir << endl << "meshOut= " << meshOut
            << endl << "xmlOut= " << xmlOut << endl << "BC= " << workspace->getBCXML() << endl;
    */

    pyNS = new QProcess(this);

    connect(pyNS, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(simulationHasBeenPerformed()));
    connect(pyNS, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorFromExternal(QProcess::ProcessError)));
    pyNS->start(pythonPath, arguments);
}

void AppController::simulationHasBeenPerformed()
{
    emit restoreCurs();
    emit messageToBeDisplayed(tr("Simulation has been completed"));
}

void AppController::saveNetwork(const QString& fileName)
{
    InputOutput* inputOutput = new InputOutput();
    connect(inputOutput, SIGNAL(curFileName(QString)), mainWindow, SLOT(setFileName(QString)));

    emit setCurs();

    QVector<int> nodes = workspace->getNodesIds();
    QVector<int> edges = workspace->getEdgesIds();

    inputOutput->saveNetwork(fileName, workspace->getGraphLayout(), workspace->getGraphProperties(),
                             nodes, edges);

    QString theMessage(tr("Network saved"));
    emit messageToBeDisplayed(theMessage);
    emit restoreCurs();
}

void AppController::setPreferences()
{
    Dialog propDialog;

    propDialog.exec();
}

void AppController::clear()
{
    mainWindow->clear();
    requestMap.clear();
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

int AppController::uniqueResultsRequestKey()
{
    while (resultsMap.contains(incrementalResultsRequest)) {
        incrementalResultsRequest++;
    }
    return incrementalResultsRequest;
}

void AppController::showResults(QPoint elementRequest)
{
    if (resultsMap.values().contains(elementRequest)) {
        mainWindow->setPageInResultsTab(resultsViewList.value(resultsMap.key(elementRequest)));
        mainWindow->showResultsDock();
        return;
    }

    int requestKey = uniqueResultsRequestKey();
    resultsMap.insert(requestKey, elementRequest);
    QString cookie;
    cookie.setNum(requestKey);

    QString pressureImageName, flowImageName;

    /*
    if (elementRequest.x() == 1 ) {         // MeshElement corresponds to a node of the graph.
        pressureImageName += imagesDir + workspace->getNodeName(elementRequest.y()) + "_pressure.png";
        flowImageName += imagesDir + workspace->getNodeName(elementRequest.y()) + "_flow_png";
    } else if (elementRequest.x() == 2) {   // MeshElement correspond to a segment of an edge of the graph.
        pressureImageName += imagesDir + workspace->getEdgeName(elementRequest.y()) + "_pressure";
        flowImageName += imagesDir + workspace->getEdgeName(elementRequest.y()) + "_flow_png";
    }
    */

    QPixmap pressureImage, flowImage;
    //pressureImage.load(pressureImageName);
    //flowImage.load(flowImageName);
    pressureImage.load(":images/aorta_asc_1_pressure.png");
    flowImage.load(":images/aorta_asc_1_flow.png");

    QString PMean = QString("100");
    QString FMean = QString("150");

    ResultsView* resultsView = new ResultsView(cookie, pressureImage, flowImage, PMean, FMean);

    connect(resultsView, SIGNAL(okButtonClicked(QString)), this, SLOT(closeResultsView(QString)));
    connect(resultsView, SIGNAL(deleteItself()), resultsView, SLOT(close()));

    resultsViewList.insert(requestKey, resultsView);

    mainWindow->insertResultsViewToResultsDock(resultsView, elementRequest);
    mainWindow->showResultsDock();
}

void AppController::dataRequest(QPoint elementRequest)
{
    QString XMLString;
    QString XMLSchema;

    if (elementRequest.x() == 1) {          // Element is a node.
        XMLString = workspace->getNodeProperties(elementRequest.y());
        XMLSchema = ":XMLschema/schema.xsd";
    } else if (elementRequest.x() == 2) {   // Element is an edge.
        XMLString = workspace->getEdgeProperties(elementRequest.y());
        XMLSchema = ":XMLschema/schema.xsd";
    }

    if (XMLString.isEmpty()) {
        QString id;
        id.setNum(elementRequest.y());

        if (elementRequest.x() == 1) {          // Element is a node.
            XMLString += "<node id=\"" + id + "\"/>";
        } else if (elementRequest.x() == 2) {   // Element is an edge.
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

    collectData(elementRequest, XMLString, hiddenItems, readOnlyItems, XMLSchema);
}

void AppController::collectData(QPoint elementRequest, QString XMLString, QVector<QString> hiddenItems, QVector<QString> readOnlyItems, QString XMLSchema)
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

    DataCollector* dataCollector = new DataCollector(cookie, XMLString, hiddenItems, readOnlyItems);
    dataCollector->setRequestSchemaPath(XMLSchema);
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

    if (temp.x() == 1) {        // Element is a node.
        workspace->setNodeProperties(temp.y(), elementData);
        QDomNodeList nodeList = doc.elementsByTagName("node");
        QDomNode nodeNode = nodeList.at(0);
        QDomElement node = nodeNode.toElement();
        QString attrName = node.attribute("name");
        if (!attrName.isNull()) {
            workspace->setNodeName(temp.y(), attrName);
        }
    } else if (temp.x() == 2) { // Element is an edge.
        workspace->setEdgeProperties(temp.y(), elementData);
        QDomNodeList edgeList = doc.elementsByTagName("edge");
        QDomNode edgeNode = edgeList.at(0);
        QDomElement edge = edgeNode.toElement();
        QString attrName = edge.attribute("name");
        if (!attrName.isNull()) {
            workspace->setEdgeName(temp.y(), attrName);
        }
    } else if (temp.x() == 3) { // Boundary Conditions.
        workspace->setBCXML(elementData);
    } /*else if (temp.x() == 4) { // Simulation Parameters.
        workspace->setSPXML(elementData);
    } else if (temp.x() == 5) { // Patient information.
        workspace->setPatientInfoXML(elementData);
    }*/
}

void AppController::dataConfirAndClose(QString cookie, QString elementData)
{
    int requestKey = cookie.toInt();

    dataConfirmed(cookie, elementData);

    requestMap.remove(requestKey);
    dataCollectorList.remove(requestKey);

    mainWindow->removeDataCollectorFromDock();
}

void AppController::closeResultsView(QString cookie)
{
    int resultsKey = cookie.toInt();

    resultsMap.remove(resultsKey);
    resultsViewList.remove(resultsKey);

    mainWindow->removeResultsViewFromResultsDock();
}

void AppController::dockClosed()
{
    requestMap.clear();
    dataCollectorList.clear();
}

void AppController::resultsDockClosed()
{
    resultsMap.clear();
    resultsViewList.clear();
}

void AppController::showMessage(QString theTitle, QString theMessage)
{
    QMessageBox messBox(0);
    messBox.setWindowTitle(theTitle);
    messBox.setText(theMessage);
    messBox.addButton(QMessageBox::Ok);

    messBox.exec();
}

bool AppController::checkPaths(QString pythonPath, QString pyNSPath)
{
    if (pythonPath.isEmpty()) {
        emit restoreCurs();
        showMessage(tr ("WARNING!"), tr("Path to python has not been set.\nPlease set it in Preferences..."));
        return false;
    }

    if (pyNSPath.isEmpty()) {
        emit restoreCurs();
        showMessage(tr ("WARNING!"), tr("Path to pyNS has not been set.\nPlease set it in Preferences..."));
        return false;
    }

    return true;
}
