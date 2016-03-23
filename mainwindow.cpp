
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
** File Name: mainwindow.cpp
**
** mainwindow.cpp - Aggralinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    console = new Console;
    console->setEnabled(false);
    setCentralWidget(console);
    serial = new QSerialPort(this);
    moistureData = new MoistureDialog(this);
    serialTimeOut = new QTimer(this);

    plot = new DataPlot(this);

    ui->actionQuit->setEnabled(true);
    ui->actionMoisture->setEnabled(false);

    initActionsConnections();

    saveFileSwitch = true; //file not loaded, no problem to close
    saveFileName = "";
    ui->action_Save->setEnabled(false);
    ui->actionSaveAs->setEnabled(false);

    connect(moistureData, SIGNAL(btnEnterClick()), this, SLOT(dlgEnter()));
    connect(moistureData, SIGNAL(btnFinishClick()), this, SLOT(dlgFinish()));

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(serialTimeOut,SIGNAL(timeout()), this,SLOT(endUpload()));
#ifndef QT_DEBUG
    QTimer* init_timer = new QTimer(this);
    init_timer->singleShot(100, this, SLOT(showSplash()));
#else
    QTimer* init_timer = new QTimer(this);
    init_timer->singleShot(100, this, SLOT(loadExampleFile()));
#endif
}

MainWindow::~MainWindow()
{
    closeSerialPort();
    delete plot;
    delete ui;
    delete console;
    delete moistureData;
    delete serial;
}


void MainWindow::openSerialPort()
{
    serial->setBaudRate(9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadOnly)) {
            console->setEnabled(true);
            console->setLocalEchoEnabled( true);
            ui->statusBar->showMessage(tr("Connected"));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        ui->statusBar->showMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    console->setEnabled(false);
    ui->statusBar->showMessage(tr("Disconnected"));
}

void MainWindow::about()
{
    QString s;
    QTextStream toabout(&s);

    toabout << tr("The <b>Reboundlinx Software</b> is used with the <br>") <<
               tr("<b><i>James Instruments Inc.</i></b> Digital Rebound Hammers.<br><br>")<<
               tr("USA: +1773.4636565<br>")<<
               tr("Europe: +31.548.659032<br>")<<
               tr("Email: <a href=\"mailto:info@ndtjames.com?Subject=Reboundlinx\" target=\"_top\">info@ndtjames.com</a><br>")<<
               tr("Web: <a href=\"http://www.ndtjames.com\">http://www.ndtjames.com</a><br>");

    QMessageBox::information(this, tr("About Reboundlinx"), s);
}

void MainWindow::help()
{
    QProcess* help = new QProcess(this);
    help->start("hh.exe aggralinx.chm");
}

void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}

void MainWindow::readData()
{
    QFile file(rdFile());
    QTextStream out(&file);
    serialTimeOut->start(500);
    QByteArray data = serial->readAll();
    console->putData(data);
    if(!file.open(QIODevice::Append)){
        QMessageBox::information(this, "readData", tr("Cannot write rd.txt"));
    }else{
        out<<data;
    }
    file.close();
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::initActionsConnections()
{
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(help()));
    connect(ui->actionMoisture, SIGNAL(triggered()), this, SLOT(moisture()));
    connect(ui->actionPlot, SIGNAL(triggered()), this, SLOT(plotData()));
    connect(ui->action_Save, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(openFile()));
}

void MainWindow::cleanData()
{
    Parser* p = new Parser(rdFile(), tFile());
    delete p;
    ui->actionMoisture->setEnabled(true);
}

void MainWindow::processSerialPort()
{
    foundSerialPort = checkSerialPort();
    if(foundSerialPort){openSerialPort();}
}

void MainWindow::showSplash()
{
    const int five_sec = 5000;

    SplashDialog* splash = new SplashDialog();
    splash->setModal( true );
    splash->show();

    QTimer* splash_timer = new QTimer(this);
    splash_timer->singleShot(five_sec, this, SLOT(processSerialPort()));
}

bool MainWindow::checkSerialPort()
{
    QString description;
    QString manufacturer;
    QString portname;
    const QString portmanufacturer = "FTDI";
    const QString noport = tr("No Available Ports\nCheck instrument is plugged in\nor serial port installed properly\n then restart Reboundlinx");
    const QString messageTitle = "checkSerialPort";
    const QString connected = tr("Connected to ");
    QList <QSerialPortInfo> availablePorts;
    bool r = false;
    availablePorts = QSerialPortInfo::availablePorts();

    if(!availablePorts.isEmpty()){
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            portname = info.portName();
            description = info.description();
            manufacturer = info.manufacturer();
            if(manufacturer == portmanufacturer){
                serial->setPortName(portname);
                r = true;
            }
        if( r == true )break;
        }
    }
    if(r == false){
        QMessageBox::information(this,messageTitle,noport);
    }else{
        QMessageBox::information(this ,messageTitle , connected + portname );
    }
    return(r);
}

