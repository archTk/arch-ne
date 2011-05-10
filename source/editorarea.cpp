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

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QMap>
#include <QKeyEvent>
#include <QLabel>

#include <iostream>
#include <QTextStream>
using namespace std;

#include "editorarea.h"
#include "workspace.h"

QTextStream qcout(stdout);

EditorArea::EditorArea(QWidget *parent)
        : QWidget(parent)
{
    setPalette(QPalette(QColor(250, 250, 250)));
    setAutoFillBackground(true);

    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);

    edgeDiscretization = 31;    // It defines the divisions on the "Bezier" to position the mesh_element.

    meshElToBeHigh = -1;

    resToBeDisplayed = 0;
}

void EditorArea::setWorkspace(Workspace* theWorkspace)
{
    workspace = theWorkspace;
}

Workspace* EditorArea::getWorkspace()
{
    return workspace;
}

void EditorArea::setResToBeDisplayed(int theResult)
{
    resToBeDisplayed = theResult;
    updateRender();
}

void EditorArea::paintEvent(QPaintEvent*)
{
    QMap<QString, int> supportNodes(workspace->getSupportNodes());
    QMap<QString, int> supportEdges(workspace->getSupportEdges());
    QVector<int> selectedNodes(workspace->getSelectedNodes());
    QVector<int> selectedEdges(workspace->getSelectedEdges());
    QVector<int> supportNodes4selecting(workspace->getSupportNodes4selecting());
    QVector<int> supportEdges4selecting(workspace->getSupportEdges4selecting());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    qreal penWidth = size2screen(5);
    qreal penWidth4selecting = size2screen(16);
    qreal penWidth4selected = size2screen(12);
    qreal penWidth4area = size2screen(1);
    qreal penWidth4labels = size2screen(0.2);

    if (!selectedNodes.isEmpty()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::green);
        paintSelectedNodes(painter);
    }

    /*if (supportNodes.contains("tooCloseNode")) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::magenta);
        paintTooCloseNodes(painter);
    }*/

    if (supportNodes.contains("highlightingNode")) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::yellow);
        paintHighlightingNode(painter);
    }

    if (supportEdges.contains("highlightingEdge")) {
        painter.setPen(QPen(Qt::yellow, penWidth4selecting, Qt::SolidLine,
                           Qt::RoundCap, Qt::RoundJoin));
        paintHighlightingEdge(painter);
    }

    if (workspace->getGridStatus()) {
        paintGrid(painter);
    }

    if (workspace->getShowSelectingArea()) {
        painter.setPen(QPen(Qt::gray, penWidth4area, Qt::SolidLine,
                            Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::NoBrush);
        paintSelectingArea(painter);
    }

    if (!selectedEdges.isEmpty()) {
        painter.setPen(QPen(Qt::green, penWidth4selected, Qt::SolidLine,
                            Qt::RoundCap, Qt::RoundJoin));
        paintSelectedEdges(painter);
    }

    if(!supportNodes4selecting.isEmpty()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::green);
        paintSupportNodes4selecting(painter);
    }

    if(!supportEdges4selecting.isEmpty()) {
        painter.setPen(QPen(Qt::green, penWidth4selected, Qt::SolidLine,
                            Qt::RoundCap, Qt::RoundJoin));
        paintSupportEdges4selecting(painter);
    }

    hitElements.clear();
    hitElements.append(QPoint(-1, -1));
    hitMeshEls.clear();
    hitMeshEls.append(QPoint(-1, -1));

    painter.setPen(QPen(Qt::blue, penWidth, Qt::SolidLine,
                        Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    paintEdges(painter);    // paintEdges before paintNodes to give priority to nodes in  -emit elementBeenHit-.
                            // paintEdges also populates "QMap<int, QVector<float> > sOnBezier".

    //painter.setPen(Qt::NoPen);    -- Done in paintNodes.
    painter.setBrush(Qt::red);
    paintNodes(painter);

    if (workspace->dataInGraphMesh()) {
        paintMeshEls(painter);
    }

    if (workspace->getShowLabelsStatus()) {
        painter.setPen(QPen(Qt::red, penWidth4labels, Qt::SolidLine,
                           Qt::RoundCap, Qt::RoundJoin));
        paintLabels(painter);
    }

    if (resToBeDisplayed !=0) {
        paintResults(painter);
    }

    if (!hitElements.isEmpty()) {
        if (!hitElements[0].x() != -1) {
            emit elementsBeenHit(hitElements);
        }
    }

    if (!hitMeshEls.isEmpty()) {
        if (!hitMeshEls[0].x() != -1) {
            emit meshElsBeenHit(hitMeshEls);
        }
    }

    if (meshElToBeHigh != -1) {
        paintMeshElToBeHigh(painter);
    }
}

void EditorArea::zoomIn()
{
    float zoomFactor = workspace->getZoomFactor();
    QPointF screenOrigin = workspace->getScreenOrigin();

    if (zoomFactor == 400.0) {
        return;
    }

    float s0 = zoomFactor / 100.0;
    zoomFactor = zoomFactor + 25;
    float s1 = zoomFactor / 100.0;

    screenOrigin.setX(screenOrigin.x() + width() / 2.0 * (1.0 / s0 - 1.0 / s1));
    screenOrigin.setY(screenOrigin.y() + height() / 2.0 * (1.0 / s0 - 1.0 / s1));

    emit screenOriginChanged(screenOrigin);
    emit zoomFactorChanged(zoomFactor);
    updateRender();
}

void EditorArea::zoomOut()
{
    float zoomFactor = workspace->getZoomFactor();
    QPointF screenOrigin = workspace->getScreenOrigin();

    if (zoomFactor == 25.0) {
        return;
    }

    float s0 = zoomFactor / 100.0;
    zoomFactor = zoomFactor - 25.0;
    float s1 = zoomFactor / 100.0;

    screenOrigin.setX(screenOrigin.x() + width() / 2.0 * (1.0 / s0 - 1.0 / s1));
    screenOrigin.setY(screenOrigin.y() + height() / 2.0 * (1.0 / s0 - 1.0 / s1));

    emit zoomFactorChanged(zoomFactor);
    emit screenOriginChanged(screenOrigin);
    updateRender();
}

void EditorArea::toBeCleared()
{
    clear();
}

void EditorArea::updateRender()
{
    emit updateMainWindow();
    update();
}

void EditorArea::clear()
{
    meshElsPos.clear();
    meshElsType.clear();
    meshId2GraphId.clear();
    meshElToBeHigh = -1;
    //workspace->clear();
}

void EditorArea::homeView()
{
    float zoomFactor = 100.0;
    QPointF screenOrigin(0, 0);

    emit zoomFactorChanged(zoomFactor);
    emit screenOriginChanged(screenOrigin);
    updateRender();
}


void EditorArea::mousePressEvent(QMouseEvent* event)
{
    int selectedTool = workspace->getSelectedTool();

    if (selectedTool == 6) {
        screenOriginStartPos = event->pos();
    }

    emit mouseBeenPressed(screen2graph(event->pos()));
}

void EditorArea::mouseMoveEvent(QMouseEvent* event)
{
    int selectedTool = workspace->getSelectedTool();
    QPointF screenOrigin = workspace->getScreenOrigin();
    float zoomFactor = workspace->getZoomFactor();
    bool mouseButtonPressed = workspace->getMouseButtonPressed();

    QPoint validPos = event->pos();
    mouseCurrentPos = validPos;

    if (validPos.x() <= 0.0) {
         validPos.setX(1.0);
    }
    if (validPos.x() >= width()){
         validPos.setX((float)width() - 1.0);
    }
    if (validPos.y() <= 0.0) {
         validPos.setY(1.0);
    }
    if (validPos.y() >= height()) {
         validPos.setY((float)height() - 1.0);
    }

    if (selectedTool == 6 && mouseButtonPressed) {
        screenOriginEndPos = event->pos();
        screenOrigin = screenOrigin + (screenOriginEndPos - screenOriginStartPos) * 100 / zoomFactor;
        screenOriginStartPos = screenOriginEndPos;
        emit screenOriginChanged(screenOrigin);
        updateRender();
    } else {
        emit mouseMoved(screen2graph(validPos));
    }
}

void EditorArea::mouseReleaseEvent(QMouseEvent* event)
{
    int selectedTool = workspace->getSelectedTool();

    QPoint validPos = event->pos();
    if (validPos.x() <= 0.0){
         validPos.setX(1.0);
    }
    if (validPos.x() >= width()){
         validPos.setX((float)width() - 1.0);
    }
    if (validPos.y() <= 0.0) {
         validPos.setY(1.0);
    }
    if (validPos.y() >= height()) {
         validPos.setY((float)height() - 1.0);
    }

    if (selectedTool == 6) {
        screenOriginEndPos.setX(0.0);
        screenOriginEndPos.setY(0.0);
        screenOriginStartPos.setX(0.0);
        screenOriginStartPos.setY(0.0);
        emit updateHistory();
    }

    emit mouseReleased(screen2graph(validPos));
}

void EditorArea::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift) {
        emit shiftPressedChanged(true);
    } else if (event->key() == Qt::Key_Cancel || event->key() == 16777219
               || event->key() == Qt::Key_Delete) {
        emit Key_Cancel_Pressed();
    } else if (event->modifiers() == Qt::CTRL && event->key() == Qt::Key_A) {
        emit Key_CTRL_A_Pressed();
    } else {
        event->ignore();
    }
}

