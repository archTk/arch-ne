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
    explicit ResultsView(QPixmap* results, QWidget *parent = 0);
    ~ResultsView();

private:
    Ui::ResultsView *ui;

signals:
    void okButtonClicked();
    void deleteItself();

private slots:
    void okButtClicked();
};

#endif // RESULTSVIEW_H
