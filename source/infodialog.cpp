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

#include "infodialog.h"
#include "ui_infodialog.h"

#include <QCloseEvent>
#include <iostream>
#include <QTextStream>
using namespace std;

QTextStream infoDout(stdout);

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    setWindowFlags(Qt::WindowMinimizeButtonHint |
                   Qt::WindowSystemMenuHint);

    ui->setupUi(this);
    ui->abortButton->setCheckable(false);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}

void InfoDialog::initWithMessage(QString message)
{
    ui->infoLabel->setText(message);
    ui->advanceLabel->setText("");
}

void InfoDialog::abortButtonReleased()
{
    emit abortSimulation();
}

void InfoDialog::changeDisplayedMessage(QString theMessage)
{
    theMessage.replace("->", "");
    ui->advanceLabel->setText(theMessage);
}

void InfoDialog::changeEvent(QEvent *event)
{
    infoDout << "InfoDialog::changeEvent" << endl;
    if (event->type() == QEvent::WindowStateChange) {
        if (windowState() == Qt::WindowMinimized) {
            infoDout << "infoD minimized" << endl;
            infoDout << "InfoD changeEvent eventType= " << event->type() << endl;
            emit minimizeApp();
        } else if (windowState() == Qt::WindowActive) {
            infoDout << "infoD app active" << endl;
            emit maximizeApp();
        }
    }
}

void InfoDialog::closeEvent(QCloseEvent* event)
{
    emit abortSimulation();
    event->accept();
}
