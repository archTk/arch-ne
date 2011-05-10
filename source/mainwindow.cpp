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

#include <QtGui>
#include <QDockWidget>
#include <QGridLayout>

#include "mainwindow.h"
#include "editorarea.h"
#include "workspace.h"
#include "DataCollector/datacollector.h"
#include "dock.h"
#include "resultsdock.h"
#include "resultschoiceframe.h"
#include "resultsgroupbox.h"
#include "resultsview.h"

#include <QTextStream>
using namespace std;

QTextStream mainout(stdout);

MainWindow::MainWindow()
{
    editorArea = new EditorArea;
    setCentralWidget(editorArea);

    tabs = new QTabWidget;
    connect(tabs, SIGNAL(currentChanged(int)),this, SLOT(tabsContentChanged()));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();

    readSettings(); // It restores user's preferences.

    setCurrentFile(""); // It sets the window title.
    setUnifiedTitleAndToolBarOnMac(true);
}

EditorArea* MainWindow::getEditorArea()
{
    return editorArea;
}

//void MainWindow::initNewCase()
//{
//   clear();
//}

void MainWindow::insertDataCollectorToDock(DataCollector* theDataCollector, QPoint elementRequest)
{
    QString element;

    if (elementRequest.x() == 1) {
        element = "node";
    } else if (elementRequest.x() == 2) {
        element = "edge";
    } else if (elementRequest.x() == 3) {
        element = "BC";
    } else if (elementRequest.x() == 4) {
        element = "SP";
    } else if (elementRequest.x() == 5) {
        element = "Patient Info";
    }

    if (elementRequest.x() == 1 || elementRequest.x() == 2) {
        QString idString;
        idString.setNum(elementRequest.y());
        element.append(" ");
        element.append(idString);
    }

    dataCollectorList.insert(theDataCollector, elementRequest);

    tabs->insertTab(0, theDataCollector, element);
    tabs->setCurrentWidget(theDataCollector);
    emit editingEl2Ws(elementRequest);
    dock->setWidget(tabs);
}

void MainWindow::insertResultsViewToResultsDock(ResultsView* theResultsView, QPoint elementRequest)
{
    resultsGroupBox->addResultsView(theResultsView, elementRequest);
}

void MainWindow::removeAllDataCollectorFromDock()
{
    tabs->clear();
    dataCollectorList.clear();
    QPoint temp(-1, -1);
    emit editingEl2Ws(temp);
}

void MainWindow::removeAllResultsViewFromResultsDock()
{
    resultsGroupBox->removeAllResultsViewFromResultsDock();
    //resultsTabs->clear();
    //resultsViewList.clear();
    //QPoint temp(-1, -1);
    //emit editingEl2Ws(temp);
}

void MainWindow::removeDataCollectorFromDock()
{
    dataCollectorList.remove(tabs->currentWidget());
    tabs->removeTab(tabs->currentIndex());
    if (tabs->count() == 0) {
        hideDock();
    }
}

void MainWindow::removeResultsViewFromResultsDock()
{
    resultsGroupBox->removeResultsViewFromResultsDock();
    //resultsViewList.remove(resultsTabs->currentWidget());
    //resultsTabs->removeTab(resultsTabs->currentIndex());
    //if (resultsTabs->count() == 0) {
    //    hideResultsDock();
    //}
}

void MainWindow::setPageInTab(QWidget* theDataCollector)
{
    tabs->setCurrentWidget(theDataCollector);
    emit editingEl2Ws(dataCollectorList.value(tabs->currentWidget()));
}

void MainWindow::setPageInResultsTab(QWidget *theResultsView)
{
    resultsGroupBox->setPageInResultsTab(theResultsView);
    //resTab->setCurrentWidget(theResultsView);
    //emit editingEl2Ws(resultsViewList.value(resultsTabs->currentWidget()));
}

void MainWindow::tabsContentChanged()
{
    emit editingEl2Ws(dataCollectorList.value(tabs->currentWidget()));
}

