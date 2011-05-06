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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class ResultsView;

class QAction;
class QLabel;
class QMenu;
class QFile;
class QActionGroup;
class QGridLayout;
class QString;

class EditorArea;
class DataCollector;
class ResultsGroupBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    EditorArea* getEditorArea();

    //void initNewCase();
    void clear();

    void setCurrentFile(const QString& fileName);

    void insertDataCollectorToDock(DataCollector* theDataCollector, QPoint  elementRequest);
    void removeAllDataCollectorFromDock();
    void removeDataCollectorFromDock();
    void setPageInTab(QWidget* theDataCollector);

    void insertResultsViewToResultsDock(ResultsView* theResultsView, QPoint elementRequest);
    void removeAllResultsViewFromResultsDock();
    void removeResultsViewFromResultsDock();
    void setPageInResultsTab(QWidget* theResultsView);

    void hideDock();
    void hideResultsDock();
    void showDock();
    void showResultsDock();

    void writeSettings();

    //QString getFileName();

signals:
    void addSegmentPressed();
    void BCPressed();
    void bendSegmentPressed();
    void blockNodesPressed();
    void caseInfoPressed();
    void closeEventSignal(QCloseEvent* event);
    void currentFNameAndWDir(QString theFName, QString theWDir);
    void defaultMeshPressed();
    void dockClosedSig();
    void editingEl2Ws(QPoint editingElement);
    void goCustomize();
    void goMesh();
    void goSimulate();
    void graphToBeCustomized(const QString& fileName);
    void graphToBeSimulated(const QString& fileName);
    void homeViewPressed();
    void importBCPressed();
    void importMesh();
    void importNetwork();
    //void importSPPressed();
    void infoPressed();
    //void initNewCase();
    void loadGraphFromLayout();
    void loadGraphFromGraph();
    void meshToBeLoaded();
    void newNetwork();
    void openNetwork();
    void redoPressed();
    void removeSegmentPressed();
    void resultToDisplay(int theResult);
    void resultsDockClosedSig();
    void resultsPressed();
    void save();
    void saveAs();
    void selectElementsPressed();
    void setPrefPressed();
    void showGridPressed();
    void showLabelsPressed();
    void showMeshPressed();
    void snapToGridPressed();
    void splitSegmentPressed();
    //void SPPressed();
    void superEdgePressed();
    void translatePressed();
    void undoPressed();
    void unravelNetPressed();
    void zoomInPressed();
    void zoomOutPressed();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void about();
    //void customizePressed();
    void documentWasModified();
    void elementsBeenHit(QVector<QPoint> hitEls);
    //void importNetwork();
    //void importMesh();
    void meshElsBeenHit(QVector<QPoint> hitMeshEls);
    //void meshPressed();
    //void newNetwork();
    //void openNetwork();
    //bool save();
    //bool saveAs();
    //void simulatePressed();
    void tabsContentChanged();
    void resultsTabsContentChanged();

public slots:
    void restoreCurs();
    void setCurs();
    void updateMainWindow();
    void mouseEnteredInDock();
    void mouseEnteredInResultsDock();
    void mouseLeftDock();
    void mouseLeftResultsDock();
    void dockClosed();
    void resultsDockClosed();
    void setFileName(QString theName);
    void showStatusBarMessage(QString theMessage);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    void loadNetwork();
    void loadNetWithoutLayout();
    void loadMesh();
    //bool maybeSave();
    void readSettings();
    //bool saveFile(const QString& fileName);
    //QString strippedName(const QString& fullFileName);

    QDockWidget* dock;
    QDockWidget* resultsDock;
    QTabWidget* tabs;
    QTabWidget* resultsTabs;
    ResultsGroupBox* resultsGroupBox;
    EditorArea* editorArea;

    //QString curFile;////

    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* operationMenu;
    QMenu* viewMenu;
    QMenu* helpMenu;

    QToolBar* fileToolBar;
    QToolBar* operationToolBar;
    QToolBar* viewToolBar;

    QActionGroup* operationGroup;

    QAction* aboutAct;
    QAction* addSegmentAct;
    QAction* BCAct;
    QAction* bendSegmentAct;
    QAction* blockNodesAct;
    QAction* caseInfoAct;
    QAction* customizeAct;
    QAction* defaultMeshAct;
    QAction* exitAct;
    QAction* homeViewAct;
    QAction* importBCAct;
    QAction* importMeshAct;
    QAction* importNetworkAct;
    //QAction* importPatientInfoAct;
    //QAction* importSPAct;
    QAction* infoAct;
    QAction* meshAct;
    QAction* newNetworkAct;
    QAction* openNetworkAct;
    QAction* preferencesAct;
    QAction* redoAct;
    QAction* removeSegmentAct;
    QAction* resultsAct;
    QAction* saveAct;
    QAction* saveAsAct;
    QAction* selectElementsAct;
    QAction* showGridAct;
    QAction* showLabelsAct;
    QAction* showMeshAct;
    QAction* simulateAct;
    QAction* snapToGridAct;
    //QAction* SPAct;
    QAction* splitSegmentAct;
    QAction* superEdgeAct;
    QAction* translateAct;
    QAction* undoAct;
    QAction* unravelNetAct;
    QAction* zoomInAct;
    QAction* zoomOutAct;

    QMap<QWidget*, QPoint> dataCollectorList;
};

#endif // MAINWINDOW_H
