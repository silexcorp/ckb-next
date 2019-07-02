#include "kbmodeeventmgr.h"
#include "ui_kbmodeeventmgr.h"
#include <QFileDialog>
#include <QDebug>
#include "kbwidget.h"
#include <QCloseEvent>

KbModeEventMgr::KbModeEventMgr(QWidget* parent, KbMode* m, QTableWidgetItem* itm) :
    QDialog(parent), info(m->winInfo()), item(itm), mode(m),
    ui(new Ui::KbModeEventMgr)
{
    ui->setupUi(this);
    ui->modeLabel->setText(QString(tr("Switch to mode \"%1\" when:")).arg(mode->name()));
    ui->windowTitleLineEdit->setFocus();
    ui->windowTitleLineEdit->setText(info->windowTitle);
    ui->programLineEdit->setText(info->program);
    ui->instanceNameLineEdit->setText(info->wm_instance_name);
    ui->classNameLineEdit->setText(info->wm_class_name);
    ui->enableBox->setChecked(info->enabled);
    if(info->windowTitleSubstr)
        ui->isContainsCombo->setCurrentIndex(1);
}

KbModeEventMgr::~KbModeEventMgr()
{
    delete ui;
}

void KbModeEventMgr::on_cancelBtn_clicked()
{
    this->close();
}

void KbModeEventMgr::closeEvent(QCloseEvent* evt)
{
    // Disable the mode event if empty
    if(info->isEmpty())
        info->enabled = false;

    // Update the UI icon
    item->setIcon(KbWidget::eventIcon(mode));
    info->setNeedsSave();

    // Continue handling the event
    evt->accept();
}

void KbModeEventMgr::on_okBtn_clicked()
{
    // Write everything
    info->windowTitle = ui->windowTitleLineEdit->text();
    info->program = ui->programLineEdit->text();
    info->wm_instance_name = ui->instanceNameLineEdit->text();
    info->wm_class_name = ui->classNameLineEdit->text();
    info->windowTitleSubstr = ui->isContainsCombo->currentIndex();
    info->enabled = ui->enableBox->isChecked();

    this->close();
}

void KbModeEventMgr::on_clearbtn_clicked()
{
    ui->windowTitleLineEdit->clear();
    ui->programLineEdit->clear();
    ui->instanceNameLineEdit->clear();
    ui->classNameLineEdit->clear();
    ui->isContainsCombo->setCurrentIndex(0);

    ui->okBtn->setFocus();
}

void KbModeEventMgr::on_browseButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select a program"), QString(), QString(), nullptr, QFileDialog::DontResolveSymlinks);
    if(file.isNull())
        return;
    ui->programLineEdit->setText(file);
}
