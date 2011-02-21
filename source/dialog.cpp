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

#include "dialog.h"

#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QSettings>

#include <QTextStream>
using namespace std;
QTextStream dialout(stdout);

Dialog::Dialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QSettings settings("archTk", "ARCHNetworkEditor");
    QString pythonPath = settings.value("pythonPath", QString()).toString();
    QString pyNSPath = settings.value("pyNSPath", QString()).toString();

    ui->pythonEdit->setText(pythonPath);
    ui->pyNSEdit->setText(pyNSPath);
}

void Dialog::cancelPressed()
{
    close();
}

void Dialog::okPressed()
{
    QSettings settings("archTk", "ARCHNetworkEditor");
    settings.setValue("pythonPath", ui->pythonEdit->text());
    settings.setValue("pyNSPath", ui->pyNSEdit->text());
    cancelPressed();
}

void Dialog::pythonChoosePressed()
{
    QString pythonPath = QFileDialog::getOpenFileName(this, tr("Set Python path"), "/home");
    ui->pythonEdit->setText(pythonPath);
}

void Dialog::pyNSChoosePressed()
{
    QString pyNSPath = QFileDialog::getExistingDirectory(this, tr("Set pyNS path"), "/home", QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui->pyNSEdit->setText(pyNSPath);
}
