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

    QTextStream textin(&rawInput);
    QString word;

    while( textin.status() == QTextStream::Ok) {
            textin >> word;
            word = cleanWord(word);
            if(word != "" && word != "Stone") Words.push_back(word); //special case stuff
    }

    TestDate = toQDate();
    TestTime = toQTime();
    TestDateTime = toQDateTime();
    TestMaterial = toMaterial();
    TestReading = readingToDouble();
    TestPercentage = percentageToDouble();
}

InstrumentData::~InstrumentData()
{

}

QString InstrumentData::cleanWord(QString data)
{
    QString out;
    QChar c;

    for(auto i = data.begin(); i != data.end(); ++i){
        c = *i;
        if(c.isLetterOrNumber()||c.isPunct()) out.append(c);
    }

    return out;
}

bool InstrumentData::isMaterialDirect()
{
    bool out;
    out = toMaterial() == Direct ? true : false;

    return(out);
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
    QString out;
    if(TestMaterial == Direct && Words.size() == 5)
        out = Words[5];
    return out;
}

QString InstrumentData::rawReading()
{
    return Words[2];
}

QString InstrumentData::rawTime()
{
     return Words[0];
}

double InstrumentData::percentageToDouble()
{
    double r;
    QString s;

    if(Words.size() == 5){
        s = Words[4];
    }else {
        s = "-1.0";
    }

    QTextStream ss(&s);
    ss>>r;
    if( ss.status() != QTextStream::Ok){
        r=0.0;
        QMessageBox::information(NULL,"Percentage","Bad Reading Data");
    }

    return r;
}

double InstrumentData::readingToDouble()
{
    double r;
    QString s = Words[2];
    QTextStream ss(&s);

    ss>>r;
    if( ss.status() != QTextStream::Ok){
        r=0.0;
        QMessageBox::information(NULL,"Reading","Bad Reading Data");
    }

    return r;
}

InstrumentData::Material InstrumentData::toMaterial()
{
    Material material = Bad_Data;

    if(rawMaterial() == "Sand") material = Sand;
    if(rawMaterial() == "Gravel") material = Gravel;
    if(rawMaterial() == "Cr.") material = Crushed_Stone;
    if(rawMaterial() == "Direct") material = Direct;

    return(material);
}

QDate InstrumentData::toQDate()
{
    QDate output, bad;

    bad = QDate::fromString("01/01/0001","MM'/'dd'/'yyyy");

    QString buffer = rawDate();

    output = QDate::fromString( buffer,"MM'/'dd'/'yy");
    output = output.year() < 2016 ? output.addYears(100):output; //stops date being 1900's

    output = (output.isValid() == true)? output : bad; //returns all 1's if bad data in

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

bool InstrumentData::updateTestPercent(const double &value){

    bool result;

    if(value > 0.0 && value < 100.0){
        TestPercentage = value;
        result = true;
    }else{
        result = false;
    }
    return result;
}
