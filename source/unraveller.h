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

#ifndef UNRAVELLER_H
#define UNRAVELLER_H

#include <QObject>
#include <QPointF>

class Graph;
class GraphLayout;
class Workspace;

class Unraveller : public QObject
{
    Q_OBJECT
public:
    explicit Unraveller(Graph* theGraph, GraphLayout* theGraphLayout, Workspace* theWorkspace, QObject *parent = 0);

signals:
    void screenOriginChanged(QPointF screenOrigin);

public slots:
    void unravelNet(QPointF screenOrigin);

private:
    Workspace* workspace;
    Graph* graph;
    GraphLayout* graphLayout;
};

#endif // UNRAVELLER_H
