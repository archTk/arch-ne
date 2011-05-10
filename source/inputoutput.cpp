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

#include "inputoutput.h"
#include <QtXml/QDomNode>
#include <QtXml/QDomElement>
#include <QtXmlPatterns/QtXmlPatterns>
#include <QFileDialog>
#include <QMessageBox>
#include <QPointF>

#include "graph.h"
#include "graphlayout.h"
#include "graphproperties.h"
#include "graphmesh.h"
#include "networkproperties.h"

#include <iostream>
#include <QTextStream>
using namespace std;

#include <math.h>

QTextStream IOout(stdout);

InputOutput::InputOutput(QObject *parent) :
    QObject(parent)
{
}

bool InputOutput::loadGraphFromLayout(Graph *graph, GraphLayout *graphLayout, GraphProperties *graphProperties, NetworkProperties* networkProperties)
{
    QString layoutFileName = QFileDialog::getOpenFileName(0, tr("Open a network"),
                                                    "",
                                                    tr("XML files (*_layout.xml)"));

    if (layoutFileName.isNull()) {
        return false;
    }

    QFile layoutFile(layoutFileName);

    QString temp = layoutFileName;
    temp.remove("_layout.xml");
    QString graphFileName = temp + "_graph.xml";

    QFile graphFile(graphFileName);
    if (!graphFile.exists()) {
        QMessageBox msgBox;
        msgBox.setText("The corresponding \"_graph.xml\" file\n"
                       "is not present in the same folder.\n\n"
                       "I'm not able to load the network.");
        msgBox.exec();
        return false;
    } else {
        if (!graphFile.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                                 tr("Cannot read file %1:\n%2.")
                                 .arg(graphFileName)
                                 .arg(graphFile.errorString()));
            return false;
        }

        QTextStream graphIn(&graphFile);
        QDomDocument graphDoc("graph");
        graphDoc.setContent(graphIn.readAll());

        loadGraph(graphDoc, graph, graphProperties, networkProperties);
    }

    if (!layoutFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(layoutFileName)
                             .arg(layoutFile.errorString()));
        return false;
    }

    QTextStream layoutIn(&layoutFile);
    QDomDocument layoutDoc("layout");
    layoutDoc.setContent(layoutIn.readAll(), graphLayout);

    loadLayout(layoutDoc, graphLayout);

    QFileInfo fileInfo(temp);
    QString fName = fileInfo.fileName();
    QString wDir = fileInfo.path();

    emit curFNameAndWDir(fName, wDir);
    return true;
}

bool InputOutput::loadGraphFromGraph(Graph *graph, GraphLayout *graphLayout, GraphProperties *graphProperties, NetworkProperties *networkProperties)
{
    QString networkFileName = QFileDialog::getOpenFileName(0, tr("Import a network"),
                                                    "",
                                                    tr("XML files (*.xml)"));

    if (networkFileName.isNull()) {
        return false;
    }

    QFile networkFile(networkFileName);
    if (!networkFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(networkFileName)
                             .arg(networkFile.errorString()));
        return false;
    }

    QTextStream networkIn(&networkFile);
    QDomDocument networkDoc("network");
    networkDoc.setContent(networkIn.readAll());

    loadGraph(networkDoc, graph, graphProperties, networkProperties);

    QVector<int> nodesIds = graph->getNodesIds();
    int min = 100;
    int max = 1100;
    for (int i = 0; i < nodesIds.size(); i++) {
        int tempX = int(rand() / (RAND_MAX + 1.0) * (max + 1 - min) + min);
        int tempY = int(rand() / (RAND_MAX + 1.0) * (max + 1 - min) + min);
        float X = (float)tempX / 10.0 + 200.0;
        float Y = (float)tempY / 10.0 + 200.0;
        QString attrId;
        attrId.setNum(nodesIds[i]);
        QString attrX;
        attrX.setNum(X);
        QString attrY;
        attrY.setNum(Y);

        graphLayout->insertNodeFromXML(attrId, attrX, attrY);
    }

    QString idString;
    QString s("0.5");
    QString d("0.0");
    QVector<int> edgesIds = graph->getEdgesIds();
    for (int i = 0; i < edgesIds.size(); i++) {
        idString.setNum(edgesIds[i]);
        graphLayout->insertEdgeFromXML(idString, s, d);
    }

    QString temp(networkFileName);
    temp.remove(".xml");

    QFileInfo fileInfo(temp);
    QString fName = fileInfo.fileName();
    QString wDir = fileInfo.path();

    fName.remove("_graph");

    emit curFNameAndWDir(fName, wDir);

    return true;
}

