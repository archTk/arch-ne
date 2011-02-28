#include "resultsview.h"
#include "ui_resultsview.h"

ResultsView::ResultsView(QString cookie, QPixmap pressurePic, QPixmap flowPic, QString PMean,
                         QString FMean, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultsView)
{
    ui->setupUi(this);
    //QPixmap pressureImage = *pressurePic.scaledToWidth(50, Qt::SmoothTransformation);
    QPixmap pressureImage = pressurePic.scaledToWidth(100, Qt::SmoothTransformation);
    QPixmap flowImage= flowPic.scaledToWidth(100, Qt::SmoothTransformation);
    ui->pressureLabel->setPixmap(pressureImage);
    ui->PValue->setText(PMean);

    ui->flowLabel->setPixmap(flowImage);
    ui->FValue->setText(FMean);

    setResultsCookie(cookie);
}

ResultsView::~ResultsView()
{
    delete ui;
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
