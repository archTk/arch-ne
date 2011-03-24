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

#include <QApplication>
#include <QPlastiqueStyle>

#include "mainwindow.h"
#include "appcontroller.h"

extern void qt_set_sequence_auto_mnemonic(bool);

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(ARCHNetworkEditor);

    qt_set_sequence_auto_mnemonic(true);

    QApplication app(argc, argv);
    //app.setStyle(new QPlastiqueStyle);
    MainWindow mainWin;

    AppController appController;
    appController.setMainWindow(&mainWin);
    appController.setEditorArea(mainWin.getEditorArea());
    appController.createConnections();

    mainWin.clear();

    mainWin.show();

    return app.exec();
}