void EditorArea::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift) {
        emit shiftPressedChanged(false);
    } else {
        event->ignore();
    }
}

void EditorArea::paintEdges(QPainter &painter)
{
    bool firstTime = true;

    QVector<int> edgesIds = workspace->getEdgesIds();

    for (int i=0; i < edgesIds.size(); i++) {
        QPoint nodesOfEdge = workspace->getNodesOfEdge(edgesIds[i]);
        QPointF screenXYFirst = graph2screen(workspace->getNodePosition(nodesOfEdge.x()));
        QPointF screenXYSecond = graph2screen(workspace->getNodePosition(nodesOfEdge.y()));
        QPointF screenMidPoint = graph2screen(workspace->getThirdPointPos(edgesIds[i]));

        QPainterPath path;
        path.moveTo(screenXYFirst);
        path.quadTo(screenMidPoint, screenXYSecond);
        path.quadTo(screenMidPoint, screenXYFirst);
        painter.drawPath(path);

        QVector<QPointF> pointsOnBezier;
        QVector<float> sOnEdge;

        for (int k = 0; k < edgeDiscretization + 1; k++) {
            float t = float(k) / edgeDiscretization;
            QPointF coord = (1 - t) * (1 - t) * screenXYFirst + 2 * (1 - t) * t * screenMidPoint + t * t * screenXYSecond;
            pointsOnBezier << coord;
        }

        for (int k = 0; k < pointsOnBezier.size(); k++) {
            if (k == 0) {
                sOnEdge << 0.0;
            } else {
                float sJesima = workspace->distance(pointsOnBezier[k], pointsOnBezier[k - 1]);
                sOnEdge << sJesima;
            }
        }

        for (int k = 1; k < pointsOnBezier.size(); k++) {
            sOnEdge[k] = sOnEdge[k] + sOnEdge[k - 1];
        }

        float length = sOnEdge.last();

        for (int k = 0; k < sOnEdge.size(); k++) {
            sOnEdge[k] = sOnEdge[k] / length;
        }

        sOnBezier.insert(edgesIds[i], sOnEdge);

        if (path.intersects(QRectF(mouseCurrentPos.x() - 7, mouseCurrentPos.y() - 7, 14, 14))) {
            if (firstTime) {
                firstTime = false;
                hitElements.clear();
            }

            QPoint temp(2, edgesIds[i]);
            hitElements.append(temp);
        }
    }
}

