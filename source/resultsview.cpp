#include "resultsview.h"
#include "ui_resultsview.h"

ResultsView::ResultsView(QString cookie, QPixmap* results, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultsView)
{
    ui->setupUi(this);
    ui->resultsLabel->setPixmap(*results);
    ui->resultsLabel->setGeometry(0, 0, 400, 0);

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