void MainWindow::resultsTabsContentChanged()
{
    mainout << "MW::resultsTabsContentChanged" << endl;
    editorArea->setMeshElToBeHigh(resultsGroupBox->currVListValue().y());
    setPageInTab(dataCollectorList.key(editorArea->getGraphEl((resultsGroupBox->currVListValue()).y())));
    editorArea->update();
}

void MainWindow::mouseEnteredInDock()
{
    emit editingEl2Ws(dataCollectorList.value(tabs->currentWidget()));
}

void MainWindow::mouseEnteredInResultsDock()
{
    if (resultsGroupBox->resultsVList().isEmpty()) {
        return;
    }

    editorArea->setMeshElToBeHigh ((resultsGroupBox->currVListValue()).y());
    setPageInTab(dataCollectorList.key(editorArea->getGraphEl((resultsGroupBox->currVListValue()).y())));
}

void MainWindow::mouseLeftDock()
{
    emit editingEl2Ws(QPoint(-1, -1));
}

void MainWindow::mouseLeftResultsDock()
{
    editorArea->setMeshElToBeHigh(-1);
    emit editingEl2Ws(QPoint(-1, -1));
}

void MainWindow::dockClosed()
{
    removeAllDataCollectorFromDock();
}

void MainWindow::resultsDockClosed()
{
    removeAllResultsViewFromResultsDock();
}

void MainWindow::elementsBeenHit(QVector<QPoint> hitEls)
{
    if (hitEls[0].x() != -1) {
        QMapIterator<QWidget*, QPoint> dataColIter(dataCollectorList);
        while (dataColIter.hasNext()) {
            dataColIter.next();
            if (dataColIter.value() == hitEls[0]) {
                tabs->setCurrentWidget(dataColIter.key());
            }
        }
    }
}

void MainWindow::meshElsBeenHit(QVector<QPoint> hitMeshEls)
{
    if (hitMeshEls[0].x() != -1) {
        resultsGroupBox->meshElBeenHit(hitMeshEls);
    }
}

void MainWindow::hideDock()
{
    dock->hide();
}

void MainWindow::hideResultsDock()
{
    resultsDock->hide();
}

void MainWindow::showDock()
{
    dock->setMinimumSize(100, 100);
    dock->show();
}

void MainWindow::showResultsDock()
{
    //resultsDock->setMinimumSize(150, 100);
    resultsDock->show();
}