/*QString InputOutput::loadDefaultBC()
{
    QFile defBCFile(":XMLschema/defaultBC.xml");

    if (!defBCFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                             tr("Cannot read defaultBC.xml"));
        return QString();
    }

    QTextStream BCIn(&defBCFile);
    QString BCXML = BCIn.readAll();
    return BCXML;
}*/

void InputOutput::loadMesh(GraphMesh *graphMesh)
{
    QString meshFileName = QFileDialog::getOpenFileName(0, tr("Import a mesh"),
                                                    "",
                                                    tr("XML files (*.xml)"));

    if (meshFileName.isNull()) {
        return;
    }

    QFile meshFile(meshFileName);
    if (!meshFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(meshFileName)
                             .arg(meshFile.errorString()));
        return;
    }

    QTextStream meshIn(&meshFile);

    QDomDocument meshDoc("mesh");
    meshDoc.setContent(meshIn.readAll());
    meshFile.close();

    populateGraphMeshDataStructure(meshDoc, graphMesh);
}

void InputOutput::loadMeshAfterGenerating(const QString &fileName, GraphMesh* graphMesh)
{
    IOout << "InputOutput::loadMeshAfterGenerating fileName=" << fileName << endl;

    QFile meshInFile(fileName);

    meshInFile.open( QIODevice::ReadWrite);

    QTextStream meshIn(&meshInFile);

    QDomDocument meshDoc("mesh");
    meshDoc.setContent(meshIn.readAll());

    meshInFile.close();

    populateGraphMeshDataStructure(meshDoc, graphMesh);

    /*QDomNodeList networkMeshList = meshDoc.elementsByTagName("NetworkMesh");
    QDomNode networkMesh = networkMeshList.at(0);
    QDomElement meshElements = networkMesh.firstChildElement("elements");

    QString meshElementsString;
    IOout << meshElementsString << endl;
    QTextStream meshElementsStream(&meshElementsString);
    meshElements.save(meshElementsStream, 4);

    QDomElement meshElement = meshElements.firstChildElement("element");

    while (!meshElement.isNull()) {
        QString attrId = meshElement.attribute("id");
        QString attrType = meshElement.attribute("type");
        QString attrMeshnode1 = meshElement.attribute("meshnode1_id");
        QString attrMeshnode2 = meshElement.attribute("meshnode2_id");
        QString attrMeshnode3 = meshElement.attribute("mehnode3_id");
        IOout << "listaAttr= " << attrId << " " << attrType << " " << attrMeshnode1 << " " << attrMeshnode2 << " " << attrMeshnode3 << endl;
        QString attrEdgeId;
        attrEdgeId.clear();

        QDomElement parameters = meshElement.firstChildElement("parameters");
        QString parametersString;
        QTextStream parametersStream(&parametersString);
        parameters.save(parametersStream, 4);

        QDomElement pcoord = meshElement.firstChildElement("pcoord");
        attrEdgeId = pcoord.attribute("edgeId");
        if (!attrEdgeId.isNull()) {
            QDomElement s1 = pcoord.firstChildElement("s1");
            QString s1ValueString;
            QTextStream s1ValueStream(&s1ValueString);
            s1.save(s1ValueStream, 4);
            s1ValueString.remove("<s1>");
            s1ValueString.remove("</s1>");

            graphMesh->createEdgeFromXML(attrEdgeId, attrId, attrType, attrMeshnode1, attrMeshnode2, s1ValueString, parametersString);
        } else {
            QString attrNodeId = pcoord.attribute("nodeId");
            graphMesh->createNodeFromXML(attrNodeId, attrId, attrType, attrMeshnode1, attrMeshnode2, attrMeshnode3, parametersString);
        }
        meshElement = meshElement.nextSiblingElement("element");
    }

    meshInFile.close();*/
}


