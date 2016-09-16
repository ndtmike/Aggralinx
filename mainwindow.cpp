
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

/*
 * Main window constructor
 * Loads ui
 *      console to display data
 *      serial port for communications
 *      dialog to enter moisture data
 *      timer to show splash screen
 *      window to show data that's plotted
 *
 *  - issue with plot not being raised and not getting focus
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    plot = new DataPlot(this);
    console = new Console;
    setCentralWidget(console);
    serial = new QSerialPort(this);
    moistureData = new MoistureDialog(this);
    serialTimeOut = new QTimer(this);

    CurrentLocale = QLocale::system(); //standardized number strings
    QString systemlocale = CurrentLocale.name();
    CurrentLocale = QLocale(systemlocale);
    QLocale::setDefault(CurrentLocale);
    Translator = new QTranslator(this);

    saveFileName = "";
    initActionsConnections();
    QTimer* init_timer = new QTimer(this);
    init_timer->singleShot(100, this, SLOT(showSplash()));
    delete init_timer;
}

/*
 * Main Destructor deletes all objects
 */

MainWindow::~MainWindow()
{
    delete plot;
    delete ui;
    delete console;
    delete moistureData;
    delete serial;
}

/*
 * Displays details regarding software
*/

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

/*
 * Checks for existance of serial port, not tested with linux
*/

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

/*
 * after data is loaded it is turned into a vector of
 * InstrumentData objects.
 * Set up as group of numbers that is 'localized'
 * enum for material needs to be checked
 */

QString MainWindow::createDataLine(QVector<InstrumentData>::Iterator i){

    QString displaystring;
    QTextStream display(&displaystring);
    QString percent;

//    QDateTime idt = i->toQDateTime();
    QTime it = i->toQTime(); //issues with order of QDate time and number format
    QDate id = i->toQDate();

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
            percent = i->getPercent() == -1.0 ? "": QString("%L1").arg(i->getPercent(),0,'f',1) +' '+ '%';
            break;
        case InstrumentData::Bad_Data:
            matstr = tr("Bad Data");
            break;
    }

    QString reading;
    reading = QString("%L1").arg(i->readingToDouble(),0,'f',1);
    reading = i->isMaterialDirect()? reading : reading.append( QString('%'));

    display<<it.toString(CurrentLocale.timeFormat(QLocale::ShortFormat))<<' '
           <<id.toString(CurrentLocale.dateFormat(QLocale::ShortFormat))<<' '
 //           <<CurrentLocale.name()<<' '
           <<matstr<<' '
           <<reading<<' '
           <<percent<<endl;

    return(displaystring);
}

QString MainWindow::createHeader()
{
    QString r;
    QTextStream out(&r);

    QDateTime current = QDateTime::currentDateTime();

    out<< "James Instruments Inc." << '\n'
       << "Aggrameter" << '\n'
       << "Copyright 2016" << '\n'
       << tr("Report Created On: ")
       << CurrentLocale.toString( current, CurrentLocale.dateTimeFormat(QLocale::ShortFormat))//time report created
       << '\n'
       <<CurrentLocale.name()<<'\n';

    return r;
}

/*
 * allows copy for copy and paste
 */

void MainWindow::copy()
{
    console->selectAll();
    console->copy();
}

/*
 * closes the data plot when application closes
 */

void MainWindow::closeEvent (QCloseEvent* /*event*/)
{
    plot->close();
}

/*
 * changes text of Menu System
*/

void MainWindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
    }else{
        QMainWindow::changeEvent(e);
    }
}

/*
 * closes the serial port when application closes
 */

void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    console->setEnabled(false);
    ui->statusBar->showMessage(tr("Disconnected"));
}


/*
 * outputs InstrumentData vector to console in localized form
 */

void MainWindow::displayInstData()
{
    QString displaystring;

    for( QVector<InstrumentData>::Iterator i = InstDataVector.begin();
        i != InstDataVector.end(); ++i){
        displaystring += createDataLine(i);
    }
    console->appendPlainText(displaystring); //replace with formatted data
}

