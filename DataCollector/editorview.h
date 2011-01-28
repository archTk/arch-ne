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

#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#include <QTreeView>
#include <QKeyEvent>

class EditorView : public QTreeView
{
    Q_OBJECT

public:
    EditorView(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent* event);
signals:
    void keyPressed(QString text);
public slots:

};

#endif 