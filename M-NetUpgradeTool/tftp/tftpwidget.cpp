﻿#include "tftpwidget.h"
#include "ui_tftpwidget.h"
#include "common\msgbox.h"

TftpWidget::TftpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TftpWidget)
{
    ui->setupUi(this);

    mData = DataPacket::bulid()->data;
    mExportDlg = new ExportDlg(this);
    mTftpThread = new TftpThread(this);

    timer = new QTimer(this);
    timer->start(500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}

TftpWidget::~TftpWidget()
{
    delete ui;
}

bool TftpWidget::checkFile()
{
    bool ret = false;
    QString fn = ui->lineEdit->text();
    if(!fn.isEmpty()) {
        QString str = fn.section(".", -1);
        if(str == "bin") {
            mData->file = fn;
            ret = true;
        } else {
            CriticalMsgBox box(this, tr("升级文件格式错误!, 请重新选择"));
        }
    } else {
        CriticalMsgBox box(this, tr("请选择升级文件"));
    }

    return ret;
}

void TftpWidget::on_openBtn_clicked()
{
    static QString fn;
    fn = QFileDialog::getOpenFileName(0,tr("文件选择"),fn,"",0);
    if (!fn.isNull()) {
        ui->lineEdit->setText(fn);
        checkFile();
    }
}


void TftpWidget::timeoutDone(void)
{
    QString str = mData->status;
    if(str.isEmpty()) str = tr("请开始");
    ui->stateLab->setText(str);

    if(mData->subPorgress>100) mData->subPorgress = 0;
    ui->progressBar_2->setValue(mData->subPorgress);

    bool en = mData->isRun;
    if(mData->ips.isEmpty()) en = true;
    ui->openBtn->setDisabled(en);
    ui->updateBtn->setDisabled(en);
    ui->exportBtn->setDisabled(en);
    ui->breakBtn->setEnabled(en);

    int x = 0;
    int count = mData->ips.size();
    if(count) x = ((mData->progress * 1.0) / count) *100;
    ui->progressBar->setValue(x);
}


void TftpWidget::on_updateBtn_clicked()
{    
    if(checkFile())
    {
        mTftpThread->startSend();
    }
}

void TftpWidget::on_exportBtn_clicked()
{
    mExportDlg->exec();
}

void TftpWidget::on_breakBtn_clicked()
{
    if(mData->isRun) {
        QuMsgBox box(this, tr("是否执行中断?"));
        if(box.Exec()) {
            mTftpThread->breakDown();
            InfoMsgBox msg(this, tr("软件即将重启!!!"));

            QProcess *process = new QProcess(this);
            process->start("M-NetUpgradeTool.exe");
            exit(0);
        }
    }
}
