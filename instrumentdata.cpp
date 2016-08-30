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

/*
 * Blind Constructor with no input
*/

InstrumentData::InstrumentData()
{
    rawInput = dummyData();
}

/*
 * parses and constructs the object with a string input
 * is there a problem after Localization?  Needs testing
 */

InstrumentData::InstrumentData(const QString& dataIn)
{

    rawInput = dataIn;

    qDebug()<< dataIn;

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

/*
 * Destructor
 */
InstrumentData::~InstrumentData()
{

}

/*
 * Removes bad characters
 */
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

/*
 * Sets boolean parameter for graphing
 */

bool InstrumentData::isMaterialDirect()
{
    bool out;
    out = toMaterial() == Direct ? true : false;

    return(out);
}

/*
 * method to get percent parameter
 */

double InstrumentData::getPercent()
{
    return(TestPercentage);
}

/*
 * method to get raw date string
 */

QString InstrumentData::rawDate()
{
      return Words[1];
}

/*
 * method to get raw date string
 */

QString InstrumentData::rawMaterial()
{
      return Words[3];
}

/*
 * method to get raw percentage string
 */

QString InstrumentData::rawPercentage()
{
    QString out;
    if(TestMaterial == Direct && Words.size() == 5)
        out = Words[5];
    return out;
}

/*
 * method to get raw reading string
 */

QString InstrumentData::rawReading()
{
    return Words[2];
}

/*
 * method to get raw time string
 */

QString InstrumentData::rawTime()
{
     return Words[0];
}

/*
 * method to get percentage as double
 */

double InstrumentData::percentageToDouble()
{
    double r;
    QString s;

    if(Words.size() == 6){
        s = Words[5];
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

/*
 * method to get reading as double
 */

double InstrumentData::readingToDouble()
{
    double r;
    QString s = Words[4];
    QTextStream ss(&s);

    ss>>r;
    if( ss.status() != QTextStream::Ok){
        r=0.0;
        QMessageBox::information(NULL,"Reading","Bad Reading Data");
    }

    return r;
}

/*
 * method to get material as enum
 * will probably need to save material as enum value
 */

InstrumentData::Material InstrumentData::toMaterial()
{
    Material material = Bad_Data;

    if(rawMaterial() == "Sand") material = Sand;
    if(rawMaterial() == "Gravel") material = Gravel;
    if(rawMaterial() == "Cr.") material = Crushed_Stone;
    if(rawMaterial() == "Direct") material = Direct;

    return(material);
}

/*
 * gets QDate Object
 * needs to be tested for other locals
 * when saved and restored
 */

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

/*
 * combines QDate and QTime to QDateTime object
 */

QDateTime InstrumentData::toQDateTime()
{
    return(QDateTime(this->toQDate(),this->toQTime()));
}

/*
 * gets QTime Object
 * needs to be tested for other locals
 * when saved and restored
 */

QTime InstrumentData::toQTime()
{
    QTime output, bad;

    bad = QTime::fromString("00:00","hh':'mm"); //for bad data in string field

    QString buffer = rawTime();
    output = QTime::fromString(buffer, "hh':'mm");

    output = (output.isValid() == true)? output : bad; //returns zero if bad data

    return(output);
}

/*
 * Enter the test percent in parameters
 */
bool InstrumentData::updateTestPercent(const double &value)
{

    bool result;

    if(value > 0.0 && value < 100.0){
        TestPercentage = value;
        result = true;
    }else{
        result = false;
    }
    return result;
}
