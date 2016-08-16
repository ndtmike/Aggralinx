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
    QVector <QString> Words;

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
//    QString buffer = rawInput.section(" ",rawDateIndex(),rawDateIndex());
    return Words[1];
}

QString InstrumentData::rawMaterial()
{
//    QString buffer = rawInput.section(" ",rawMaterialIndex(),rawMaterialIndex());
    return Words[3];
}

QString InstrumentData::rawPercentage()
{
    QString buffer = rawInput.section(" ",rawPercentageIndex(),rawPercentageIndex());
    return buffer;
}

QString InstrumentData::rawReading()
{
//    QString buffer = rawInput.section(" ",rawReadingIndex(),rawReadingIndex());
    return Words[2];
}

QString InstrumentData::rawTime()
{
//     QString buffer = rawInput.section(" ",rawTimeIndex(),rawTimeIndex());
     return Words[0];
}

void InstrumentData::updatePercentage(QString &percentage)
{
    rawInput = rawInput.left(upDatePercentageIndex())+' '+ percentage;
}

QDate InstrumentData::toQDate()
{
    QDate output;
    QString buffer = this->rawDate();
    output = QDate::fromString( buffer,"MM'/'DD'/'YYYY");
    return(output);
}

QDateTime InstrumentData::toQDateTime()
{
    return(QDateTime(this->toQDate(),this->toQTime()));
}

QTime InstrumentData::toQTime()
{
    QTime output;
    QString buffer = this->rawTime();
    output =QTime::fromString(buffer, "HH':'MM");

    return(output);
}
