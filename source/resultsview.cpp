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

ResultsView::ResultsView(QString cookie, QPixmap pressurePic, QPixmap flowPic, QString PMean,
                         QString FMean, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultsView)
{
    ui->setupUi(this);
    pressureImage = pressurePic;
    flowImage = flowPic;
    QPixmap shownPImage = pressureImage.scaledToWidth(50, Qt::SmoothTransformation);
    QPixmap shownFImage= flowImage.scaledToWidth(50, Qt::SmoothTransformation);
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
    if ((ui->pressureLabel->height() + ui->flowLabel->height()) > height() * 0.8 ) {
        return;
    }

    const QPixmap* temp = ui->pressureLabel->pixmap();
    QPixmap newImage;
    newImage = (*temp).scaledToWidth(width() * 0.8, Qt::SmoothTransformation);
    ui->pressureLabel->setPixmap(newImage);
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
