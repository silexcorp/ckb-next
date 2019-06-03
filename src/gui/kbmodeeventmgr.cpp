#include "kbmodeeventmgr.h"
#include "ui_kbmodeeventmgr.h"
#include <QFileDialog>
#include <QDebug>

KbModeEventMgr::KbModeEventMgr(QWidget* parent, KbWindowInfo* info, QString modeName) :
    QDialog(parent), info_ptr(info),
    ui(new Ui::KbModeEventMgr)
{
    ui->setupUi(this);
    ui->modeLabel->setText(QString(tr("Switch to mode \"%1\" when:")).arg(modeName));
    ui->windowTitleLineEdit->setFocus();
    ui->windowTitleLineEdit->setText(info->windowTitle);
    ui->programLineEdit->setText(info->program);
    ui->instanceNameLineEdit->setText(info->wm_instance_name);
    ui->classNameLineEdit->setText(info->wm_class_name);
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

void KbModeEventMgr::on_okBtn_clicked()
{
    // Write everything
    info_ptr->windowTitle = ui->windowTitleLineEdit->text();
    info_ptr->program = ui->programLineEdit->text();
    info_ptr->wm_instance_name = ui->instanceNameLineEdit->text();
    info_ptr->wm_class_name = ui->classNameLineEdit->text();
    info_ptr->windowTitleSubstr = ui->isContainsCombo->currentIndex();
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