void EditorArea::paintNodes(QPainter &painter)
{
    bool firstTime = true;

    qreal radius = size2screen(6);
    qreal width = radius * 2;
    qreal height = radius * 2;

    QVector<int> nodesIds = workspace->getNodesIds();

    for (int i=0; i < nodesIds.size(); i++) {
        QPointF screenXY = graph2screen(workspace->getNodePosition(nodesIds[i]));

        if (workspace->getNodeMov(nodesIds[i])) {
            painter.setPen(Qt::NoPen);
        } else {
            painter.setPen(QPen(Qt::darkRed, size2screen(3), Qt::SolidLine,
                               Qt::RoundCap, Qt::RoundJoin));
        }

        QPainterPath path;
        path.arcTo(QRectF(-radius, -radius, width, height), 0, 360);
        path.translate(screenXY);
        painter.drawPath(path);

        QPainterPath mousePath;
        mousePath.arcTo(QRectF(-5, -5, 10, 10), 0, 360);
        mousePath.translate(mouseCurrentPos);

        if (path.intersects(mousePath)) {
            if (firstTime) {
                firstTime = false;
                hitElements.clear();
            }
            QPoint temp(1, nodesIds[i]);
            hitElements.append(temp);
        }
    }
}

void EditorArea::paintGrid(QPainter& painter)
{
    painter.setPen(QPen(Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));

    float gridSpacing = workspace->getGridSpacing();

    float spacingOnScreen = size2screen(gridSpacing);

    QPointF screenOrigin(0, 0);
    QPointF screenEnd(width(), height());
    QPointF originPhysCoord = screen2graph(screenOrigin);

    float A = (originPhysCoord.x() - int(originPhysCoord.x() / gridSpacing) * gridSpacing) / gridSpacing;

    float B = (originPhysCoord.y() - int(originPhysCoord.y() / gridSpacing) * gridSpacing) / gridSpacing;

    int countX = 0;
    int countY = 0;
    while (- size2screen(A * gridSpacing) + countX * spacingOnScreen <= screenEnd.x() + 2 * spacingOnScreen) {
        while (- size2screen(B * gridSpacing) + countY * spacingOnScreen <= screenEnd.y() + 2 * spacingOnScreen) {

            painter.drawPoint(QPoint( - size2screen(A * gridSpacing) + countX * spacingOnScreen,
                                      - size2screen(B * gridSpacing) + countY * spacingOnScreen ));
            countY++;
        }
        countY = 0;
        countX++;
    }

}

