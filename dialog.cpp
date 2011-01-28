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
    setWindowTitle("Preferences");
    resize(600, 200);

    QLabel* pythonDir = new QLabel;
    pythonDir->setText(tr("Python directory"));

    QLabel* inputDirLabel = new QLabel;
    inputDirLabel->setText(tr("Input directory for meshing"));

    QLabel* outputDirLabel = new QLabel;
    outputDirLabel->setText(tr("Output directory for meshing"));

    QPushButton* cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    QPushButton* okButton = new QPushButton(tr("OK"));
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), this, SLOT(okPressed()));

    QGridLayout* buttLayout = new QGridLayout;
    buttLayout->addWidget(okButton, 0, 0);
    buttLayout->addWidget(cancelButton, 0, 1);

    QGridLayout* layout = new QGridLayout;
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 2);
    layout->addWidget(pythonDir, 0, 0);
    layout->addWidget(inputDirLabel, 1, 0);
    layout->addWidget(outputDirLabel, 2, 0);
    layout->addLayout(buttLayout, 3, 2);

    setLayout(layout);
}

void Dialog::okPressed()
{
    // TO DO: save the data in the text field;
    cancelPressed();
}

void Dialog::cancelPressed()
{
    close();
}
