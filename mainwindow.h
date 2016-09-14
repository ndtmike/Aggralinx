/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/
/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: mainwindow.h
**
** Header file for mainwindow.h - aggralinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QtDebug>
#include <QDate>
#include <QDateTime>
#include <QFileDialog>
#include <QIODevice>
#include <QList>
#include <QLocale>
#include <QMainWindow>
#include <QMessageBox>
#include <QProcess>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QTime>
#include <QTimer>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <QPolygonF>

#include "console.h"
#include "splash.h"
#include "parser.h"
#include "moisturedialog.h"
#include "dataplot.h"
#include "instrumentdata.h"

#define TEST_REG


QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class MoistureDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QLocale* CurrentLocale;
    QString saveFileName;

protected:
    void closeEvent(QCloseEvent */*event*/) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *);

private slots:
    void about();
    void copy();
    void closeSerialPort();

    void dlgBack();
    void dlgEnter();
    void dlgFinish();
    void dlgForward();
    void dlgMoisture();

    void endUpload();
    void handleError(QSerialPort::SerialPortError error);
    void help();

    void LNGEnglish();
    void LNGDeutsche();
    void LNGEspanol();

    void openFile();
    void openSerialPort();
    void plotData();
    void processSerialPort();
    void readData();
    void save();
    bool saveAs();
    void showSplash();

private:

    bool checkSerialPort();
    QString createDataLine(QVector<InstrumentData>::Iterator i); //reads data form InstDataVector and returns local string
    QString createHeader();
    void displayInstData(); //display text from InstDataVector
    void dlgUpdate();
    bool foundSerialPort;
    void initActionsConnections();
    QVector <InstrumentData> InstDataVector; //loaded with each line uploaded for localization purposes
    QVector <InstrumentData> ::iterator dlgInstDataVectorIterator;
    void LNGLoadTranslator();  //after QLocale is chnaged string need to be loaded
    void loadData(QString Data);
    int posGetPos(QString& data, int line_number, bool begin);
    bool saveFile(const QString &fileName);
    QTranslator* Translator;
    void updateConsole(QString line, int line_number);

    Console *console;
    MoistureDialog *moistureData;
    DataPlot* plot;
    QSerialPort *serial;
    QTimer *serialTimeOut;
    Ui::MainWindow *ui;

    static const QString helpString(void){ return("hh.exe aggralinx.chm");};
};
#endif // MAINWINDOW_H
