/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: moisturedialog.cpp
**
** Header file for moisturedialog.cpp - aggralinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#include "moisturedialog.h"
#include "ui_moisturedialog.h"

MoistureDialog::MoistureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoistureDialog)
{
    ui->setupUi(this);
    initActionsConnections();
}

MoistureDialog::~MoistureDialog()
{
    delete ui;
}

void MoistureDialog::changedata(QString timetext, QString testpercent){
    ui->lineData->setText(timetext);
    ui->lePercentStr->setText(testpercent);
}

void MoistureDialog::initActionsConnections()
{
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->btnEnter, SIGNAL(clicked()), this, SLOT(enterData()));
    connect(ui->btnFinish, SIGNAL(clicked()), this, SLOT(finish()) );
    connect(ui->btnForward, SIGNAL(clicked()), this, SLOT(forward()));
    connect(ui->btnBackward, SIGNAL(clicked()), this, SLOT(backward()));
}

void MoistureDialog::display()
{
    activateWindow();
    raise();
    show();
}


void MoistureDialog::cancel()
{
    hide();
}

void MoistureDialog::enterData()
{

    emit btnEnterClick();
}

void MoistureDialog::forward()
{

    emit btnForwardClick();
}

void MoistureDialog::backward()
{
    emit btnBackClick();
}
void MoistureDialog::finish()
{
    emit btnFinishClick();
    hide();
}
