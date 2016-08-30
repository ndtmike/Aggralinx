/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: analydialog.h
**
** Header file for analydialog.cpp - aggralinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#ifndef MOISTUREDIALOG_H
#define MOISTUREDIALOG_H

#include <QtCore/QtGlobal>
#include <QDialog>
#include <QDate>
#include <QDateTime>

#include <QIODevice>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QVector>
#include <QList>
#include <iterator>

#include "instrumentdata.h"
//#include "mainwindow.h"

namespace Ui {
class MoistureDialog;
}

class MoistureDialog : public QDialog
{
    Q_OBJECT
public:    
    explicit MoistureDialog(QWidget *parent = 0);
    ~MoistureDialog();

    void changedata(QString texttime, QString testpercent);
    QString getPercent();
    void display();

signals:
    void btnBackClick();
    void btnEnterClick();
    void btnFinishClick();
    void btnForwardClick();

private slots:
    void backward();
    void cancel();
    void enterData();
    void finish();
    void forward();

private:
    void initActionsConnections();

    Ui::MoistureDialog *ui;
};
#endif // MOISTUREDIALOG_H
