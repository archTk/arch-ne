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

#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H

#include <QObject>
#include <QtXml/QDomDocument>

class Graph;
class GraphLayout;
class GraphProperties;
class GraphMesh;
class NetworkProperties;
class QFile;

class InputOutput : public QObject
{
    Q_OBJECT
public:
    explicit InputOutput(QObject* parent = 0);

    bool loadGraphFromLayout(Graph* graph, GraphLayout* graphLayout, GraphProperties* graphProperties, NetworkProperties* networkProperties);
    bool loadGraphFromGraph(Graph* graph, GraphLayout* graphLayout, GraphProperties* graphProperties, NetworkProperties* networkProperties);
    //QString loadDefaultBC();

    void loadMesh(GraphMesh* graphMesh);

    void saveGraph(const QString& fileName, const QString& wDir, GraphProperties* graphProperties, NetworkProperties* networkProperties, QVector<int> nodes, QVector<int> edges);
    bool saveNetwork(const QString& fileName, GraphLayout* graphLayout, GraphProperties* graphProperties, NetworkProperties* networkProperties, QVector<int> nodes, QVector<int> edges);

    void saveBC(const QString& fName, const QString& wDir, QString BCXML);

    void loadMeshAfterGenerating(const QString& fileName, GraphMesh* graphMesh);

    void importBC(NetworkProperties* networkProperties);
    //void importPatientInfo(NetworkProperties* networkProperties);
    //void importSP(NetworkProperties* networkProperties);

signals:
    void curFNameAndWDir(QString theFName, QString theWDir);
    void graphSaved();
    void pyNSCompleted(QString fileName);
    void meshFileReady(QString fileName);

public slots:

private:
    void loadGraph(QDomDocument theDomDoc, Graph* graph, GraphProperties* graphProperties, NetworkProperties* networkProperties);
    void loadLayout(QDomDocument theDomDoc, GraphLayout* graphLayout);
};

#endif // INPUTOUTPUT_H
