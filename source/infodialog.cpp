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

#include <iostream>
#include <QTextStream>
using namespace std;

QTextStream infoDout(stdout);

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
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
}

void InfoDialog::abortButtonReleased()
{
    infoDout << "infoDialog::abortButtonReleased" << endl;
    emit abortSimulation();
    //done(1);
}