void EditorArea::paintHighlightingNode(QPainter& painter)
{
    QMap<QString, int> supportNodes(workspace->getSupportNodes());

    QPointF screenXY = graph2screen(workspace->getNodePosition(supportNodes.value("highlightingNode")));

    qreal radius = size2screen(14);
    qreal rectX = radius;
    qreal rectY = radius;
    qreal width = radius * 2;
    qreal height = radius * 2;

    int translateX = screenXY.x();
    int translateY = screenXY.y();
    painter.translate(translateX, translateY);
    painter.drawPie(QRect(- rectX, - rectY, width, height), 0, 360 *16);

    painter.translate( - translateX, - translateY);
}

void EditorArea::paintTooCloseNodes(QPainter& painter)
{
    QMap<QString, int> supportNodes(workspace->getSupportNodes());
    QMapIterator<QString, int> nodesIterator(supportNodes);

    qreal radius = size2screen(10);
    qreal rectX = radius;
    qreal rectY = radius;
    qreal width = radius * 2;
    qreal height = radius * 2;

    while (nodesIterator.hasNext()) {
        nodesIterator.next();
        if (nodesIterator.key() == "tooCloseNode") {
            QPointF screenXY = graph2screen(workspace->getNodePosition(nodesIterator.value()));
            int translateX = screenXY.x();
            int translateY = screenXY.y();
            painter.translate(translateX, translateY);
            painter.drawPie(QRect(- rectX, - rectY, width, height), 0, 360 *16);

            painter.translate( - translateX, - translateY);
        }
    }
}

void EditorArea::paintHighlightingEdge(QPainter& painter)
{
    QMap<QString, int> supportEdges(workspace->getSupportEdges());

    QPoint nodesOfEdge = workspace->getNodesOfEdge(supportEdges.value("highlightingEdge"));
    QPointF screenXYFirst = graph2screen(workspace->getNodePosition(nodesOfEdge.x()));
    QPointF screenXYSecond = graph2screen(workspace->getNodePosition(nodesOfEdge.y()));
    QPointF screenMidPoint = graph2screen(workspace->getThirdPointPos(supportEdges.value("highlightingEdge")));

    QPainterPath path;
    path.moveTo(screenXYFirst);
    path.quadTo(screenMidPoint, screenXYSecond);
    path.quadTo(screenMidPoint, screenXYFirst);
    path.closeSubpath();
    painter.drawPath(path);
}

