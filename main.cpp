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

#include <QApplication>

#include "mainwindow.h"
#include "appcontroller.h"

extern void qt_set_sequence_auto_mnemonic(bool);

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(ARCHNetworkEditor);

    qt_set_sequence_auto_mnemonic(true);

    QApplication app(argc, argv);
    MainWindow mainWin;

    mainWin.setGeometry(0, 0, 100, 100);
    mainWin.resize(800, 500);

    AppController appController;
    appController.setMainWindow(&mainWin);
    appController.setEditorArea(mainWin.getEditorArea());
    appController.createConnections();

    mainWin.initNewCase();

    mainWin.show();

    return app.exec();
}
