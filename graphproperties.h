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

#ifndef GRAPHPROPERTIES_H
#define GRAPHPROPERTIES_H

#include <QObject>
#include <QMap>

class GraphProperties : public QObject
{
    Q_OBJECT
public:
    explicit GraphProperties(QObject *parent = 0);

    void clear();
    void createNode(int nodeId, QString attrName, QString nodeProperties);

    void createEdge(int nedgeId, QString node1_idString, QString node2_idString);
    void insertEdgeFromXML(QString edgeId, QString attrName, QString edgeString);

    void deleteNode(int nodeId);
    void deleteEdge(int edgeId);

    void setCase(QString theCase);
    void setSuperedges(QString theSuperedges);
    void setTransformations(QString theTransformations);

    void setNodeName(int nodeId, QString theName);
    void setNodeProperties(int nodeId, QString attrType);

    void setEdgeName(int edgeId, QString theName);
    void setEdgeProperties(int edgeId, QString edgeProperties);

    void setNodesProperties(QMap<int, QString> theNodesProperties);

    void setEdgesProperties(QMap<int, QString> theEdgesProperties);

    QString getCase();
    QString getSuperedges();
    QString getTransformations();

    QString getNodeName(int nodeId);
    QString getNodeProperties(int nodeId);

    QString getEdgeName(int edgeId);
    QString getEdgeProperties(int edgeId);

    QMap<int, QString> getNodesProperties();

    QMap<int, QString> getEdgesProperties();

signals:

public slots:
    void deleteNodeFromGraph(int nodeId);
    void setEdgeType(int edgeId, QString theType);  // It is used for applyDefaultMesh(), thus setNodeType is not needed.

private:
    QMap<int, QString> nodesNames;
    QMap<int, QString> nodesProperties;
    QMap<int, QString> edgesNames;
    QMap<int, QString> edgesProperties;
    QString caseProp;
    QString superedges;
    QString transformations;
};

#endif // GRAPHPROPERTIES_H
