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

#ifndef EDITORAREA_H
#define EDITORAREA_H

#include <QWidget>
#include <QMouseEvent>

#include "applicationstatus.h"

class Workspace;

class EditorArea : public QWidget
{
    Q_OBJECT

public:
    EditorArea(QWidget* parent = 0);

    void clear();
    Workspace* getWorkspace();
    void setWorkspace(Workspace* theWorkspace);

public slots:
    void homeView();
    void updateRender();
    void zoomIn();
    void zoomOut();
    void toBeCleared();

private slots:

protected slots:

signals:
    void mouseBeenPressed(QPointF validPos);
    void mouseMoved(QPointF validPos);
    void mouseReleased(QPointF validPos);
    void zoomFactorChanged(float zoomFactor);
    void screenOriginChanged(QPointF screenOrigin);
    void shiftPressedChanged(bool shiftStatus);
    void updateHistory();
    void updateMainWindow();
    void Key_Cancel_Pressed();
    void Key_B_Pressed();
    void Key_CTRL_A_Pressed();
    void elementsBeenHit(QVector<QPoint> hitElements);

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    void paintEdges(QPainter& painter);
    void paintNodes(QPainter& painter);
    void paintHighlightingEdge(QPainter& painter);
    void paintHighlightingNode(QPainter& painter);
    void paintTooCloseNodes(QPainter& painter);
    void paintGrid(QPainter& painter);
    void paintSelectedNodes(QPainter& painter);
    void paintSelectedEdges(QPainter& painter);
    void paintSupportNodes4selecting(QPainter& painter);
    void paintSupportEdges4selecting(QPainter& painter);
    void paintSelectingArea(QPainter& painter);
    void paintMeshEls(QPainter& painter);
    void paintLabels(QPainter& painter);

    QPointF graph2screen(QPointF graphCoord);
    QPointF screen2graph(QPointF screenCoord);

    float size2screen(float graphSize);
    float size2graph(float screenSize);

    Workspace* workspace;

    QPoint mouseCurrentPos;

    QPoint screenOriginStartPos;
    QPoint screenOriginEndPos;

    QVector<QPoint> hitElements;

    QMap<int, QVector<float> > sOnBezier;
    int edgeDiscretization;
};

#endif // EDITORAREA_H