void InputOutput::populateGraphMeshDataStructure(QDomDocument theMeshDoc, GraphMesh *graphMesh)
{
    QDomNodeList networkMeshList = theMeshDoc.elementsByTagName("NetworkMesh");
    QDomNode networkMesh = networkMeshList.at(0);
    QDomElement meshElements = networkMesh.firstChildElement("elements");

    QString meshElementsString;
    QTextStream meshElementsStream(&meshElementsString);
    meshElements.save(meshElementsStream, 4);

    QDomElement meshElement = meshElements.firstChildElement("element");

    while (!meshElement.isNull()) {
        QString attrId = meshElement.attribute("id");
        QString attrType = meshElement.attribute("type");
        QString attrMeshnode1 = meshElement.attribute("meshnode1_id");
        QString attrMeshnode2 = meshElement.attribute("meshnode2_id");
        QString attrMeshnode3 = meshElement.attribute("meshnode3_id");
        QString attrEdgeId;
        attrEdgeId.clear();

        QDomElement parameters = meshElement.firstChildElement("parameters");
        QString parametersString;
        QTextStream parametersStream(&parametersString);
        parameters.save(parametersStream, 4);

        QDomElement pcoord = meshElement.firstChildElement("pcoord");
        attrEdgeId = pcoord.attribute("edgeId");
        if (!attrEdgeId.isNull()) {
            QDomElement s1 = pcoord.firstChildElement("s1");
            QString s1ValueString;
            QTextStream s1ValueStream(&s1ValueString);
            s1.save(s1ValueStream, 4);
            s1ValueString.remove("<s1>");
            s1ValueString.remove("</s1>");

            graphMesh->createEdgeFromXML(attrEdgeId, attrId, attrType, attrMeshnode1, attrMeshnode2, s1ValueString, parametersString);
        } else {
            QString attrNodeId = pcoord.attribute("nodeId");
            graphMesh->createNodeFromXML(attrNodeId, attrId, attrType, attrMeshnode1, attrMeshnode2, attrMeshnode3, parametersString);
        }
        meshElement = meshElement.nextSiblingElement("element");
    }
}

void InputOutput::importBC(NetworkProperties *networkProperties)
{
    QString BCFileName = QFileDialog::getOpenFileName(0, tr("Import Boundary Conditions"),
                                                    "",
                                                    tr("XML files (*.xml)"));

    if (BCFileName.isNull()) {
        return;
    }

    QFile BCFile(BCFileName);
    if (!BCFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(BCFileName)
                             .arg(BCFile.errorString()));
        return;
    }

    QTextStream BCIn(&BCFile);
    QString BCXML(BCIn.readAll());

    networkProperties->setBCXML(BCXML);
}

/*void InputOutput::importPatientInfo(NetworkProperties *networkProperties)
{
    QString PatientInfoFileName = QFileDialog::getOpenFileName(0, tr("Import Patient Info"),
                                                    "",
                                                    tr("XML files (*.xml)"));

    if (PatientInfoFileName.isNull()) {
        return;
    }

    QFile PatientInfoFile(PatientInfoFileName);
    if (!PatientInfoFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(PatientInfoFileName)
                             .arg(PatientInfoFile.errorString()));
        return;
    }

    QTextStream PatientInfoIn(&PatientInfoFile);
    QString PatientInfoXML(PatientInfoIn.readAll());

    networkProperties->setPatientInfoXML(PatientInfoXML);
}*/

/*void InputOutput::importSP(NetworkProperties *networkProperties)
{
    QString SPFileName = QFileDialog::getOpenFileName(0, tr("Import Simulation Parameters"),
                                                    "",
                                                    tr("XML files (*.xml)"));

    if (SPFileName.isNull()) {
        return;
    }

    QFile SPFile(SPFileName);
    if (!SPFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(SPFileName)
                             .arg(SPFile.errorString()));
        return;
    }

    QTextStream SPIn(&SPFile);
    QString SPXML(SPIn.readAll());

    networkProperties->setSPXML(SPXML);
}*/

