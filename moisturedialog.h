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

#include <QDialog>
#include <QtCore/QtGlobal>
#include <QIODevice>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QList>
#include <iterator>

#include "instrumentdata.h"
#include "mainwindow.h"

namespace Ui {
class MoistureDialog;
}

class MoistureDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MoistureDialog(QWidget *parent = 0);
    ~MoistureDialog();

    bool loadFile(const QString &fileName);
    QList< ::InstrumentData> combinedData;
    int list_iterator;
signals:
    void btnEnterClick();
    void btnFinishClick();

private slots:
    void enterData();
    void finish();
    void forward();
    void backward();
    void cancel();

private:
    Ui::MoistureDialog *ui;
    void initActionsConnections();
    QList<QString> inputData;

    void updateDialog();

    static qint64 maxLineLength(void){ return(35);};
    static double maxPercent(void){return(30.0);};
    static double minPercent(void){return(0.0);};
    static int percentPrecision(void){return(2);};
    static int convertCountingNumbers(void){return(1);};
    static QString dummyPercent(void){return(" xxx.x");};

};
#endif // MOISTUREDIALOG_H