QString MainWindow::displayHeader(QString d)
{
    QString r;
    QStringList h;

    h = d.split('\n', QString::SkipEmptyParts);
    for(quint8 i = 0; i<5; ++i ){
        console->appendPlainText(h.at(0));
        if( i == 4){
            CurrentLocale = QLocale(h.at(0));
            QLocale::setDefault(CurrentLocale);
            LNGLoadTranslator();
        }

        h.removeFirst();
    }
    r = h.join('\n');

    return(r);
}

/*
 * moves through InstrumentData Vector when
 * back button pushed
 */

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

/*
 * When enter button pressed on moisture dialog
 * update percentage string with InstrumentData
 * Vector and display same
 */

void MainWindow::dlgEnter()
{
    QString percentstring;
    double percentdouble;
    bool ok = true;
    QTextStream percentstream(&percentstring);

    percentstring = moistureData->getPercent();
    percentstream >>percentdouble;

    if(percentstream.status() != QTextStream::Ok){
        QMessageBox::information(this, tr("Enter"), tr("Bad Percent Data"));
        ok = false;
    }else{//all is good check range
        ok = dlgInstDataVectorIterator->updateTestPercent(percentdouble);
        if(ok == false){
            QMessageBox::information(this,tr("Enter"),tr("Percent Data Out of Range"));
        }
    }
    if(ok == true){//everything good update console
        console->setPlainText("");
        displayInstData();
        dlgForward();
    }

    moistureData->setFocus();
}

/*
 * All data entered and data and
 * regression values need to calculated
 */

void MainWindow::dlgFinish()
{
    plot->createPoints( console->toPlainText() );
    plot->show();
}

/*
 * moves through InstrumentData Vector when
 * back button pushed
 */

void MainWindow::dlgForward()
{
    if(InstDataVector.end() - dlgInstDataVectorIterator > 1){//end is one past crashes when pointed at
        ++dlgInstDataVectorIterator;
    }else{
        dlgInstDataVectorIterator = InstDataVector.begin();
    }
    dlgUpdate();
}

/*
 * initially display constructor
 */

void MainWindow::dlgMoisture()
{
    dlgInstDataVectorIterator=InstDataVector.begin();

    dlgUpdate();
}

/*
 * loading moisture dialog with
 * data from InstrumentData vector
 */

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

/*
 * After all data is uploaded and timer times out
 * this function gets called
 * saves data to temp filed
 * Stops timer (one upload at a time)
 * loads data in InstrumentData vector
 * sets parameters that we NOW HAVE DATA
 */

void MainWindow::endUpload()
{
    QString data;

    serialTimeOut->stop();
    delete serialTimeOut;
    QMessageBox::information(this, "endUpload", tr("Upload Complete"));
    data = console->toPlainText();
    console->setPlainText(""); //erase uploaded data
    console->setPlainText(createHeader());
    loadData(data);  //loads InstDataVector
    displayInstData();

    ui->action_Save->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
    ui->action_Open->setEnabled(false);
    ui->actionMoisture->setEnabled(true);
    ui->actionPlot->setEnabled(true);
    ui->actionMoisture->setEnabled(true);
}

/*
 * Catch serial port errors
 */

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

/*
 * Display help file
 */

void MainWindow::help()
{
    QProcess* help = new QProcess(this);
    help->start("hh.exe Aggralinx.chm");
}


/*
 * creates actions and connections for program flow
 */

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
    connect(ui->actionEnglish, SIGNAL(triggered()), this, SLOT(LNGEnglish()));
    connect(ui->actionDeutsche, SIGNAL(triggered()), this, SLOT(LNGDeutsche()));
    connect(ui->actionEspanol, SIGNAL(triggered()), this, SLOT(LNGEspanol()));

    connect(moistureData, SIGNAL(btnBackClick()), this, SLOT(dlgBack()));
    connect(moistureData, SIGNAL(btnEnterClick()), this, SLOT(dlgEnter()));
    connect(moistureData, SIGNAL(btnFinishClick()), this, SLOT(dlgFinish()));
    connect(moistureData, SIGNAL(btnForwardClick()), this, SLOT(dlgForward()));

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(serialTimeOut,SIGNAL(timeout()), this,SLOT(endUpload()));
}