void EditorArea::paintSelectedNodes(QPainter& painter)
{
    QVector<int> selectedNodes(workspace->getSelectedNodes());

    qreal radius = size2screen(11);
    qreal rectX = radius;
    qreal rectY = radius;
    qreal width = radius * 2;
    qreal height = radius * 2;

    for (int i = 0; i < selectedNodes.size(); i++) {
        QPointF screenXY = graph2screen(workspace->getNodePosition(selectedNodes[i]));

        int translateX = screenXY.x();
        int translateY = screenXY.y();
        painter.translate(translateX, translateY);
        painter.drawPie(QRect(- rectX, - rectY, width, height), 0, 360 *16);

        painter.translate( - translateX, - translateY);
    }

}

void EditorArea::paintSupportNodes4selecting(QPainter &painter)
{
    QVector<int> supportNodes4selecting(workspace->getSupportNodes4selecting());

    qreal radius = size2screen(9);
    qreal rectX = radius;
    qreal rectY = radius;
    qreal width = radius * 2;
    qreal height = radius * 2;

    for (int i = 0; i < supportNodes4selecting.size(); i++) {
        QPointF screenXY = graph2screen(workspace->getNodePosition(supportNodes4selecting[i]));

        int translateX = screenXY.x();
        int translateY = screenXY.y();
        painter.translate(translateX, translateY);
        painter.drawPie(QRect(- rectX, - rectY, width, height), 0, 360 *16);

        painter.translate( - translateX, - translateY);
    }

}

void EditorArea::paintSelectedEdges(QPainter& painter)
{
    QVector<int> selectedEdges(workspace->getSelectedEdges());

    for (int i = 0; i < selectedEdges.size(); i++) {
        QPoint nodesOfEdge = workspace->getNodesOfEdge(selectedEdges[i]);
        QPointF screenXYFirst = graph2screen(workspace->getNodePosition(nodesOfEdge.x()));
        QPointF screenXYSecond = graph2screen(workspace->getNodePosition(nodesOfEdge.y()));
        QPointF screenMidPoint = graph2screen(workspace->getThirdPointPos(selectedEdges[i]));

        QPainterPath path;
        path.moveTo(screenXYFirst);
        path.quadTo(screenMidPoint, screenXYSecond);
        path.quadTo(screenMidPoint, screenXYFirst);
        painter.drawPath(path);
    }
}

void EditorArea::paintSupportEdges4selecting(QPainter& painter)
{
    QVector<int> supportEdges4selecting(workspace->getSupportEdges4selecting());

    for (int i = 0; i < supportEdges4selecting.size(); i++) {
        QPoint nodesOfEdge = workspace->getNodesOfEdge(supportEdges4selecting[i]);
        QPointF screenXYFirst = graph2screen(workspace->getNodePosition(nodesOfEdge.x()));
        QPointF screenXYSecond = graph2screen(workspace->getNodePosition(nodesOfEdge.y()));
        QPointF screenMidPoint = graph2screen(workspace->getThirdPointPos(supportEdges4selecting[i]));

        QPainterPath path;
        path.moveTo(screenXYFirst);
        path.quadTo(screenMidPoint, screenXYSecond);
        path.quadTo(screenMidPoint, screenXYFirst);
        painter.drawPath(path);
    }
}

void EditorArea::paintSelectingArea(QPainter& painter)
{
    QPointF selectingElementsStart = workspace->getSelectingElementsStart();
    QPointF selectingElementsEnd = workspace->getSelectingElementsEnd();

    painter.drawRect(QRectF(graph2screen(selectingElementsStart), graph2screen(selectingElementsEnd)));
}

