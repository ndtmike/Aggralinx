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
    setCentralWidget(console);
    serial = new QSerialPort(this);
    moistureData = new MoistureDialog(this);
    serialTimeOut = new QTimer(this);
    plot = new DataPlot(this);

    saveFileName = "";
    initActionsConnections();

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(serialTimeOut,SIGNAL(timeout()), this,SLOT(endUpload()));
#ifndef QT_DEBUG
    QTimer* init_timer = new QTimer(this);
    init_timer->singleShot(100, this, SLOT(showSplash()));
#else
#ifdef TEST_REG
    QTimer* init_timer = new QTimer(this);
    init_timer->singleShot(100, this, SLOT(showSplash()));
#else
    QTimer* init_timer = new QTimer(this);
    init_timer->singleShot(100, this, SLOT(loadExampleFile()));
#endif
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

void MainWindow::about()
{
    QString s;
    QTextStream toabout(&s);

    toabout << tr("The <b>Aggralinx Software</b> Version 3.1 is used with the <br>") <<
               tr("<b><i>James Instruments Inc.</i></b> Aggrameter T-T-100.<br><br>")<<
               tr("USA: +1773.4636565<br>")<<
               tr("Europe: +31.548.659032<br>")<<
               tr("Email: <a href=\"mailto:info@ndtjames.com?Subject=Aggrameter\" target=\"_top\">info@ndtjames.com</a><br>")<<
               tr("Web: <a href=\"http://www.ndtjames.com\">http://www.ndtjames.com</a><br>")<<
               tr("<br>Copyright 2016<br><br>")<<
               tr("Aggralinx is based in part on the work of<br> the <a href=\"http://qwt.sf.net\"> Qwt project (http://qwt.sf.net)</a>,<br>")<<
               tr("the <a href=\"http://www.qt.io\">QT project (http://www.qt.io) </a>,<br>")<<
               tr("and the <a href=\"http://www.mingw.org/\">MinGW Project (http://www.mingw.org)</a>");

    QMessageBox::information(this, tr("About Aggrameter"), s);
}

bool MainWindow::checkSerialPort()
{
    QString description;
    QString manufacturer;
    QString portname;
    const QString portmanufacturer = "FTDI";
    const QString noport = tr("No Available Ports"
                              "\nCheck instrument is plugged in"
                              "\nor serial port installed properly"
                              "\n then restart Aggralinx");
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

void MainWindow::cleanData()
{
    Parser* p = new Parser(rdFile(), tFile());
    delete p;
    ui->actionMoisture->setEnabled(true);
}

QString MainWindow::createDataLine(QVector<InstrumentData>::Iterator i){

    QString displaystring;
    QTextStream display(&displaystring);

    QDateTime idt = i->toQDateTime();

    QString matstr;
    InstrumentData::Material mat = i->toMaterial();

    switch (mat){
        case InstrumentData::Sand:
            matstr = tr("Sand");
            break;
        case InstrumentData::Gravel:
            matstr = tr("Gravel");
            break;
        case InstrumentData::Crushed_Stone:
            matstr = tr("Crushed Stone");
            break;
        case InstrumentData::Direct:
            matstr = tr("Direct");
            break;
        case InstrumentData::Bad_Data:
            matstr = tr("Bad Data");
            break;
    }

    QString reading;
    reading = QString("%L1").arg(i->readingToDouble(),0,'f',1);
    reading = i->isMaterialDirect()? reading : reading.append( QString('%'));

    QString percent;
    percent = i->getPercent() == -1.0 ? "": QString("%L1").arg(i->getPercent(),0,'f',1) + '%';

    display<<idt.toString(Qt::DefaultLocaleShortDate)<<' '
           <<matstr<<' '
           <<reading<<' '
           <<percent<<'\n';

    return(displaystring);
}

void MainWindow::copy()
{
    console->selectAll();
    console->copy();
}

void MainWindow::closeEvent (QCloseEvent* /*event*/)
{
    plot->close();
}

void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    console->setEnabled(false);
    ui->statusBar->showMessage(tr("Disconnected"));
}

void MainWindow::displayInstData()
{
    QString displaystring;

    for( QVector<InstrumentData>::Iterator i = InstDataVector.begin();
        i != InstDataVector.end(); ++i){

        displaystring += createDataLine(i);

    }
    console->appendPlainText(displaystring);
/*
    displaystring.clear();
    display<<"German"<<'\n';

    QLocale::setDefault(QLocale(QLocale::German,QLocale::Germany));
    QLocale german;

    for( auto i = InstDataVector.begin();
        i != InstDataVector.end(); ++i){

        displaystring += createDataLine(i);
    }
    console->appendPlainText(displaystring);

    displaystring.clear();
    display<<"Spain"<<'\n';

    QLocale::setDefault(QLocale(QLocale::Spanish,QLocale::Spain));
    QLocale spain;4

    for( auto i = InstDataVector.begin();
        i != InstDataVector.end(); ++i){
        displaystring += createDataLine(i);
    }
    console->appendPlainText(displaystring);

    displaystring.clear();
    display<<"Mexico"<<'\n';

    QLocale::setDefault(QLocale(QLocale::Spanish,QLocale::Mexico));
    QLocale mexico;

    for( auto i = InstDataVector.begin();
        i != InstDataVector.end(); ++i){
        displaystring += createDataLine(i);
    }
    console->appendPlainText(displaystring);

    QLocale::setDefault(QLocale::system());
*/
}
void MainWindow::dlgBack()
{

    if(dlgInstDataVectorIterator- InstDataVector.begin()>0){
        --dlgInstDataVectorIterator;
    }else{
        dlgInstDataVectorIterator = InstDataVector.end();
        --dlgInstDataVectorIterator; //end is one past crashes when pointed at
    }
    dlgUpdate();
}

