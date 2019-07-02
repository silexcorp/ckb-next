#ifndef MODELISTWIDGET_H
#define MODELISTWIDGET_H

#include <QTableWidget>
#include <QTimer>

// Reorderable list widget

class ModeListWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit ModeListWidget(QWidget *parent = 0);
    void addItem(QTableWidgetItem* itm, QIcon eventicn);
    void setCurrentRow(int idx);
    int count();
    QTableWidgetItem* item(int idx);
    void clear();
    void setup();
    QTableWidgetItem* getIconItem(QTableWidgetItem* item);

signals:
    void orderChanged();

private slots:
    void timerTick();
    void enter(QTableWidgetItem* item);
    void change(QTableWidgetItem* item);

private:
    QVariant        currentData;
    QList<QVariant> previousItems;
    QVariant        dragged;
    QTimer          reorderTimer;

    const static int DATA_ROLE = Qt::UserRole + 100;

    void rescanItems();
};

#endif // MODELISTWIDGET_H
