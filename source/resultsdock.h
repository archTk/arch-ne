#ifndef RESULTSDOCK_H
#define RESULTSDOCK_H

#include <QDockWidget>
#include <QCloseEvent>

class ResultsDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit ResultsDock(QWidget *parent = 0);
    void closeEvent(QCloseEvent *event);

protected:
    void enterEvent(QEvent *);

signals:
    void mouseEnteredInResultsDock();
    void resultsDockClosed();

public slots:

};

#endif // RESULTSDOCK_H
