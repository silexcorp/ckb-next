#include <QUuid>
#include "modelistwidget.h"
#include <QHeaderView>
#include <QDebug>
#include <QTableWidgetItem>

ModeListWidget::ModeListWidget(QWidget *parent) :
    QTableWidget(parent)
{
    reorderTimer.setSingleShot(true);
    reorderTimer.setInterval(100);
    connect(&reorderTimer, SIGNAL(timeout()), this, SLOT(timerTick()));

    connect(this, SIGNAL(itemEntered(QTableWidgetItem*)), this, SLOT(enter(QTableWidgetItem*)));
    connect(this, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(change(QTableWidgetItem*)));
}

// For some reason this can't be called in the constructor
void ModeListWidget::setup()
{
    verticalHeader()->setSectionsMovable(true);
    setColumnCount(2);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    qDebug() << "setup called";
}

void ModeListWidget::timerTick(){
    qDebug() << "Tick";
    bool reordered = false;
    QTableWidgetItem* reselect = 0;
    QList<QVariant> newItems;
    // Scan the item list to see if they changed
    int c = rowCount();
    for(int i = 0; i < c; i++){
        QTableWidgetItem* itm = QTableWidget::item(i, 0);
        QVariant data = itm->data(DATA_ROLE);
        newItems.append(data);
        if(i >= previousItems.count() || data != previousItems[i])
            reordered = true;
        // Re-select the dragged item (if any)
        if(data == dragged)
            reselect = itm;
    }
    if(previousItems.length() != newItems.length())
        return;
    if(reordered){
        previousItems = newItems;
        emit orderChanged();
        if(reselect){
            reselect->setSelected(true);
            setCurrentItem(reselect);
            dragged = QVariant();
        }
    }
}

void ModeListWidget::enter(QTableWidgetItem* item){
    rescanItems();
    // Check for drag+drop setup
    if(item)
        dragged = item->data(DATA_ROLE);
}

void ModeListWidget::change(QTableWidgetItem* item){
    reorderTimer.stop();
    reorderTimer.start();
}

void ModeListWidget::rescanItems(){
    QList<QVariant> newItems;
    int c = rowCount();
    for(int i = 0; i < c; i++){
        QVariant data = QTableWidget::item(i, 0)->data(DATA_ROLE);
        if(data.isNull()){
            // Generate the ID for this item if it doesn't already exist
            data = QUuid::createUuid();
            QTableWidget::item(i, 0)->setData(DATA_ROLE, data);
        }
        newItems.append(data);
    }
    previousItems = newItems;
}
#include <QDebug>
void ModeListWidget::addItem(QTableWidgetItem* itm, QIcon eventicn)
{
    // Get the number of rows, and add the new item to col 0, row x
    insertRow(rowCount());
    setItem(rowCount() - 1, 0, itm);
    QTableWidgetItem* iconitm = new QTableWidgetItem(eventicn, QString());
    iconitm->setToolTip(tr("Event"));
    setItem(rowCount() - 1, 1, iconitm);
}

QTableWidgetItem* ModeListWidget::getIconItem(QTableWidgetItem* item)
{
    return QTableWidget::item(item->row(), 1);
}

void ModeListWidget::setCurrentRow(int idx)
{
    QTableWidget::setCurrentCell(idx, 0, QItemSelectionModel::Rows | QItemSelectionModel::Select);
}

int ModeListWidget::count()
{
    return rowCount();
}

QTableWidgetItem* ModeListWidget::item(int idx)
{
    return QTableWidget::item(idx, 0);
}

void ModeListWidget::clear()
{
    // This clears everything and frees memory
    setRowCount(0);
}
