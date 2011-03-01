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

#include "resultsview.h"
#include "ui_resultsview.h"

#include <iostream>
#include <QTextStream>
using namespace std;

QTextStream resultsout(stdout);

ResultsView::ResultsView(QString cookie, QPixmap pressurePix, QPixmap flowPix, QString PMean,
                         QString FMean, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultsView)
{
    ui->setupUi(this);

    connect(ui->splitter, SIGNAL(splitterMoved(int, int)), this, SLOT(resizeLabels()));

    pressPix = pressurePix;
    flPix = flowPix;
    QPixmap shownPImage = pressPix.scaledToWidth(50, Qt::SmoothTransformation);
    QPixmap shownFImage= flowPix.scaledToWidth(50, Qt::SmoothTransformation);

    ui->pressureLabel->setPixmap(shownPImage);
    ui->PValue->setText(PMean);

    ui->flowLabel->setPixmap(shownFImage);
    ui->FValue->setText(FMean);

    setResultsCookie(cookie);
}

ResultsView::~ResultsView()
{
    delete ui;
}

void ResultsView::resizeEvent(QResizeEvent *event)
{
    resizeLabels();
}

void ResultsView::resizeLabels()
{
    QSize newPSize = ui->pressureLabel->size();
    QPixmap newPPixmap;
    newPPixmap = pressPix.scaled(newPSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->pressureLabel->setPixmap(newPPixmap);

    QSize newFSize = ui->flowLabel->size();
    QPixmap newFPixmap;
    newFPixmap = flPix.scaled(newFSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->flowLabel->setPixmap(newFPixmap);

    //resizeLabels();
}

void ResultsView::okButtClicked()
{
    emit okButtonClicked(resultsCookie);
    emit deleteItself();
}

void ResultsView::setResultsCookie(QString theCookie)
{
    resultsCookie = theCookie;
}
