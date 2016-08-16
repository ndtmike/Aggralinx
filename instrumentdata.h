/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: instrumentdata.h
**
** Header file for instrumentdata.cpp - aggralinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#ifndef INSTRUMENTDATA_H
#define INSTRUMENTDATA_H
#include <QString>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include <QMessageBox>
#include <QDialog>
#include <QtCore/QtGlobal>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>

class InstrumentData
{
public:
    InstrumentData();
    InstrumentData(const QString& dataIn);
    ~InstrumentData();

    QString rawDate();
    QString rawInput;
    QString rawMaterial();
    QString rawPercentage();
    QString rawReading();
    QString rawTime();
    void updatePercentage(QString& percentage);
    QDate toQDate();
    QDateTime toQDateTime();
    QTime toQTime();

private:
    QVector <QString> Words;

    static QString dummyData(void){return("00:00 00/00/00 000.0 Dummy xx.x");};
    static int rawDateIndex(void){return(1);};
    static int upDatePercentageIndex(void){return(27);};
    static int rawMaterialIndex(void){return(3);};
    static int rawPercentageIndex(void){return(4);};
    static int rawReadingIndex(void){return(2);};
    static int rawTimeIndex(void){return(0);};
};

#endif // INSTRUMENTDATA_H
