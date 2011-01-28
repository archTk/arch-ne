////////////////////////////////////////////////////////////////////////////////
//
//   Copyright 2011 - ARCH vascular Netwrok Editor workgroup
//
//                    Orobix Srl -- www.orobix.com
//
//   Licensed under the archTk License, Version 1.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       https://github.com/archTk/arch-ne/licenses/LICENSE-1.0
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
    void loadGraphFromLayout();
    void loadGraphFromGraph();
    void loadMesh();
    void generateMesh(const QString& fileName);
    void graphHasBeenSaved(const QString& fileName);

    void meshHasBeenGenerated(const QString& fileName);

    void saveNetwork(const QString& fileName);

    void dataRequest(QPoint elementRequest);
    void dataConfirmed(QString cookie ,QString elementData);
    void dataConfirAndClose(QString cookie, QString elementData);

    void dockClosed();

private:
    void clear();
    int uniqueDataRequestKey();

    MainWindow* mainWindow;
    Workspace* workspace;
    EditorArea* editorArea;

    QMap<int, QPoint> requestMap;
    QMap<int, DataCollector*> dataCollectorList;
    int incrementalDataRequest;
};

#endif // APPCONTROLLER_H
