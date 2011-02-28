#ifndef RESULTSVIEW_H
#define RESULTSVIEW_H

#include <QWidget>

namespace Ui {
    class ResultsView;
}

class ResultsView : public QWidget
{
    Q_OBJECT

public:
    explicit ResultsView(QString cookie, QPixmap pressurePic, QPixmap flowPic, QString PMean,
                         QString FMean, QWidget *parent = 0);
    ~ResultsView();

private:
    Ui::ResultsView *ui;
    void setResultsCookie(QString theCookie);

signals:
    void okButtonClicked(QString cookie);
    void deleteItself();

private slots:
    void okButtClicked();

private:
    QString resultsCookie;
};

#endif // RESULTSVIEW_H
