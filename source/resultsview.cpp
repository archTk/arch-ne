#include "resultsview.h"
#include "ui_resultsview.h"

ResultsView::ResultsView(QPixmap* results, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultsView)
{
    ui->setupUi(this);
    ui->resultslabel->setPixmap(*results);
}

ResultsView::~ResultsView()
{
    delete ui;
}