void MainWindow::createActions()
{
    newNetworkAct = new QAction(QIcon(":/images/new.png"), tr("&New Network"), this);
    newNetworkAct->setShortcuts(QKeySequence::New);
    newNetworkAct->setStatusTip(tr("Create a new network"));
    connect(newNetworkAct, SIGNAL(triggered()), this, SIGNAL(newNetwork()));

    openNetworkAct = new QAction(QIcon(":/images/openNet.png"), tr("&Open Network"), this);
    openNetworkAct->setShortcuts(QKeySequence::Open);
    openNetworkAct->setStatusTip(tr("Open a network saved by ARCH Network Editor"));
    connect(openNetworkAct, SIGNAL(triggered()), this, SIGNAL(openNetwork()));

    importNetworkAct = new QAction(QIcon(":/images/importNet.png"), tr("&Import Network"), this);
    importNetworkAct->setStatusTip(tr("Import a network"));
    connect(importNetworkAct, SIGNAL(triggered()), this, SIGNAL(importNetwork()));

    importBCAct = new QAction(QIcon(":/images/importBC.png"), tr("Import BC"), this);
    importBCAct->setStatusTip(tr("Import Boundary Conditions"));
    connect(importBCAct, SIGNAL(triggered()), this, SIGNAL(importBCPressed()));

    importMeshAct = new QAction(QIcon(":/images/importMesh.png"), tr("Import Mesh"), this);
    importMeshAct->setStatusTip(tr("Import an existing mesh"));
    connect(importMeshAct, SIGNAL(triggered()), this, SIGNAL(importMesh()));

    importResultsAct = new QAction(QIcon(":/images/importResults.png"), tr("Import Results"), this);
    importResultsAct->setStatusTip(tr("Import existing results"));
    connect(importResultsAct, SIGNAL(triggered()), this, SIGNAL(importResults()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save File"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the netwrok"));
    connect(saveAct, SIGNAL(triggered()), this, SIGNAL(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the network under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SIGNAL(saveAs()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the ARCHNetworkEditor"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    /////

    undoAct = new QAction(QIcon(":/images/undo.png"), tr("Undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    connect(undoAct, SIGNAL(triggered()), this, SIGNAL(undoPressed()));

    redoAct = new QAction(QIcon(":/images/redo.png"), tr("Redo"), this);
    redoAct->setShortcut(tr("Ctrl+shift+Z"));
    connect(redoAct, SIGNAL(triggered()), this, SIGNAL(redoPressed()));

    caseInfoAct = new QAction(QIcon(":/images/patientInfo.png"), tr("Case Info"), this);
    caseInfoAct->setStatusTip(tr("Set case information"));
    connect(caseInfoAct, SIGNAL(triggered()), this, SIGNAL(caseInfoPressed()));

    BCAct = new QAction(QIcon(":/images/BC.png"), tr("Set Boundary Conditions"), this);
    BCAct->setStatusTip(tr("Set Boundary Conditions"));
    connect(BCAct, SIGNAL(triggered()), this, SIGNAL(BCPressed()));

    customizeAct = new QAction(QIcon(":/images/custom.png"), tr("Personalize graph information"), this);
    customizeAct->setStatusTip(tr("Personalize graph information"));
    connect(customizeAct, SIGNAL(triggered()), this, SIGNAL(goCustomize()));

    simulateAct = new QAction(QIcon(":/images/simulation.png"), tr("Simulate"), this);
    simulateAct->setStatusTip(tr("Launch the simulation"));
    connect(simulateAct, SIGNAL(triggered()), this, SIGNAL(goSimulate()));
    /////

    addSegmentAct = new QAction(QIcon(":/images/addSegment.png"), tr("&Add Segment"), this);
    addSegmentAct->setStatusTip(tr("Create a new vascular segment"));
    connect(addSegmentAct, SIGNAL(triggered()), this, SIGNAL(addSegmentPressed()));

    bendSegmentAct = new QAction(QIcon(":/images/bendSegment.png"), tr("&Bend Segment"), this);
    bendSegmentAct->setStatusTip(tr("Bend vascular segments"));
    connect(bendSegmentAct, SIGNAL(triggered()), this, SIGNAL(bendSegmentPressed()));

    removeSegmentAct = new QAction(QIcon(":/images/removeSegment.png"), tr("&Remove Segment"), this);
    removeSegmentAct->setShortcut(tr("Ctrl+R"));
    removeSegmentAct->setStatusTip(tr("Remove an existing vascular segment"));
    connect(removeSegmentAct, SIGNAL(triggered()), this, SIGNAL(removeSegmentPressed()));

    splitSegmentAct = new QAction(QIcon(":/images/splitSegment.png"), tr("Spli&t Segment"), this);
    splitSegmentAct->setShortcut(tr("Ctrl+T"));
    splitSegmentAct->setStatusTip(tr("Split an existing vascular segment"));
    connect(splitSegmentAct, SIGNAL(triggered()), this, SIGNAL(splitSegmentPressed()));

    superEdgeAct = new QAction(QIcon(":/images/superEdge.png"), tr("Super Edge"), this);
    superEdgeAct->setStatusTip(tr("Create a super edge"));
    connect(superEdgeAct, SIGNAL(triggered()), this, SIGNAL(superEdgePressed()));

    selectElementsAct = new QAction(QIcon(":/images/moveGraph.png"), tr("Select Elements"), this);
    selectElementsAct->setShortcut(tr("Ctrl+shift+M"));
    selectElementsAct->setStatusTip(tr("Select part of the network"));
    connect(selectElementsAct, SIGNAL(triggered()), this, SIGNAL(selectElementsPressed()));

    resultsAct = new QAction(QIcon(":/images/results.png"), tr("Show results"), this);
    resultsAct->setStatusTip(tr("Show the simulation results"));
    connect(resultsAct, SIGNAL(triggered()), this, SIGNAL(resultsPressed()));

    meshAct = new QAction(QIcon(":/images/mesh.png"), tr("Mesh the network"), this);
    meshAct->setStatusTip(tr("Create the mesh for the network"));
    connect(meshAct, SIGNAL(triggered()), this, SIGNAL(goMesh()));

    defaultMeshAct = new QAction(QIcon(":/images/defaultMesh.png"), tr("Apply default mesh"), this);
    defaultMeshAct->setShortcut(tr("Ctrl+shift+D"));
    defaultMeshAct->setStatusTip(tr("Apply the default mesh to the network"));
    connect(defaultMeshAct, SIGNAL(triggered()), this, SIGNAL(defaultMeshPressed()));

    infoAct = new QAction(QIcon(":/images/info.png"), tr("&Edit Information"), this);
    infoAct->setShortcut(tr("Ctrl+I"));
    infoAct->setStatusTip(tr("Edit element properties"));
    connect(infoAct, SIGNAL(triggered()), this, SIGNAL(infoPressed()));
    /////

    zoomInAct = new QAction(QIcon(":/images/zoomIn.png"), tr("Zoom In"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setStatusTip(tr("Zoom in"));
    connect(zoomInAct, SIGNAL(triggered()), this, SIGNAL(zoomInPressed()));

    zoomOutAct = new QAction(QIcon(":/images/zoomOut.png"), tr("Zoom Out"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setStatusTip(tr("Zoom out"));
    connect(zoomOutAct, SIGNAL(triggered()), this, SIGNAL(zoomOutPressed()));

    translateAct = new QAction(QIcon(":/images/translate.png"), tr("Translate"), this);
    translateAct->setStatusTip(tr("Translate the scene"));
    connect(translateAct, SIGNAL(triggered()), this, SIGNAL(translatePressed()));

    homeViewAct = new QAction(QIcon(":/images/home.png"), tr("Reset View"), this);
    homeViewAct->setShortcut(tr("Ctrl+0"));
    homeViewAct->setStatusTip(tr("Reset the scene view"));
    connect(homeViewAct, SIGNAL(triggered()), this, SIGNAL(homeViewPressed()));

    showGridAct = new QAction(QIcon(":/images/grid.png"), tr("Grid"), this);
    showGridAct->setStatusTip(tr("Grid"));
    connect(showGridAct, SIGNAL(triggered()), this, SIGNAL(showGridPressed()));

    snapToGridAct = new QAction(QIcon(":/images/snapToGrid.png"), tr("Snap To Grid"), this);
    snapToGridAct->setStatusTip(tr("Snap To Grid"));
    connect(snapToGridAct, SIGNAL(triggered()), this, SIGNAL(snapToGridPressed()));

    showLabelsAct = new QAction(QIcon(":/images/showLabel.png"), tr("Show labels"), this);
    showLabelsAct->setStatusTip(tr("Show labels"));
    connect(showLabelsAct, SIGNAL(triggered()), this, SIGNAL(showLabelsPressed()));

    showMeshAct = new QAction(QIcon(":/images/showMesh.png"), tr("Show mesh"), this);
    showMeshAct->setStatusTip(tr("Show mesh elements"));
    connect(showMeshAct, SIGNAL(triggered()), this, SIGNAL(showMeshPressed()));

    unravelNetAct = new QAction(QIcon(":/images/unravel.png"), tr("Unravel Network"), this);
    unravelNetAct->setShortcut(tr("Ctrl+U"));
    unravelNetAct->setStatusTip(tr("Unravel the network"));
    connect(unravelNetAct, SIGNAL(triggered()), this, SIGNAL(unravelNetPressed()));

    blockNodesAct = new QAction(QIcon(":/images/blockNodes.png"), tr("Block node position"), this);
    blockNodesAct->setShortcut(tr("Ctrl+B"));
    blockNodesAct->setStatusTip(tr("Block the nodes position"));
    connect(blockNodesAct, SIGNAL(triggered()), this, SIGNAL(blockNodesPressed()));
    /////

    aboutAct = new QAction(tr("About ARCHNetworkEditor"), this);
    aboutAct->setStatusTip(tr("Show the ARCHNetworkEditor's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    preferencesAct = new QAction(tr("preferences..."), this);
    preferencesAct->setShortcuts(QKeySequence::Preferences);
    preferencesAct->setStatusTip(tr("Set application preferences"));
    connect(preferencesAct, SIGNAL(triggered()), this, SIGNAL(setPrefPressed()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newNetworkAct);
    fileMenu->addAction(openNetworkAct);
    fileMenu->addAction(importNetworkAct);
    fileMenu->addAction(importBCAct);
    fileMenu->addAction(importMeshAct);
    fileMenu->addAction(importResultsAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(aboutAct);
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(BCAct);
    //editMenu->addAction(SPAct);
    //editMenu->addAction(importSPAct);
    editMenu->addAction(caseInfoAct);
    //editMenu->addAction(importPatientInfoAct);
    editMenu->addSeparator();
    editMenu->addAction(preferencesAct);

    menuBar()->addSeparator();

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(translateAct);
    viewMenu->addAction(homeViewAct);
    viewMenu->addAction(showGridAct);
    viewMenu->addAction(snapToGridAct);
    viewMenu->addAction(showLabelsAct);
    viewMenu->addAction(showMeshAct);
    viewMenu->addAction(unravelNetAct);
    viewMenu->addAction(blockNodesAct);

    operationMenu = menuBar()->addMenu(tr("&Operations"));
    operationMenu->addAction(addSegmentAct);
    operationMenu->addAction(bendSegmentAct);
    operationMenu->addAction(removeSegmentAct);
    operationMenu->addAction(splitSegmentAct);
    operationMenu->addAction(superEdgeAct);
    operationMenu->addAction(selectElementsAct);
    operationMenu->addAction(infoAct);
    operationMenu->addAction(resultsAct);
    operationMenu->addSeparator();
    operationMenu->addAction(BCAct);
    //operationMenu->addAction(SPAct);
    operationMenu->addAction(customizeAct);
    operationMenu->addAction(meshAct);
    operationMenu->addAction(defaultMeshAct);
    operationMenu->addAction(simulateAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(aboutAct);

    addSegmentAct->setCheckable(true);
    bendSegmentAct->setCheckable(true);
    removeSegmentAct->setCheckable(true);
    splitSegmentAct->setCheckable(true);
    superEdgeAct->setCheckable(true);
    selectElementsAct->setCheckable(true);
    resultsAct->setCheckable(true);
    infoAct->setCheckable(true);

    showLabelsAct->setCheckable(true);
    showLabelsAct->setChecked(false);
    showMeshAct->setCheckable(true);
    showMeshAct->setChecked(true);

    selectElementsAct->setChecked(true);

    translateAct->setCheckable(true);
    showGridAct->setCheckable(true);

    showGridAct->setChecked(true);
    snapToGridAct->setCheckable(true);
    snapToGridAct->setChecked(true);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newNetworkAct);
    fileToolBar->addAction(openNetworkAct);
    fileToolBar->addAction(importNetworkAct);
    fileToolBar->addAction(importBCAct);
    fileToolBar->addAction(importMeshAct);
    fileToolBar->addAction(importResultsAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->setMovable(false);

    viewToolBar = addToolBar(tr("View"));
    viewToolBar->setAllowedAreas(Qt::TopToolBarArea);
    viewToolBar->addAction(zoomInAct);
    viewToolBar->addAction(zoomOutAct);
    viewToolBar->addAction(translateAct);
    viewToolBar->addAction(homeViewAct);
    viewToolBar->addAction(showGridAct);
    viewToolBar->addAction(snapToGridAct);
    viewToolBar->addAction(showLabelsAct);
    viewToolBar->addAction(showMeshAct);
    viewToolBar->addAction(unravelNetAct);
    viewToolBar->addAction(blockNodesAct);

    operationToolBar = addToolBar(tr("Operations"));
    addToolBar(Qt::LeftToolBarArea, operationToolBar);
    operationToolBar->setMovable(true);

    operationGroup = new QActionGroup(this);
    operationGroup->addAction(addSegmentAct);
    operationGroup->addAction(bendSegmentAct);
    operationGroup->addAction(removeSegmentAct);
    operationGroup->addAction(splitSegmentAct);
    operationGroup->addAction(infoAct);
    operationGroup->addAction(translateAct);
    operationGroup->addAction(superEdgeAct);
    operationGroup->addAction(selectElementsAct);
    operationGroup->addAction(resultsAct);
    operationGroup->setExclusive(true);

    operationToolBar->setMovable(true);
    operationToolBar->addAction(addSegmentAct);
    operationToolBar->addAction(bendSegmentAct);
    operationToolBar->addAction(removeSegmentAct);
    operationToolBar->addAction(splitSegmentAct);
    operationToolBar->addAction(superEdgeAct);
    operationToolBar->addAction(selectElementsAct);
    operationToolBar->addAction(infoAct);
    operationToolBar->addAction(resultsAct);
    operationToolBar->addSeparator();
    operationToolBar->addAction(BCAct);
    //operationToolBar->addAction(SPAct);
    operationToolBar->addAction(caseInfoAct);
    operationToolBar->addAction(customizeAct);
    operationToolBar->addAction(meshAct);
    operationToolBar->addAction(defaultMeshAct);
    operationToolBar->addAction(simulateAct);
    //operationToolBar->addAction(importSPAct);
    //operationToolBar->addAction(importPatientInfoAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createDockWindows()
{
    dock = new Dock(this);
    dock->setWindowTitle("Info");
    connect(dock, SIGNAL(mouseEnteredInDock()), this, SLOT(mouseEnteredInDock()));
    connect(dock, SIGNAL(mouseLeftDock()), this, SLOT(mouseLeftDock()));
    connect(dock, SIGNAL(dockClosed()), this, SLOT(dockClosed()));
    connect(dock, SIGNAL(dockClosed()), this, SIGNAL(dockClosedSig()));
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->hide();

    resultsGroupBox = new ResultsGroupBox(this);
    connect(resultsGroupBox, SIGNAL(visualizingEl2Ws(QPoint)), this, SIGNAL(editingEl2Ws(QPoint)));
    connect(resultsGroupBox, SIGNAL(result2BeDisaplyedChanged(int)), this, SIGNAL(resultToDisplay(int)));
    connect(resultsGroupBox, SIGNAL(resTabsContentChanged()), this, SLOT(resultsTabsContentChanged()));

    resultsDock = new ResultsDock(this);
    resultsDock->setWindowTitle("Results");
    resultsDock->setWidget(resultsGroupBox);

    connect(resultsDock, SIGNAL(mouseEnteredInResultsDock()), this, SLOT(mouseEnteredInResultsDock()));
    connect(resultsDock, SIGNAL(mouseLeftResultsDock()), this, SLOT(mouseLeftResultsDock()));
    connect(resultsDock, SIGNAL(resultsDockClosed()), this, SLOT(resultsDockClosed()));
    connect(resultsDock, SIGNAL(resultsDockClosed()), this, SIGNAL(resultsDockClosedSig()));
    resultsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, resultsDock);
    resultsDock->hide();
}


void MainWindow::readSettings()
{
    QSettings settings("archTk", "ARCHNetworkEditor");
    QPoint pos = settings.value("pos", QPoint(0, 0)).toPoint();
    QSize size = settings.value("size", QSize(800, 600)).toSize();
    int opToolBarPos = settings.value("operationToolBarPosition", int(1)).toInt();

    resize(size);
    move(pos);
    addToolBar((Qt::ToolBarArea)opToolBarPos, operationToolBar);
}

void MainWindow::writeSettings()
{   
    QSettings settings("archTk", "ARCHNetworkEditor");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("operationToolBarPosition", toolBarArea(operationToolBar));
}

void MainWindow::setCurrentFile(const QString& fileName)
{
    //curFile = fileName;
    setWindowModified(false);

    QString shownName = fileName;
    if (fileName.isEmpty()) {
        shownName = "untitled";
    }
    setWindowFilePath(shownName);
}

//QString MainWindow::strippedName(const QString& fullFileName)
//{
//    return QFileInfo(fullFileName).fileName();
//}

//void MainWindow::newNet()
//{


    //if (maybeSave()) {
        //emit currentFileAndWDir(QString(), QString());
        //setCurrentFile("");
        //clear();
        //emit initNewCase();
    //}
//}

//void MainWindow::openNetwork()
//{
//    if (maybeSave()) {
//        loadNetwork();
//    }
//}

//void MainWindow::importNetwork()
//{
//    if (maybeSave()) {
//        loadNetWithoutLayout();
//    }
//}

//void MainWindow::importMesh()
//{
//    if (!editorArea->getWorkspace()->dataInGraph()) {
//        QMessageBox msgBox;
//        msgBox.setText("You need to have a network\n"
//                       "before importing a mesh.");
//        msgBox.exec();
//        return;
//    }

//    loadMesh();
//}

//void MainWindow::customizePressed()
//{
//    emit graphToBeCustomized(curFile);
//}

void MainWindow::closeEvent(QCloseEvent* event)
{
    emit closeEventSignal(event);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About ARCHNetworkEditor"),
                       tr("The ARCHNetworkEditor has been\n"
                          "developed in the context of the FP7.\n\n"
                          "Powered by OROBIX"));
}

void MainWindow::documentWasModified()
{
    setWindowModified(true);
}

void MainWindow::updateMainWindow()
{
    bool gridStatus = editorArea->getWorkspace()->getGridStatus();
    bool snapStatus = editorArea->getWorkspace()->getSnapToGridStatus();
    bool labelsStatus = editorArea->getWorkspace()->getShowLabelsStatus();
    bool meshStatus = editorArea->getWorkspace()->getShowMeshStatus();
    int selectedTool = editorArea->getWorkspace()->getSelectedTool();

    if (gridStatus) {
        showGridAct->setChecked(true);
    } else {
        showGridAct->setChecked(false);
    }

    if (snapStatus) {
        snapToGridAct->setChecked(true);
    } else {
        snapToGridAct->setChecked(false);
    }

    if (labelsStatus) {
        showLabelsAct->setChecked(true);
    } else {
        showLabelsAct->setChecked(false);
    }

    if (meshStatus) {
        showMeshAct->setChecked(true);
    } else {
        showMeshAct->setChecked(false);
    }

    addSegmentAct->setChecked(false);
    removeSegmentAct->setChecked(false);
    splitSegmentAct->setChecked(false);
    superEdgeAct->setChecked(false);
    selectElementsAct->setChecked(false);
    infoAct->setChecked(false);
    translateAct->setChecked(false);

    switch (selectedTool) {
    case 0:
        addSegmentAct->setChecked(true);
        break;
    case 1:
        removeSegmentAct->setChecked(true);
        break;
    case 2:
        splitSegmentAct->setChecked(true);
        break;
    case 3:
        superEdgeAct->setChecked(true);
        break;
    case 4:
        selectElementsAct->setChecked(true);
        break;
    case 5:
        infoAct->setChecked(true);
        break;
    case 6:
        translateAct->setChecked(true);
        break;
    }
}

//QString MainWindow::getFileName()
//{
//    return curFile;
//}

void MainWindow::clear()
{
    removeAllDataCollectorFromDock();
    removeAllResultsViewFromResultsDock();
    hideDock();
    hideResultsDock();
    dataCollectorList.clear();
    resultsGroupBox->clear();
}

void MainWindow::showStatusBarMessage(QString theMessage)
{
    statusBar()->showMessage(theMessage, 2000);
}

void MainWindow::setFileName(QString theName)
{
    setCurrentFile(theName);
}

void MainWindow::setCurs()
{
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
}

void MainWindow::restoreCurs()
{
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}
