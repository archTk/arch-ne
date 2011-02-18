#ifndef QUERYEDITOR_H
#define QUERYEDITOR_H

#include <QWidget>

class QPlainTextEdit;
class QLineEdit;

class QueryEditor : public QWidget
{
    Q_OBJECT

public:
    QueryEditor(QWidget *parent=0);

public slots:
    void displayQuery();
    void updateQuery();
    void setDocFileName();
    void setQueryFileName();

signals:
    void valueChanged(int newValue);

private:
    QPlainTextEdit *queryTextEdit;
    QPlainTextEdit *outputTextEdit;
    QLineEdit *docNameLineEdit;
    QLineEdit *queryFileNameLineEdit;
    QString docFileName;
    QString queryFileName;
    void evaluateQuery(const QString &str);
};

#endif