void InputOutput::saveGraph(const QString& fName, const QString& wDir, GraphProperties* graphProperties, NetworkProperties* networkProperties, QVector<int> nodes, QVector<int> edges)
{
    IOout << "IO::saveGraph" << endl;
    QString graphName(fName);
    graphName.prepend(wDir + "/");
    graphName.append("_graph.xml");

    QString networkResult;

    QString graphId;

    graphId.setNum(1);
    QString graphVersion;
    graphVersion = "3.0";
    QString xmlns("\"\nxmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                  "xsi:noNamespaceSchemaLocation=\"vascular_network_v3.2.xsd\">\n");

    networkResult.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                  "<!--Vascular Network Model: graph description-->\n"
                  "<NetworkGraph id=\"");
    networkResult.append(graphId);
    networkResult.append("\" version=\"");
    networkResult.append(graphVersion);
    networkResult.append(xmlns);

    networkResult.append(networkProperties->getCaseInfoXML());

    networkResult.append("<nodes>\n");
    for (int i = 0; i < nodes.size(); i++) {
        QString nodeString = graphProperties->getNodeProperties(nodes[i]);
        networkResult += nodeString;
    }
    networkResult.append("</nodes>\n");

    networkResult.append(graphProperties->getSuperedges());

    networkResult.append("<edges>\n");
    for (int i = 0; i < edges.size(); i++) {
        QString edgeString = graphProperties->getEdgeProperties(edges[i]);

        if (!edgeString.isNull()) {
            networkResult += edgeString;
        }
    }

    networkResult.append("</edges>\n");

    networkResult.append(graphProperties->getTransformations());

    networkResult.append("</NetworkGraph>");

    QFile networkFile(graphName);

    networkFile.open( QIODevice::WriteOnly );

    QTextStream graphOut(&networkFile);

    graphOut << networkResult;

    networkFile.close();

    emit graphSaved();
}

bool InputOutput::saveNetwork(const QString& fName, const QString& wDir, GraphLayout* graphLayout, GraphProperties* graphProperties,
                              NetworkProperties* networkProperties, QVector<int> nodes, QVector<int> edges)
{
    QString graphName(wDir + "/" + fName);
    QString layout(wDir + "/" + fName);
    graphName.append("_graph.xml");
    layout.append("_layout.xml");

    QFile test(graphName);
    if (test.exists()) {
        QMessageBox::StandardButton ret;
                ret = QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                                           tr("The file already exists.\n"
                                              "Are you sure you want to replace it?"),
                                           QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) {
            return false;
        }
    }

    QFile networkFile(graphName);
    if (!networkFile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(graphName)
                             .arg(networkFile.errorString()));
        return false;
    }

//    if (networkFile.exists()) {
//        IOout << "IO::saveNetwork ocio che esite già!" << endl;

//        QMessageBox::StandardButton ret;
//        ret = QMessageBox::warning(0, tr("ARCHNetworkEditor"),
//                                   tr("Are you sure you want to replace the file?"),
//                                   QMessageBox::Yes | QMessageBox::No);
//        if (ret == QMessageBox::No) {
//            return false;
//        }
//    }

    QFile layoutFile(layout);
    if (!layoutFile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(layout)
                             .arg(layoutFile.errorString()));
        return false;
    }

    QString networkResult;

    QString graphId;

    graphId.setNum(1);
    QString graphVersion;
    graphVersion = "3.0";
    QString xmlns("\"\nxmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                  "xsi:noNamespaceSchemaLocation=\"vascular_network_v3.2.xsd\">\n");

    networkResult.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                  "<!--Vascular Network Model: graph description-->\n"
                  "<NetworkGraph id=\"");
    networkResult.append(graphId);
    networkResult.append("\" version=\"");
    networkResult.append(graphVersion);
    networkResult.append(xmlns);

    networkResult.append(networkProperties->getCaseInfoXML());

    networkResult.append("<nodes>\n");
    for (int i = 0; i < nodes.size(); i++) {
        QString nodeString = graphProperties->getNodeProperties(nodes[i]);
        networkResult += nodeString;
    }
    networkResult.append("</nodes>\n");

    networkResult.append(graphProperties->getSuperedges());

    networkResult.append("<edges>\n");

    for (int i = 0; i < edges.size(); i++) {
        QString edgeString = graphProperties->getEdgeProperties(edges[i]);

        if (!edgeString.isNull()) {
            networkResult += edgeString;
        }
    }

    networkResult.append("</edges>\n");

    networkResult.append(graphProperties->getTransformations());

    networkResult.append("</NetworkGraph>");

    ///////

    QString layoutResult;
    QString layoutVersion;
    layoutVersion = "69";

    layoutResult.append("<!--Vascular Network Model: graph layout description-->\n"
                        "<NetworkGraphLayout id=\"");
    layoutResult.append(graphId);
    layoutResult.append("\" version =\"");
    layoutResult.append(layoutVersion);
    layoutResult.append(xmlns);

    layoutResult.append(networkProperties->getCaseInfoXML());

    layoutResult.append("<nodes_layout>\n");
    for (int i = 0; i < nodes.size(); i++) {
        QPointF nodePosition = graphLayout->getNodePosition(nodes[i]);
        QString nodeIdString;
        nodeIdString.setNum(nodes[i]);

        layoutResult.append("<node_layout id=\"");
        layoutResult.append(nodeIdString);
        layoutResult.append("\" x=\"");
        QString xString;
        xString.setNum(nodePosition.x());
        layoutResult.append(xString);
        layoutResult.append("\" y=\"");
        QString yString;
        yString.setNum(nodePosition.y());
        layoutResult.append(yString);
        layoutResult.append("\">");

        layoutResult.append("\n</node_layout>\n");
    }
    layoutResult.append("</nodes_layout>\n");

    layoutResult.append("<edges_layout>\n");
    for (int i = 0; i < edges.size(); i++) {
        QPointF edgeLayoutPar = graphLayout->getEdgeLayoutParameters(edges[i]);
        QString edgeIdString;

        edgeIdString.setNum(edges[i]);
        layoutResult.append("<edge_layout id=\"");
        layoutResult.append(edgeIdString);
        layoutResult.append("\" s=\"");
        QString sString;
        sString.setNum(edgeLayoutPar.x());
        layoutResult.append(sString);
        layoutResult.append("\" d=\"");
        QString dString;
        dString.setNum(edgeLayoutPar.y());
        layoutResult.append(dString);
        layoutResult.append("\">");
        layoutResult.append("\n</edge_layout>\n");
    }
    layoutResult.append("</edges_layout>\n");

    layoutResult.append("</NetworkGraphLayout>\n");

    ///////

    QTextStream networkOut(&networkFile);
    networkOut << networkResult;

    QTextStream layoutOut(&layoutFile);
    layoutOut << layoutResult;

    //QFileInfo fileInfo(fileName);
    //QString wDir = fileInfo.path();

    //emit curFNameAndWDir(fileName,wDir);
    return true;
}

