#include "resultsview.h"
#include "ui_resultsview.h"

ResultsView::ResultsView(QString cookie, QPixmap* pressurePic, QPixmap* flowPic, QString PMean,
                         QString FMean, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultsView)
{
    ui->setupUi(this);
    ui->pressureLabel->setPixmap(*pressurePic);
    ui->pressureLabel->setGeometry(0, 0, 200, 0);
    ui->PValue->setText(PMean);

    ui->flowLabel->setPixmap(*flowPic);
    ui->flowLabel->setGeometry(0, 0, 200, 0);
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