#ifdef QT_DEBUG
void MainWindow::loadExampleFile()
{
    QFile file(exampleFile());
    QTextStream load(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    console->setPlainText("");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream ReadFile(&file);
    console->setPlainText(load.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();
    saveFile(rdFile());
    serialTimeOut->start(500);
}
#endif

void MainWindow::loadTemp()
{
    QFile file(tFile());
    QTextStream load(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    console->setPlainText("");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream ReadFile(&file);
    console->setPlainText(load.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();
    ui->actionMoisture->setEnabled(true);
}

void MainWindow::endUpload()
{
    const QString outputfile = tFile();
    QFile file(outputfile);

    serialTimeOut->stop();
    QMessageBox::information(this, "endUpload", tr("Upload Complete"));
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    cleanData();
    console->setPlainText("");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream ReadFile(&file);
    console->setPlainText(ReadFile.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();
    saveFileSwitch = false;//file loaded but not saved
    ui->action_Save->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
}


void MainWindow::openFile()
{
    QString fileName = "";
    fileName = QFileDialog::getOpenFileName(this,
        tr("Open Text File"), "", tr("Text FIles (*.txt)"));
    saveFileName = fileName;
    QFile file(saveFileName);
    QTextStream load(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    console->setPlainText("");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream ReadFile(&file);
    console->setPlainText(load.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();
    ui->actionMoisture->setEnabled(true);
    saveFileSwitch = false;//file loaded but not saved
    ui->action_Save->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
}

void MainWindow::save()
{
    if(saveFileSwitch == true){
        QMessageBox::information(this,"Aggralinx",
                                 tr("Nothing to Save!"));
        return;
    }
    if(saveFileName != ""){
        saveFile(saveFileName);
        QMessageBox::information(this,"Aggralinx",tr("File: ")+
                                 '\n' + saveFileName +
                                 '\n' + tr("Saved Successfully"));
    }else{
        saveAs();
    }
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "MainWindow",
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << console->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    saveFileSwitch = true; //saved the file
    return true;
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Text (*.txt)"));
    QStringList files;
    if (dialog.exec())
        files = dialog.selectedFiles();
    else
        return false;
    saveFileName = files.at(0);
    return saveFile(files.at(0));
}

void MainWindow::copy()
{
    console->selectAll();
    console->copy();
}

void MainWindow::moisture()
{
    saveFile(tFile());

    if(!(moistureData->loadFile(tFile()))){ //moisturedialog does not wait for input
            return ;
    }
    moistureData->show();
}

void MainWindow::plotData()
{
    dlgFinish();
}

void MainWindow::dlgEnter()
{
    QString line = "";
    int line_number = 0;

    if(moistureData->list_iterator > 0){
        line = moistureData->combinedData[moistureData->list_iterator-1].rawInput;
        line_number = moistureData->list_iterator;
    }else{
        line = moistureData->combinedData.last().rawInput;
        line_number = moistureData->combinedData.size();
    }

    updateConsole(line, line_number);

    moistureData->show();
    moistureData->activateWindow();
    moistureData->raise();
    moistureData->setFocus();
}

void MainWindow::dlgFinish()
{
    plot->createPoints( console->toPlainText() );
}

void MainWindow::updateConsole(QString line, int line_number)
{
    QString consoledata = console->toPlainText();
    int posbegin = posGetPos(consoledata, line_number, true);
    int posend = posGetPos(consoledata, line_number, false);
    consoledata = consoledata.remove(posbegin+1, posend-posbegin);
    int datasize = consoledata.size();
    if(line_number > 1){//first line doens't have new line character
        if(posend >= datasize){//neither does last line
            consoledata=consoledata+line;
        }else{
        consoledata.insert(posbegin+1,line + '\n');
        }
    }else{
        consoledata.insert(posbegin,line + '\n');
    }
    console->setPlainText("");
    console->setPlainText(consoledata);
    saveFileSwitch = false;
}

int MainWindow::posGetPos(QString& data, int line_number, bool begin)
{
    int posendline = 0;
    int posbeginline=0;
    QString working = data;

    for(int i = 0; i < line_number; ++i){
        posbeginline = posendline;
        posendline = working.indexOf('\n',posendline +1);//end of line
        if (posendline == -1){
            posendline = working.size();  //end of file
        }
    }
    if(begin == true){
        return(posbeginline);
    }else{
        return(posendline);
    }
}

void MainWindow::closeEvent (QCloseEvent* /*event*/)
{
    plot->close();
/*
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Aggralinx",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {

        event->accept();
    }
*/
}