void InputOutput::saveBC(const QString &fName, const QString &wDir, QString BCXML)
{
    QString BC = wDir + "/BC_" + fName + "_graph.xml";

    QFile BCFile(BC);
    if (!BCFile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(0, tr("ARCHNetworkEditor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(BC)
                             .arg(BCFile.errorString()));
        return;
    }

    QTextStream BCOut(&BCFile);
    BCOut << BCXML;
}

void InputOutput::loadGraph(QDomDocument theDomDoc, Graph *graph, GraphProperties *graphProperties, NetworkProperties* networkProperties)
{
    QDomNodeList caseInfoList = theDomDoc.elementsByTagName("case");
    QDomNodeList nodesList = theDomDoc.elementsByTagName("nodes");
    QDomNodeList superedgesList = theDomDoc.elementsByTagName("superedges");
    QDomNodeList edgesList = theDomDoc.elementsByTagName("edges");

    QDomNodeList transformationsList = theDomDoc.elementsByTagName("transformations");

    QDomNode caseInfo = caseInfoList.at(0);
    QDomNode transformations = transformationsList.at(0);
    QDomNode nodes = nodesList.at(0);
    QDomNode superedges = superedgesList.at(0);
    QDomNode edges = edgesList.at(0);

    QDomElement node = nodes.firstChildElement("node");
    QDomElement edge = edges.firstChildElement("edge");

    QString caseString;
    if (!caseInfo.isNull()) {
        QTextStream caseInfoStream(&caseString);
        caseInfo.save(caseInfoStream, 4);
    }

    QString superedgesString;
    if (!superedges.isNull()) {
        QTextStream superedgesStream(&superedgesString);
        superedges.save(superedgesStream, 4);
    }

    QString transformationsString;
    if (!transformations.isNull()) {
        QTextStream transformationsStream(&transformationsString);
        transformations.save(transformationsStream, 4);
    }

    networkProperties->setCaseInfoXML(caseString);
    graphProperties->setSuperedges(superedgesString);
    graphProperties->setTransformations(transformationsString);

    while (!node.isNull()) {
        QString attrId = node.attribute("id");
        QString attrName = node.attribute("name");

        graph->insertNodeFromXML(attrId);

        QString nodeString;
        QTextStream nodeStream(&nodeString);
        node.save(nodeStream, 4);

        int nodeId = attrId.toInt();
        graphProperties->createNode(nodeId, attrName, nodeString);

        node = node.nextSiblingElement("node");
    }

    while (!edge.isNull()) {
        QString attrId = edge.attribute("id");
        QString node1_idString = edge.attribute("node1_id");
        QString node2_idString = edge.attribute("node2_id");
        QString attrName = edge.attribute("name");

        graph->insertEdgeFromXML(attrId, node1_idString, node2_idString);

        QString edgeString;
        QTextStream edgeStream(&edgeString);
        edge.save(edgeStream, 4);

        graphProperties->insertEdgeFromXML(attrId, attrName, edgeString);

        edge = edge.nextSiblingElement("edge");
    }
}

void InputOutput::loadLayout(QDomDocument theDomDoc, GraphLayout *graphLayout)
{
    QDomNodeList nodesLayoutList = theDomDoc.elementsByTagName("nodes_layout");
    QDomNodeList edgesLayoutList = theDomDoc.elementsByTagName("edges_layout");

    QDomNode nodesLayout = nodesLayoutList.at(0);
    QDomNode edgesLayout = edgesLayoutList.at(0);

    QDomElement nodeLayout = nodesLayout.firstChildElement("node_layout");
    QDomElement edgeLayout = edgesLayout.firstChildElement("edge_layout");

    while (!nodeLayout.isNull()) {
        QString attrId = nodeLayout.attribute("id");
        QString attrX = nodeLayout.attribute("x");
        QString attrY = nodeLayout.attribute("y");
        graphLayout->insertNodeFromXML(attrId, attrX, attrY);

        nodeLayout = nodeLayout.nextSiblingElement("node_layout");
    }

    while (!edgeLayout.isNull()) {
        QString attrId = edgeLayout.attribute("id");
        QString attrS = edgeLayout.attribute("s");
        QString attrD = edgeLayout.attribute("d");
        graphLayout->insertEdgeFromXML(attrId, attrS, attrD);

        edgeLayout = edgeLayout.nextSiblingElement("edge_layout");
    }
}

QMap< int, QMap<QString, QVector<QPointF> > > InputOutput::loadResData(const QString resFileName)
{
    QFile resInFile(resFileName);

    resInFile.open(QIODevice::ReadOnly);

    QTextStream resStreamIn(&resInFile);

    QDomDocument resDoc("results");
    resDoc.setContent((resStreamIn.readAll()));

    QMap< int, QMap<QString, QVector<QPointF> > > resultsMap;
    QMap<QString, QVector<QPointF> > tempMap;
    QVector<QPointF> tempVector;

    resultsMap.clear();

    QDomNodeList resultsList = resDoc.elementsByTagName("Solutions");
    QDomNode results = resultsList.at(0);
    QDomElement resultsElements = results.firstChildElement("edges");

    QDomElement resultsElement = resultsElements.firstChildElement("edge");

    while (!resultsElement.isNull()) {
        QString attrId = resultsElement.attribute("id");
        //IOout << "          edgeId=" << attrId << endl;
        int edgeId = attrId.toInt();

        QDomElement solution = resultsElement.firstChildElement("solution");
        QDomNode solEl = solution.firstChild();

        while (!solEl.isNull()) {
            QString solElString = solEl.nodeName();
            //IOout << "solEl= " << solElString << endl;

            QDomNode solElChild = solEl.firstChild();

            if (solElChild.nodeName() == "value") {
                tempVector.clear();
                QDomElement valueEl = solEl.firstChildElement("value");
                while (!valueEl.isNull()) {
                    QString s = valueEl.attribute("s");

                    QDomElement scalarEl = valueEl.firstChildElement("scalar");
                    QString scalarString = scalarEl.text();

                    float scalarValue;
                    scalarValue = scalarString.toFloat();

                    QPointF temp;
                    temp.setX(s.toFloat());
                    temp.setY(scalarValue);

                    tempVector << temp;
                    //IOout << "s=" << temp.x();
                    //IOout << " scalar=" << temp.y() << endl;

                    valueEl = valueEl.nextSiblingElement("value");
                }
            } else if (solElChild.nodeName() == "scalar") {
                tempVector.clear();
                QDomElement scalarEl = solEl.firstChildElement("scalar");
                float sValue;
                sValue = 0.5;

                QString scalarString = scalarEl.text();

                float scalarValue;
                scalarValue = scalarString.toFloat();

                QPointF temp;
                temp.setX(sValue);
                temp.setY(scalarValue);

                //IOout << "s= " << temp.x();
                //IOout << " scalar=" << temp.y() << endl;

                tempVector << temp;
            }

            tempMap.insert(solElString, tempVector);

            solEl = solEl.nextSibling();
        }

        resultsMap.insert(edgeId, tempMap);

        resultsElement = resultsElement.nextSiblingElement("edge");
    }

    return resultsMap;
}
