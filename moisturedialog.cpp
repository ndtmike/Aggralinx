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
    list_iterator = 0;
}

MoistureDialog::~MoistureDialog()
{
    delete ui;
}

void MoistureDialog::initActionsConnections()
{
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->btnEnter, SIGNAL(clicked()), this, SLOT(enterData()));
    connect(ui->btnFinish, SIGNAL(clicked()), this, SLOT(finish()) );
    connect(ui->btnForward, SIGNAL(clicked()), this, SLOT(forward()));
    connect(ui->btnBackward, SIGNAL(clicked()), this, SLOT(backward()));
}

bool MoistureDialog::loadFile(const QString &fileName)
{
    QFile file(fileName);
    QTextStream in(&file);
    QString linebuf;

    list_iterator = 0;

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("MoistureDialog"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }else{
        while( !in.atEnd())
        {
            linebuf=in.readLine(maxLineLength());
            if(linebuf.indexOf("Direct") == -1){ //checks that data is the right type
                    QMessageBox::warning(this, tr("MoistureDialog"),
                                         linebuf + tr("\n is not a Direct Reading"));
                    return false;
            }
            linebuf = linebuf + dummyPercent();
            combinedData.append(InstrumentData(linebuf));
        }
    }
    list_iterator = 0;
    updateDialog();
    return (true);
}

void MoistureDialog::cancel()
{
    hide();
}

void MoistureDialog::enterData()
{
    double input_percent;
    QString buffer, maxPercentStr = "", minPercentStr = "";
    bool toDoubleOK = false;
    maxPercentStr.setNum(maxPercent(),'f',percentPrecision());
    minPercentStr.setNum(minPercent(),'f',percentPrecision());

    buffer = ui->lePercentStr->text();
    buffer = buffer.remove(QChar(' '),Qt::CaseInsensitive);
    input_percent = buffer.toDouble(&toDoubleOK);
    if(toDoubleOK == false){
        QMessageBox::information(this,"MoistureDialog",tr("Invalid Input:\n") + buffer);
        return;
    }
    if(input_percent < minPercent() || input_percent > maxPercent()){
        QMessageBox::information(this,tr("Enter Moisture"),tr("Invalid Input:\n")
                                 + buffer +tr("\n must be between ") + minPercentStr +
                                 tr(" and ") + maxPercentStr);
        return;
    }
    combinedData[list_iterator].updatePercentage( buffer );

    if(list_iterator<combinedData.size() - convertCountingNumbers()){//have to use this form because of ambiguity warning
        ++list_iterator;
    }else{
        list_iterator = 0;
    }
    emit btnEnterClick();
    updateDialog();
}

void MoistureDialog::forward()
{
    if(list_iterator<combinedData.size() - convertCountingNumbers()){//have to use this from because of ambiguity warning
        ++list_iterator;
    }else{
        list_iterator = 0;
    }
    updateDialog();
}

void MoistureDialog::backward()
{
    if(list_iterator > 0 ){//have to use this from because of ambiguity warning
        --list_iterator;
    }else{
        list_iterator = combinedData.size() - convertCountingNumbers();
    }
    updateDialog();
}
void MoistureDialog::finish()
{
    emit btnFinishClick();
    hide();
}


void MoistureDialog::updateDialog()
{
    QString b="";
    QTextStream display(&b);
    display<<tr("Record Number: ")<< list_iterator + convertCountingNumbers() <<'\n'
                <<tr("Reading Time: ")<<combinedData[list_iterator].rawTime()<<'\n'
                <<tr("Reading Date: ")<<combinedData[list_iterator].rawDate()<<'\n'
                <<tr("Reading Material: ")<<combinedData[list_iterator].rawMaterial()<<'\n'
                <<tr("Reading: ")<< combinedData[list_iterator].rawReading();
    ui->lineData->setText(b);
    ui->lePercentStr->setText( combinedData[list_iterator].rawPercentage());
    ui->lePercentStr->selectAll();
    ui->lePercentStr->setFocus();
}
