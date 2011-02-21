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

#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QPointF>
#include <QFile>
#include <QMap>
#include <QProcess>

class MainWindow;
class Workspace;
class EditorArea;
class DataCollector;

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject* parent = 0);

    void setMainWindow(MainWindow* mainWin);
    void setEditorArea(EditorArea* theEditorArea);
    void createConnections();

signals:
    void messageToBeDisplayed(QString theMessage);
    void restoreCurs();
    void setCurs();
    void updateSignal();

public slots:
    void bcPressed();
    void customizeGraph(const QString& fileName);
    void dataConfirAndClose(QString cookie, QString elementData);
    void dataConfirmed(QString cookie ,QString elementData);
    void dataRequest(QPoint elementRequest);
    void dockClosed();
    void errorFromExternal(QProcess::ProcessError);
    void generateMesh(const QString& fileName);
    void goMeshing(const QString& fileName);
    void graphHasBeenCustomized(const QString& fileName);
    void loadGraphFromLayout();
    void loadGraphFromGraph();
    void loadMesh();
    void meshHasBeenGenerated();
    void patientInfoPressed();
    void saveNetwork(const QString& fileName);
    void setPreferences();
    void showResults(QPoint elementRequest);
    void spPressed();

private:
    void clear();
    int uniqueDataRequestKey();
    void collectData(QPoint elementRequest, QString XMLString, QVector<QString> hiddenItems,
                     QVector<QString> readonlyItems, QString XMLSchema);
    void showWarningMessage(QString theMessage);

    MainWindow* mainWindow;
    Workspace* workspace;
    EditorArea* editorArea;

    QProcess* pyNS;

    QMap<int, QPoint> requestMap;
    QMap<int, DataCollector*> dataCollectorList;
    int incrementalDataRequest;

    QString meshOut;
};

#endif // APPCONTROLLER_H
