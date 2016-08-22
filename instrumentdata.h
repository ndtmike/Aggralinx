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

#include <QtCore/QtGlobal>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QDialog>
#include <QIODevice>
#include <QMessageBox>
#include <QString>
#include <QTextStream>
#include <QTime>

class InstrumentData
{
public:

    enum Material{ Sand, Gravel, Crushed_Stone, Direct, Bad_Data };

    InstrumentData();
    InstrumentData(const QString& dataIn);
    ~InstrumentData();

    bool isMaterialDirect();
    double percentageToDouble(); //calculated percentage always get percent sign
    double readingToDouble(); // from instruments gets percent sign when not direct, call isMaterialDirect()
    Material toMaterial();
    QDate toQDate();
    QDateTime toQDateTime();
    QTime toQTime();
    bool updateTestPercent(const double &value);

private:

    QDate TestDate;
    QDateTime TestDateTime;
    Material TestMaterial;
    double TestPercentage;
    double TestReading;
    QTime TestTime;


    QString cleanWord(QString data);
    QString rawDate();
    QString rawInput;
    QString rawMaterial();
    QString rawPercentage();
    QString rawReading();
    QString rawTime();
    QVector <QString> Words;

    static QString dummyData(void){return("00:00 00/00/00 000.0 Dummy xx.x");};
//    static int rawDateIndex(void){return(1);};
//    static int upDatePercentageIndex(void){return(27);};
//    static int rawPercentageIndex(void){return(4);};
};

#endif // INSTRUMENTDATA_H
