#ifndef ELEMENTEDITOR_H
#define ELEMENTEDITOR_H

#include <QWidget>

class QStandardItem;

class ElementEditor : public QWidget
{
    Q_OBJECT

public:
    ElementEditor(QWidget *parent=0);
    QStringList getElementsList(QString itemName);
    QStringList getAttributesList(QString itemName);

public slots:

signals:

private:
    QList<QStandardItem *> prepareRow(const QString &first, const QString &second, const QString &third);
};

#endif