void EditorArea::paintMeshEls(QPainter& painter)
{
    QVector<int> nodesIds = workspace->getNodesIds();
    QVector<int> edgesIds = workspace->getEdgesIds();
    bool showMesh = workspace->getShowMeshStatus();
    int selectedTool = workspace->getSelectedTool();
    bool firstTime = true;
    bool firstTimeNode = true;

    qreal radius = size2screen(8);
    float fontSize = size2screen(12);

    qreal width = radius * 2;
    qreal height = radius * 2;

    painter.setFont(QFont("Arial", fontSize));
    for (int i = 0; i < nodesIds.size(); i++) {
        QString nodeMTypeString = workspace->getNodeMType(nodesIds[i]);
        QString elementType;
        elementType.clear();
        if (!nodeMTypeString.isEmpty()) {
            if (nodeMTypeString == "0D_Anastomosis") {
                elementType = "A";
            } else if (nodeMTypeString == "0D_FiveDofsV2") {
                elementType = "5";
            } else if (nodeMTypeString == "0D_EndSegment") {
                elementType = "E";
            } else if (nodeMTypeString == "0D_TwoDofsResistance") {
                elementType = "2";
            }
        }

        if (!nodeMTypeString.isNull()) {
            QPointF nodePos = graph2screen(workspace->getNodePosition(nodesIds[i]));
            int nodeMElementId = workspace->getNodeMElementId(nodesIds[i]);

            meshElsPos.insert(nodeMElementId, nodePos);
            meshElsType.insert(nodeMElementId, elementType);
            meshId2GraphId.insert(nodeMElementId, QPoint(1, nodesIds[i]));

            QPainterPath path;
            QPainterPath text;

            path.arcTo(QRectF(-radius*1.2, -radius*1.2, width*1.2, height*1.2), 0, 360);
            text.addText(-radius / 2.0, radius / 2.0, QFont("Arial", fontSize), elementType);
            path.translate(nodePos);
            text.translate(nodePos);

            if (showMesh) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(Qt::darkYellow);
                painter.drawPath(path);
                painter.setPen(Qt::yellow);
                painter.setBrush(Qt::yellow);
                painter.drawPath(text);
            } else {
                qreal dotR = size2screen(2);
                QPainterPath dotPath;
                painter.setPen(Qt::gray);
                painter.setBrush(Qt::gray);
                dotPath.arcTo(QRectF(-dotR, -dotR, dotR * 2, dotR * 2), 0, 360);
                dotPath.translate(nodePos);
                painter.drawPath(dotPath);
            }

            QPainterPath mousePath;
            mousePath.arcTo(QRectF(-1, -1, 1, 1), 0, 360);
            mousePath.translate(mouseCurrentPos);

            if (path.intersects(mousePath) && selectedTool == 7) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(Qt::darkCyan);
                painter.drawPath(path);
                painter.setPen(Qt::yellow);
                painter.setBrush(Qt::yellow);
                painter.drawPath(text);

                if (firstTimeNode) {
                    firstTimeNode = false;
                    hitMeshEls.clear();
                }

                QPoint temp(1, nodeMElementId);
                hitMeshEls.append(temp);
            }
        }
    }

    for (int j = 0; j < edgesIds.size(); j++) {
        QVector<float> sCoord = workspace->getEdgeMs(edgesIds[j]);
        QVector<QString> edgeMTypes = workspace->getEdgeMTypes(edgesIds[j]);
        QVector<int> edgeMElementsId = workspace->getEdgeMElementsId(edgesIds[j]);

        for (int h = 0; h < sCoord.size(); h++) {
            float s;
            if (h == sCoord.size() - 1.0) {
                s = (1.0 - sCoord[h]) / 2.0 + sCoord[h];
            } else {
                s = (sCoord[h + 1] - sCoord[h]) / 2.0 + sCoord[h];
            }

            int t = 0;
            while (sOnBezier.value(edgesIds[j]).at(t) < s) {
                t = t + 1;
            }
            float deltaS = s - float(t - 1) / edgeDiscretization;

            float m = float(t - 1) / edgeDiscretization + deltaS;

            QString elementType;
            elementType.clear();
            if (!edgeMTypes[h].isEmpty()) {
                if (edgeMTypes[h] == "0D_FiveDofsV2") {
                    elementType = "5";
                } else if (edgeMTypes[h] == "0D_EndSegment") {
                    elementType = "E";
                } else if (edgeMTypes[h] == "0D_Anastomosis") {
                    elementType = "A";
                } else if (edgeMTypes[h] == "0D_TwoDofsResistance") {
                    elementType = "2";
                }
            }

            QPoint nodesOfEdge = workspace->getNodesOfEdge(edgesIds[j]);
            QPointF screenXYFirst = graph2screen(workspace->getNodePosition(nodesOfEdge.x()));
            QPointF screenXYSecond = graph2screen(workspace->getNodePosition(nodesOfEdge.y()));
            QPointF screenMidPoint = graph2screen(workspace->getThirdPointPos(edgesIds[j]));

            QPointF pointPos = (1 - m) * (1 - m) * screenXYFirst + 2 * (1 - m) * m * screenMidPoint + m * m * screenXYSecond;

            meshElsPos.insert(edgeMElementsId[h], pointPos);
            meshElsType.insert(edgeMElementsId[h], elementType);
            meshId2GraphId.insert(edgeMElementsId[h], QPoint(2, edgesIds[j]));

            QPainterPath path;
            QPainterPath text;

            path.arcTo(QRectF(-radius, -radius, width, height), 0, 360);
            text.addText(-radius / 2.0, radius / 2.0, QFont("Arial", fontSize), elementType);
            path.translate(pointPos);
            text.translate(pointPos);
            if (showMesh) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(Qt::darkCyan);
                painter.drawPath(path);
                painter.setPen(Qt::yellow);
                painter.setBrush(Qt::yellow);
                painter.drawPath(text);
            } else {
                qreal dotR = size2screen(1);
                QPainterPath dotPath;
                painter.setPen(Qt::black);
                painter.setBrush(Qt::black);
                dotPath.arcTo(QRectF(-dotR, -dotR, dotR * 2, dotR * 2), 0, 360);
                dotPath.translate(pointPos);
                painter.drawPath(dotPath);
            }

            QPainterPath mousePath;
            mousePath.arcTo(QRectF(-1, -1, 1, 1), 0, 360);
            mousePath.translate(mouseCurrentPos);

            if (path.intersects(mousePath) && selectedTool == 7) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(Qt::darkCyan);
                painter.drawPath(path);
                painter.setPen(Qt::yellow);
                painter.setBrush(Qt::yellow);
                painter.drawPath(text);

                if (firstTime) {
                    firstTime = false;
                    hitMeshEls.clear();
                }

                QPoint temp(2, edgeMElementsId[h]);
                hitMeshEls.append(temp);
            }
        }
    }
}