void MainWindow::dlgEnter()
{
    QString percentstring;
    double percentdouble;
    bool ok = true;
    QTextStream percentstream(&percentstring);

    percentstring = moistureData->getPercent();
    percentstream >>percentdouble;

    qDebug()<< "String: "<< percentstring << "Double: "<< percentdouble;

    if(percentstream.status() != QTextStream::Ok){
        QMessageBox::information(this, tr("Enter"), tr("Bad Percent Data"));
        ok = false;
    }else{//all is good check range
        ok = dlgInstDataVectorIterator->updateTestPercent(percentdouble);
        if(ok == false){
            QMessageBox::information(this,tr("Enter"),tr("Percent Data Out of Range"));
        }
    }
    qDebug() << "Percent: "<<dlgInstDataVectorIterator->getPercent()<<'\n';
    if(ok == true){//everything good update console
        console->setPlainText("");
        displayInstData();
        dlgForward();
    }

    moistureData->setFocus();
}

void MainWindow::dlgFinish()
{
    plot->createPoints( console->toPlainText() );
}

void MainWindow::dlgForward()
{
    if(InstDataVector.end() - dlgInstDataVectorIterator > 1){//end is one past crashes when pointed at
        ++dlgInstDataVectorIterator;
    }else{
        dlgInstDataVectorIterator = InstDataVector.begin();
    }
    dlgUpdate();
}

void MainWindow::dlgMoisture()
{
    dlgInstDataVectorIterator=InstDataVector.begin();

    dlgUpdate();
}

void MainWindow::dlgUpdate()
{
    QString testdatetime, testpercent;
    QTextStream testdatetimestream(&testdatetime);

    testdatetimestream << tr("Test Date and Time")<<'\n'
                       << dlgInstDataVectorIterator->toQDateTime().toString(Qt::DefaultLocaleShortDate)<<'\n'<<'\n'
                       << tr("Reading:")<<'\n'
                       << QString("%L1").arg(dlgInstDataVectorIterator->readingToDouble(),0,'f',1);

    testpercent = dlgInstDataVectorIterator->getPercent() == -1.0 ? "":
                 QString("%L1").arg(dlgInstDataVectorIterator->getPercent(),0,'f',1) + '%';

    moistureData->changedata(testdatetime, testpercent);
    moistureData->display();
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::help()
{
    QProcess* help = new QProcess(this);
    help->start("hh.exe Aggralinx.chm");
}

void MainWindow::endUpload()
{
    const QString outputfile = tFile();
    QFile file(outputfile);
    QString data;

    serialTimeOut->stop();
    QMessageBox::information(this, "endUpload", tr("Upload Complete"));
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    cleanData();
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream ReadFile(&file);
    data = ReadFile.readAll();
    loadData(data);  //loads InstDataVector
    displayInstData();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();
    ui->action_Save->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
    ui->action_Open->setEnabled(false);
    ui->actionMoisture->setEnabled(true);
    ui->actionPlot->setEnabled(true);
}

void MainWindow::initActionsConnections()
{
    ui->actionQuit->setEnabled(true);
    ui->actionMoisture->setEnabled(false);
    ui->actionPlot->setEnabled(false);
    ui->actionSaveAs->setEnabled(false);
    ui->action_Save->setEnabled(false);
    ui->action_Open->setEnabled(true);

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(help()));
    connect(ui->actionMoisture, SIGNAL(triggered()), this, SLOT(dlgMoisture()));
    connect(ui->actionPlot, SIGNAL(triggered()), this, SLOT(plotData()));
    connect(ui->action_Save, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(openFile()));

    connect(moistureData, SIGNAL(btnBackClick()), this, SLOT(dlgBack()));
    connect(moistureData, SIGNAL(btnEnterClick()), this, SLOT(dlgEnter()));
    connect(moistureData, SIGNAL(btnFinishClick()), this, SLOT(dlgFinish()));
    connect(moistureData, SIGNAL(btnForwardClick()), this, SLOT(dlgForward()));
}

void MainWindow::loadData(QString Data)
{
    QStringList datalines = Data.split( "\n", QString::SkipEmptyParts );
    foreach( QString line, datalines ) {
       InstrumentData newinstdata(line);
       InstDataVector.append(newinstdata);
    }
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

void MainWindow::openFile()
{
    QString fileName = "";
    QString data;
    fileName = QFileDialog::getOpenFileName(this,
        tr("Open Text File"), "", tr("Text FIles (*.txt)"));
    saveFileName = fileName;

    QFile file(saveFileName);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream load(&file);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    data = load.readAll();
    loadData(data);  //loads InstDataVector
    displayInstData();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();

    ui->actionMoisture->setEnabled(true);
    ui->action_Save->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
    ui->action_Open->setEnabled(false);
    ui->actionPlot->setEnabled(true);
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

void MainWindow::plotData()
{
    dlgFinish();
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

void MainWindow::processSerialPort()
{
    foundSerialPort = checkSerialPort();
    if(foundSerialPort){openSerialPort();}
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

void MainWindow::save()
{
    if(saveFileName != ""){
        saveFile(saveFileName);
        QMessageBox::information(this,"Aggralinx",tr("File: ")+
                                 '\n' + saveFileName +
                                 '\n' + tr("Saved Successfully"));
    }else{
        saveAs();
    }
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
    return true;
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
/*
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
}
*/
void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}
