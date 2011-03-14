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

#ifndef GRAPHMESH_H
#define GRAPHMESH_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QPoint>

class GraphMesh : public QObject
{
    Q_OBJECT
public:
    explicit GraphMesh(QObject *parent = 0);

    bool dataInGraphMesh();
    void createNodeFromXML(QString attrNodeId, QString attrId, QString attrType, QString attrMeshnode1, QString attrMeshnode2,
                           QString attrMeshnode3, QString parameters);
    void createEdgeFromXML(QString attrEdgeId, QString attrId, QString attrType, QString attrMeshnode1, QString attrMeshnode2,
                           QString s1String, QString parameters);

    void applyDefaultMesh(QVector<int> edgesIds);

    void clear();

    QMap<int, QString> getNodesMType();
    QMap<int, QString> getNodesMParameters();
    QMap<int, QVector<int> > getNodesMMeshnodesId();
    QMap<int, int> getNodesMElementId();

    QMap<int, QVector<float> > getEdgesMS();
    QMap<int, QVector<QString> > getEdgesMTypes();
    QMap<int, QVector<QString> > getEdgesMParameters();
    QMap<int, QVector<QPoint> > getEdgesMMeshnodesId();
    QMap<int, QVector<int> > getEdgesMElementsId();

    QString getNodeMType(int nodeId);
    QString getNodeMParameters(int nodeId);
    QVector<int> getNodeMMeshnodesId(int nodeId);
    int getNodeMElementId(int nodeId);

    QVector<float> getEdgeMs(int edgeId);
    QVector<QString> getEdgeMTypes(int edgeId);
    QVector<QString> getEdgeMParameters(int edgeId);
    QVector<QPoint> getEdgeMMeshnodesId(int edgeId);
    QVector<int> getEdgeMElementsId(int edgeId);

    void setNodesMType(QMap<int, QString> theNodesMType);
    void setNodesMParameters(QMap<int, QString> theNodesMParameters);
    void setNodesMMeshnodesId(QMap<int, QVector<int> > theNodesMeshnodesId);
    void setNodesMElementId(QMap<int, int> theNodesMElementId);

    void setEdgesMS(QMap<int, QVector<float> > theEdgesMS);
    void setEdgesMTypes(QMap<int, QVector<QString> > theEdgesMTypes);
    void setEdgesMParameters(QMap<int, QVector<QString> > theEdgesMParameters);
    void setEdgesMMeshnodesId(QMap<int, QVector<QPoint> > theEdgesMMeshnodesId);
    void setEdgesMElementsId(QMap<int, QVector<int> > theEdgesMElementsId);

    void deleteNode(int nodeId);
    void deleteEdge(int edgeId);

signals:
    void setEdgeType(int edgeId, QString edgeType);

public slots:

private:
    int uniqueNodeMKey();
    int uniqueEdgeMKey();
    int incrementalNode;
    int incrementalEdge;

    QMap<int, QString> nodesMType;
    QMap<int, QString> nodesMParameters;
    QMap<int, QVector<int> > nodesMMeshnodesId; // There may be 3 meshnodes in a node (e.g. anastomosis).
    QMap<int, int> nodesMElementId;

    QMap<int, QVector<float> > edgesMS;         // Initial s-coord for the meshElements of the graph-edge.
    QMap<int, QVector<QString> > edgesMTypes;   // Type of the meshElements of the graph-edge.
    QMap<int, QVector<QString> > edgesMParameters;  // Parameters of the meshElements of the graph-edge.
    QMap<int, QVector<QPoint> > edgesMMeshnodesId;  // MeshnodeIds (first and second) of the mesh-elements of the graph-edge.
    QMap<int, QVector<int> > edgesMElementsId;      // MeshelementsIds of the graph-edge.
};

#endif // GRAPHMESH_H

