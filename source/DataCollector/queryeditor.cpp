#include <QtGui>
#include <QPlainTextEdit>
#include <QSlider>
#include <QtXmlPatterns>

#include "queryeditor.h"
#include "queryexecuter.h"

QueryEditor::QueryEditor(QWidget *parent)
   : QWidget(parent)
{
    queryTextEdit = new QPlainTextEdit; 
    outputTextEdit = new QPlainTextEdit; 
    docNameLineEdit = new QLineEdit; 
    queryFileNameLineEdit = new QLineEdit; 

    QLabel *docNameLabel = new QLabel(tr("Document: "));
    QLabel *queryFileNameLabel = new QLabel(tr("Query File: "));

    QPushButton *runButton = new QPushButton(tr("Ok"));
    QPushButton *openDocButton = new QPushButton(tr("..."));
    QPushButton *openQueryButton = new QPushButton(tr("..."));

    connect(runButton,SIGNAL(clicked()),this,SLOT(updateQuery()));
    connect(openDocButton,SIGNAL(clicked()),this,SLOT(setDocFileName()));
    connect(openQueryButton,SIGNAL(clicked()),this,SLOT(setQueryFileName()));

    QGridLayout *layout = new QGridLayout;

    layout->addWidget(docNameLabel,0,0,1,1);
    layout->addWidget(docNameLineEdit,0,1,1,1);
    layout->addWidget(openDocButton,0,2,1,1);

    layout->addWidget(queryFileNameLabel,1,0,1,1);
    layout->addWidget(queryFileNameLineEdit,1,1,1,1);
    layout->addWidget(openQueryButton,1,2,1,1);

    layout->addWidget(outputTextEdit,2,0,1,3);
    layout->addWidget(queryTextEdit,3,0,1,3);
    layout->addWidget(runButton,4,0,1,1);
    setLayout(layout);
}

void QueryEditor::displayQuery()
{
    queryTextEdit->clear();
    QFile queryFile;
    queryFile.setFileName(queryFileName);
    queryFile.open(QIODevice::ReadOnly);
    const QString query(QString::fromLatin1(queryFile.readAll()));
    queryTextEdit->setPlainText(query);
}

void QueryEditor::setDocFileName()
{
    docFileName = QFileDialog::getOpenFileName(); 
    docNameLineEdit->setText(docFileName); 
}

void QueryEditor::setQueryFileName()
{
    queryFileName = QFileDialog::getOpenFileName(); 
    queryFileNameLineEdit->setText(queryFileName);
    displayQuery(); 
}

void QueryEditor::updateQuery()
{
    if(docFileName == "" or queryTextEdit->toPlainText() == ""){
        return;
    }
    outputTextEdit->clear();
    evaluateQuery(queryTextEdit->toPlainText());
}

void QueryEditor::evaluateQuery(const QString &str)
{
    QueryExecuter *query = new QueryExecuter; 
    query->useQueryFromString();
    query->setQueryString(str);
    query->setDocFile(docFileName);
    query->evaluateQuery();

    outputTextEdit->setPlainText(query->getQueryResult());
}