/*
 * sets translator object to German
 */
void MainWindow::LNGDeutsche()
{

    CurrentLocale = QLocale(QLocale::German);
    QLocale::setDefault(CurrentLocale);
    LNGLoadTranslator();
}

/*
 * sets translator object to English
 */
void MainWindow::LNGEnglish()
{
    CurrentLocale =QLocale(QLocale::English);
    QLocale::setDefault(CurrentLocale);
    LNGLoadTranslator();
}

/*
 * sets translator object to Spanish
 */
void MainWindow::LNGEspanol()
{
    CurrentLocale = QLocale(QLocale::Spanish);
    QLocale::setDefault(CurrentLocale);
    LNGLoadTranslator();
}

/*
 *loads translator  object
*/
void MainWindow::LNGLoadTranslator()
{
    if(Translator->isEmpty()){
            Translator->load(CurrentLocale.language(), "Internationalization","_");
            qApp->installTranslator(Translator);
    }else{
        qApp->removeTranslator(Translator);
        Translator->load(CurrentLocale.language(), "Internationalization","_");
        qApp->installTranslator(Translator);
    }
}

/*
 * Loads InstrumentData Vector with each line
 */

void MainWindow::loadData(QString Data)
{
    serial->close();

    QStringList datalines = Data.split( "\n", QString::SkipEmptyParts );

    foreach( QString line, datalines ) {
        InstrumentData newinstdata(line, CurrentLocale,
            CurrentLocale.dateFormat(QLocale::ShortFormat),
            CurrentLocale.timeFormat(QLocale::ShortFormat));
        InstDataVector.append(newinstdata);
    }
}

/*
 * loads data from a file
 */

void MainWindow::loadDataFile(QString Data)
{
    serial->close();
    QStringList datalines = Data.split( "\n", QString::SkipEmptyParts );

    foreach( QString line, datalines ) {
        InstrumentData newinstdata(line, CurrentLocale,
            CurrentLocale.dateFormat(QLocale::ShortFormat),
            CurrentLocale.timeFormat(QLocale::ShortFormat));
        InstDataVector.append(newinstdata);
    }
}

/*
 * Open file dialog to load data from hard disk
 */

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
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    file.close();
    console->setPlainText("");
    data = displayHeader(data);
    loadDataFile(data);  //loads InstDataVector
    displayInstData();

    ui->actionMoisture->setEnabled(true);
    ui->action_Save->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
    ui->action_Open->setEnabled(false);
    ui->actionPlot->setEnabled(true);
    ui->actionMoisture->setEnabled(true);
}

/*
 * after serial port is found open it to
 * receive data
 */

void MainWindow::openSerialPort()
{
    serial->setBaudRate(9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadOnly)) {
            console->setEnabled(true);
            ui->statusBar->showMessage(tr("Connected"));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        ui->statusBar->showMessage(tr("Open error"));
    }
}

/*
 * method plots data from menu
 */

void MainWindow::plotData()
{
    dlgFinish();
}

/*
 * small function to make sure we have a serial port
 * before we open it.
 */

void MainWindow::processSerialPort()
{
    foundSerialPort = checkSerialPort();
    if(foundSerialPort){openSerialPort();}
}

/*
 * Reads Data from serial port
 */

void MainWindow::readData()
{
    serialTimeOut->start(500);
    QByteArray data = serial->readAll();
    console->putData(data);
}

/*
 * Saves data in Console to a file
 */

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

/*
 * Save As asks for filename
 */

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

/*
 * Streams data from Console to file
 */

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

/*
 * Shows splash screen for certain period of time
 */

void MainWindow::showSplash()
{
    const int five_sec = 5000;

    SplashDialog* splash = new SplashDialog();
    splash->setModal( true );
    splash->show();

    QTimer* splash_timer = new QTimer(this);
    splash_timer->singleShot(five_sec, this, SLOT(processSerialPort()));
    delete splash_timer;
}
