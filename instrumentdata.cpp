/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: instrumentdata.cpp
**
** plots the data on a graph - aggralinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/
#include "instrumentdata.h"

InstrumentData::InstrumentData()
{
    rawInput = dummyData();
}

InstrumentData::InstrumentData(const QString& dataIn)
{

    rawInput = dataIn;
    qDebug()<<rawInput;

    QTextStream textin(&rawInput);
    QString word;

    while( textin.status() == QTextStream::Ok) {
            textin >> word;
            qDebug() << word;
            if(word != "") Words.push_back(word);
    }
}

InstrumentData::~InstrumentData()
{

}

QString InstrumentData::rawDate()
{
      return Words[1];
}

QString InstrumentData::rawMaterial()
{
      return Words[3];
}

QString InstrumentData::rawPercentage()
{
    QString buffer = rawInput.section(" ",rawPercentageIndex(),rawPercentageIndex());
    return buffer;
}

QString InstrumentData::rawReading()
{
    return Words[2];
}

QString InstrumentData::rawTime()
{
     return Words[0];
}

void InstrumentData::updatePercentage(QString &percentage)
{
    rawInput = rawInput.left(upDatePercentageIndex())+' '+ percentage;
}

double InstrumentData::readingToDouble()
{
    double r;
    QString s = Words[2];
    QTextStream ss(&s);
    ss>>r;
    if( ss.status() != QTextStream::Ok){
        r=0.0;
        QMessageBox::information(NULL,"Data","Bad Reading Data");
    }
    return r;
}

QDate InstrumentData::toQDate()
{
    QDate output, bad;

    bad = QDate::fromString("01/01/0001","MM'/'dd'/'yyyy");

    QString buffer = rawDate();

    output = QDate::fromString( buffer,"MM'/'dd'/'yy");
    output = output.year() < 2016 ? output.addYears(100):output; //stops date being 1900's

    output = (output.isValid() == true)? output : bad; //returns all 1's if bad data in

    qDebug() << buffer << ' ' << output.toString();
    return(output);
}

QDateTime InstrumentData::toQDateTime()
{
    return(QDateTime(this->toQDate(),this->toQTime()));
}

QTime InstrumentData::toQTime()
{
    QTime output, bad;

    bad = QTime::fromString("00:00","hh':'mm"); //for bad data in string field

    QString buffer = rawTime();
    output = QTime::fromString(buffer, "hh':'mm");

    output = (output.isValid() == true)? output : bad; //returns zero if bad data

    return(output);
}