void EditorArea::paintMeshElToBeHigh(QPainter &painter)
{
    qreal radius = size2screen(8);
    float fontSize = size2screen(12);

    qreal width = radius * 2;
    qreal height = radius * 2;

    QPainterPath path;
    QPainterPath text;
    QPainterPath circle;

    path.arcTo(QRectF(-radius, -radius, width, height), 0, 360);
    text.addText(-radius / 2.0, radius / 2.0, QFont("Arial", fontSize), meshElsType.value(meshElToBeHigh));
    circle.arcTo(QRectF(-radius * 3 / 2, -radius * 3 / 2, width * 3 / 2, height * 3 / 2), 0, 360);
    path.translate(meshElsPos.value(meshElToBeHigh));
    text.translate(meshElsPos.value(meshElToBeHigh));
    circle.translate(meshElsPos.value(meshElToBeHigh));

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::magenta);
    painter.drawPath(circle);
    painter.setBrush(Qt::darkCyan);
    painter.drawPath(path);
    painter.setPen(Qt::yellow);
    painter.setBrush(Qt::yellow);
    painter.drawPath(text);
}

void EditorArea::setMeshElToBeHigh(int theMeshElToBeHigh)
{
    meshElToBeHigh = theMeshElToBeHigh;
    update();
}

void EditorArea::resetMeshElToBeHigh()
{
    meshElToBeHigh = -1;
}

QPoint EditorArea::getGraphEl(int meshId)
{
    return meshId2GraphId.value(meshId);
}

void EditorArea::paintLabels(QPainter& painter)
{
    QVector<int> nodesIds = workspace->getNodesIds();
    QVector<int> edgesIds = workspace->getEdgesIds();
    QString temp;
    float labelW = size2screen(150.0);
    float labelH = size2screen(10.0);
    float fontSize = size2screen(10.0);

    float x(size2screen(8));

    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", fontSize));

    for (int i = 0; i < nodesIds.size(); i++) {
        temp = workspace->getNodeName(nodesIds[i]);
        QPointF screenXY = graph2screen(workspace->getNodePosition(nodesIds[i]));
        painter.translate(screenXY.x() + x, screenXY.y());
        painter.drawText(0, 0, labelW, labelH, Qt::AlignLeft | Qt::AlignTop, temp );
        painter.translate(- screenXY.x() - x, - screenXY.y());
    }

    painter.setPen(Qt::darkBlue);

    for (int i = 0; i < edgesIds.size(); i++) {
        float s = 0.5;

        int t = 0;
        while (sOnBezier.value(edgesIds[i]).at(t) < s) {
            t = t + 1;
        }
        float deltaS = s - float(t - 1) / edgeDiscretization;

        float m = float(t) / edgeDiscretization + deltaS;

        QPoint nodesOfEdge = workspace->getNodesOfEdge(edgesIds[i]);
        QPointF screenXYFirst = graph2screen(workspace->getNodePosition(nodesOfEdge.x()));
        QPointF screenXYSecond = graph2screen(workspace->getNodePosition(nodesOfEdge.y()));
        QPointF screenMidPoint = graph2screen(workspace->getThirdPointPos(edgesIds[i]));

        QPointF pointPos = (1 - m) * (1 - m) * screenXYFirst + 2 * (1 - m) * m * screenMidPoint + m * m * screenXYSecond;
        temp = workspace->getEdgeName(edgesIds[i]);

        painter.translate(pointPos.x() + x, pointPos.y());
        painter.drawText(0, 0, labelW, labelH, Qt::AlignLeft | Qt::AlignVCenter, temp);
        painter.translate(- pointPos.x() - x, - pointPos.y());
    }
}

