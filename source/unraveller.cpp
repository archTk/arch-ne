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

#include "unraveller.h"

#include "graph.h"
#include "graphlayout.h"
#include "workspace.h"

#include <math.h>

#include <iostream>
#include <QTextStream>
using namespace std;

QTextStream unrout(stdout);

Unraveller::Unraveller(Graph* theGraph, GraphLayout* theGraphLayout, Workspace* theWorkspace, QObject *parent) :
    QObject(parent)
{
    graph = theGraph;
    graphLayout = theGraphLayout;
    workspace = theWorkspace;
}

void Unraveller::unravelNet(QPointF screenOrigin)
{
    QVector<int> nodesIds = graph->getNodesIds();
    QMap<int, QPointF> vel;
    vel.clear();
    double minDistance = 10000000.0;
    double maxVel = 0.0;

    for (int k = 0; k < nodesIds.size(); k++) {
        QPointF A = graphLayout->getNodePosition(nodesIds[k]);
        QVector<QPoint> edgesWithNode = graph->edgesWithNode(nodesIds[k]);
        qreal xvel = 0.0;
        qreal yvel = 0.0;
        if (!graphLayout->getNodeMovStatus(nodesIds[k])) {
            vel.insert(nodesIds[k], QPointF(xvel, yvel));
            continue;
        }

        for (int j = 0; j < nodesIds.size(); j++) {     // Repulsive force.
            if (j != k) {
                QPointF B = graphLayout->getNodePosition(nodesIds[j]);
                QPointF BA = B - A;
                qreal dx = BA.x();
                qreal dy = BA.y();
                double l = 2.0 * (dx * dx + dy * dy);
                double d = sqrt(dx * dx + dy * dy);

                if (minDistance > d) {
                    minDistance = d;
                }

                xvel -= (dx * 15.0) / l;
                yvel -= (dy * 15.0) / l;
            }
        }

        double weight = (graph->getEdgesIds().size() + 1) * 3;
        for (int h = 0; h < edgesWithNode.size(); h++) {    // Attractive force.
            QPointF C;
            if (edgesWithNode[h].y() == 1) {
                C = graphLayout->getNodePosition(graph->getNodesOfEdge(edgesWithNode[h].x()).y());
            } else {
                C = graphLayout->getNodePosition(graph->getNodesOfEdge(edgesWithNode[h].x()).x());
            }

            QPointF CA = C - A;
            qreal dx = CA.x();
            qreal dy = CA.y();

            xvel += dx / weight;
            yvel += dy / weight;
        }

        if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1) {
            xvel = yvel = 0;
        }

        vel.insert(nodesIds[k], QPointF(xvel, yvel));
    }

    QMapIterator<int, QPointF> velIterator(vel);
    while (velIterator.hasNext()) {
        velIterator.next();

        if (sqrt(velIterator.value().x() * velIterator.value().x() + velIterator.value().y() * velIterator.value().y()) > maxVel) {
            maxVel = sqrt(velIterator.value().x() * velIterator.value().x() + velIterator.value().y() * velIterator.value().y());
        }
    }

    if (minDistance < maxVel) {
        qreal decrFactor = maxVel / minDistance;
        velIterator.toFront();
        while (velIterator.hasNext()) {
            velIterator.next();
            if (graphLayout->getNodeMovStatus(velIterator.key())) {
                float tempX = vel.value(velIterator.key()).x() / decrFactor;
                float tempY = vel.value(velIterator.key()).y() / decrFactor;

                QPointF temp(tempX, tempY);
                vel[velIterator.key()] = temp;
            }
        }
    }

    bool screenOriginChang = false;

    velIterator.toFront();
    while (velIterator.hasNext()) {
        velIterator.next();
        QPointF newPos = graphLayout->getNodePosition(velIterator.key()) + velIterator.value();
        if (screenOrigin.x() > newPos.x() - 10 ) {
            screenOrigin.setX(newPos.x() - 10 );
            screenOriginChang = true;
        }

        if (screenOrigin.y() > newPos.y() - 10 ) {
            screenOrigin.setY(newPos.y() - 10 );
            screenOriginChang = true;
        }

        if (screenOriginChang) {
            emit screenOriginChanged(screenOrigin);
        }

        graphLayout->setNodePosition(velIterator.key(), newPos);
    }
}
