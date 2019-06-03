#ifndef KBMODEEVENTMGR_H
#define KBMODEEVENTMGR_H

#include <QWidget>
#include <QDialog>
#include "kbwindowinfo.h"

namespace Ui {
class KbModeEventMgr;
}

class KbModeEventMgr : public QDialog
{
    Q_OBJECT

public:
    explicit KbModeEventMgr(QWidget *parent, KbWindowInfo* info, QString modeName);
    ~KbModeEventMgr();

private slots:
    void on_cancelBtn_clicked();

    void on_okBtn_clicked();

    void on_clearbtn_clicked();

    void on_browseButton_clicked();

private:
    KbWindowInfo* info_ptr;
    Ui::KbModeEventMgr *ui;

};

#endif // KBMODEEVENTMGR_H