void EditorArea::paintResults(QPainter &painter)
{
    qcout << "EA::paintResults" << endl;

    QVector<int> edgesIds = workspace->getEdgesIds();
    float labelW = size2screen(150.0);
    float labelH = size2screen(10.0);
    float fontSize = size2screen(10.0);

    float x(size2screen(8));

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", fontSize));

    QVector<QPointF> edgeResults;
    QString resString;

    for (int i = 0; i < edgesIds.count(); i++) {
        edgeResults = workspace->getEdgeResults(edgesIds[i]);
        for (int j = 0; j < edgeResults.count(); j++) {
            //sString.setNum(edgeResults[j].x());
            resString.setNum(edgeResults[j].y());
            //qcout << "edge" << edgesIds[i] <<" s" << j << " " <<edgeResults[j].x() << " " << edgeResults[j].y() << endl;

            float s = edgeResults[j].x();
            if (s == 0) {
                s = 0.2;
            }
            if (s == 1) {
                s = 0.8;
            }

            int t = 0;
            while (sOnBezier.value(edgesIds[i]).at(t) < s) {
                t = t + 1;
            }
            float deltaS = s - float(t - 1) / edgeDiscretization;

            float m = float(t) / edgeDiscretization + deltaS;

            QPoint nodesOfEdge = workspace->getNodesOfEdge(edgesIds[i]);
            QPointF screenXYFirst = graph2screen(workspace->getNodePosition(nodesOfEdge.x()));
            QPointF screenXYSecond = graph2screen(workspace->getNodePosition(nodesOfEdge.y()));
            QPointF screenMidPoint = graph2screen(workspace->getThirdPointPos(edgesIds[i]));

            QPointF pointPos = (1 - m) * (1 - m) * screenXYFirst + 2 * (1 - m) * m * screenMidPoint + m * m * screenXYSecond;

            painter.translate(pointPos.x() + x, pointPos.y());
            painter.drawText(0, 0, labelW, labelH, Qt::AlignLeft | Qt::AlignVCenter, resString);
            painter.translate(- pointPos.x() - x, - pointPos.y());

        }
    }
}

QPointF EditorArea::graph2screen(QPointF graphCoord)
{
    float zoomFactor = workspace->getZoomFactor();
    QPointF screenOrigin = workspace->getScreenOrigin();
    return QPointF ( zoomFactor / 100.0 * graphCoord.x() - screenOrigin.x() * zoomFactor / 100,
                     zoomFactor / 100.0 * graphCoord.y() - screenOrigin.y() * zoomFactor / 100 );
}

QPointF EditorArea::screen2graph(QPointF screenCoord)
{
    float zoomFactor = workspace->getZoomFactor();
    QPointF screenOrigin = workspace->getScreenOrigin();
    return QPointF ( screenOrigin.x() + screenCoord.x() / zoomFactor * 100.0,
                     screenOrigin.y() + screenCoord.y() / zoomFactor * 100.0 );
}

float EditorArea::size2screen(float graphSize)
{
    float zoomFactor = workspace->getZoomFactor();
    return graphSize * zoomFactor / 100;
}

float EditorArea::size2graph(float screenSize)
{
    float zoomFactor = workspace->getZoomFactor();
    return screenSize * 100 / zoomFactor;
}
