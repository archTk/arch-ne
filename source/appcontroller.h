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

#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QCloseEvent>
#include <QObject>
#include <QPointF>
#include <QFile>
#include <QMap>
#include <QProcess>

class InfoDialog;
class MainWindow;
class Workspace;
class EditorArea;
class DataCollector;
class ResultsView;

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject* parent = 0);

    void setMainWindow(MainWindow* mainWin);
    void setEditorArea(EditorArea* theEditorArea);
    void createConnections();

signals:
    void currentFile(QString theCurrentFile);
    void messageToBeDisplayed(QString theMessage);
    void restoreCurs();
    void setCurs();
    void updateSignal();

public slots:
    void abortSimulation();
    void BCPressed();
    void caseInfoPressed();
    void customizeGraph();
    void closeEvent(QCloseEvent* event);
    void closeResultsView(QString cookie);
    void dataConfirAndClose(QString cookie, QString elementData);
    void dataConfirmed(QString cookie ,QString elementData);
    void dataRequest(QPoint elementRequest);
    void dockClosed();
    void errorFromExternal(QProcess::ProcessError);
    void generateMesh();
    void goMeshing();
    void goCustomizing();
    void graphHasBeenCustomized();
    void importBC();
    void importMesh();
    void importNetwork();
    void initNewCase();
    void meshHasBeenGenerated();
    void newNetwork();
    void openNetwork();
    void resultsDockClosed();
    bool save();
    bool saveAs();
    void setFNameAndWDir(QString theFName, QString theWDir);
    void setPreferences();
    void setResult2Display(int theResult);
    void showResults(QPoint elementRequest);
    void showResultsDock();
    void simulateGraph();
    void simulationHasBeenPerformed();

private slots:

private:
    void clear();
    void collectData(QPoint elementRequest, QString XMLString, QVector<QString> hiddenItems,
                     QVector<QString> readonlyItems, QString XMLSchema);
    void loadGraphFromLayout();
    void loadGraphFromGraph();
    bool maybeSave();
    bool saveNetwork();
    void showMessage(QString theTitle, QString theMessage);
    int uniqueDataRequestKey();
    int uniqueResultsRequestKey();

    MainWindow* mainWindow;
    Workspace* workspace;
    EditorArea* editorArea;
    InfoDialog* infoDialog;

    QProcess* pyNS;

    QMap<int, QPoint> requestMap;
    QMap<int, QPoint> resultsMap;
    QMap<int, DataCollector*> dataCollectorList;
    QMap<int, ResultsView*> resultsViewList;
    int incrementalDataRequest;
    int incrementalResultsRequest;

    QString meshOut;
    //QString imagesDir;

    QString fName;  // File name.
    QString wDir;   // Working directory.

    bool checkPaths(QString pythonPath, QString pyNSPath);

    int resToBeDisplayed;
};

#endif // APPCONTROLLER_H
