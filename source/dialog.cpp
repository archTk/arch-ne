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

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include <QTextStream>
using namespace std;
QTextStream dialout(stdout);

Dialog::Dialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    connect(buttBox, SIGNAL(accepted()), this, SLOT(okPressed()));
    connect(buttBox, SIGNAL(rejected()), this, SLOT(cancelPressed()));

}

void Dialog::okPressed()
{
    dialout << "okPressed" << endl;
    // TO DO: save the data in the text field;
    cancelPressed();
}

void Dialog::cancelPressed()
{
    dialout << "cancel" << endl;
    close();
}
