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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class QAction;
class QMenu;
class QFile;
class QActionGroup;
class QGridLayout;
class QString;

class EditorArea;
class DataCollector;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    EditorArea* getEditorArea();

    void initNewCase();

    void insertDataCollectorToDock(DataCollector* theDataCollector, QPoint  elementRequest);
    void removeAllDataCollectorFromDock();
    void removeDataCollectorFromDock();
    void setPageInTab(DataCollector* theDataCollector);

    void hideDock();
    void showDock();

    void setPythonPath(QString thePythonPath);
    void setScriptPath(QString theScriptPath);

    QString getPythonPath();
    QString getScriptPath();

    QAction* addSegmentAct;
    QAction* blockNodesAct;
    QAction* defaultMeshAct;
    QAction* homeViewAct;
    QAction* infoAct;
    QAction* meshAct;
    QAction* redoAct;
    QAction* removeSegmentAct;
    QAction* selectElementsAct;
    QAction* setPreferencesAct;
    QAction* showGridAct;
    QAction* showLabelsAct;
    QAction* snapToGridAct;
    QAction* splitSegmentAct;
    QAction* superEdgeAct;
    QAction* translateAct;
    QAction* undoAct;
    QAction* unravelNetAct;
    QAction* zoomInAct;
    QAction* zoomOutAct;

signals:
    void addSegmentPressed();
    void blockNodesPressed();
    void defaultMeshPressed();
    void homeViewPressed();
    void infoPressed();
    void loadGraphFromLayout();
    void loadGraphFromGraph();
    void meshToBeGenerated(const QString& fileName);
    void meshToBeLoaded();
    void redoPressed();
    void removeSegmentPressed();
    void saveNetwork(const QString& fileName);
    void selectElementsPressed();
    void showGridPressed();
    void showLabelsPressed();
    void snapToGridPressed();
    void splitSegmentPressed();
    void superEdgePressed();
    void translatePressed();
    void undoPressed();
    void unravelNetPressed();
    void zoomInPressed();
    void zoomOutPressed();
    void editingEl2Ws(QPoint editingElement);
    void dockClosedSig();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void about();
    //void appContrHasLoaded();
    void documentWasModified();
    void newNetwork();
    void openNetwork();
    void importNetwork();
    void importMesh();
    void meshPressed();
    bool save();
    bool saveAs();
    void setPrefPressed();
    void tabsContentChanged();
    void elementsBeenHit(QVector<QPoint> hitEls);

public slots:
    void restoreCurs();
    void setCurs();
    void updateMainWindow();
    void mouseEnteredInDock();
    void dockClosed();
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
    bool maybeSave();
    void readSettings();
    bool saveFile(const QString& fileName);
    void setCurrentFile(const QString& fileName);
    QString strippedName(const QString& fullFileName);
    void writeSettings();

    QDockWidget* dock;
    QTabWidget* tabs;
    EditorArea* editorArea;

    QString curFile;

    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* operationMenu;
    QMenu* viewMenu;
    QMenu* helpMenu;

    QToolBar* fileToolBar;
    QToolBar* operationToolBar;
    QToolBar* viewToolBar;

    QActionGroup* operationGroup;

    QAction* newNetworkAct;
    QAction* openNetworkAct;
    QAction* importNetworkAct;
    QAction* importMeshAct;
    QAction* saveAct;
    QAction* saveAsAct;
    QAction* exitAct;
    QAction* aboutAct;

    QMap<QWidget*, QPoint> dataCollectorList;

    QString pythonPath; // Path to python.
    QString scriptPath; // Path to MeshGenerator_Script.py. This file is inside pyNS.
    // These QStrings are here to use writeSettings and readSettings.
};

#endif // MAINWINDOW_H
