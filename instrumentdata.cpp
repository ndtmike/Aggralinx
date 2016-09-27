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
 * parses and constructs the object with a string input
 * is there a problem after Localization?  Needs testing
 * Added QLocale to constructor
 */

InstrumentData::InstrumentData(const QString& dataIn, QLocale locale, QString dateformat, QString timeformat)
{

    rawInput = dataIn;
    QTextStream textin(&rawInput);
    QString word;

    while( textin.status() == QTextStream::Ok) {
            textin >> word;
            word = cleanWord(word);
            if(word != "" && word != "Stone") Words.push_back(word); //special case stuff
    }

    CurrentLocale = locale;
    DateFormat = dateformat;
    TimeFormat = timeformat;

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
      QString r;
      foreach (QString v, Words) {
         if(v == QString("Direct")|| //cases uploaded by unit
            v == QString("Sand")||
            v == QString("Gravel")||
            v == QString("Cr.")||
            v == QObject::tr("Direct")|| //cases translated from file
            v == QObject::tr("Sand")||
            v == QObject::tr("Gravel")||
            v == QObject::tr("Cr.")){
             r = v;
             break;
         }
      }
      return r;
}

/*
 * method to get raw percentage string
 *
 * returns "no percentage" if no percentage string
 */

QString InstrumentData::rawPercentage()
{
    bool ok = false;
    bool second = false;
    QString r = QString("no percentage");  //untranslated flag

    foreach(QString v, Words){
        v.toDouble( &ok );
        if(ok == true && second == true ){//found second valid double
            r = v;
            break;
        }
        if(ok == true && second == false){//found the first valid double
            second = true;
        }
    }
    return(r);
}

/*
 * method to get raw reading string
 */

QString InstrumentData::rawReading()
{
    bool ok = false;
    QString r;

    foreach(QString v, Words){
        v.toDouble( &ok );
        if(ok == true){
            r = v;
            break; //found the first valid double
        }
    }

    return(r);
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

    s = rawPercentage();

    if( s == QString("no percentage")){
        r=-1.0;
    }else{
        QTextStream ss(&s);
        ss>>r;
        if( ss.status() != QTextStream::Ok){
            r=0.0;
            QMessageBox::information(NULL,"Percentage","Bad Reading Data");
        }
    }
    return r;
}

/*
 * method to get reading as double
 */

double InstrumentData::readingToDouble()
{
    double r = 0.0;
    bool found = false;

    for(auto i = Words.begin();i != Words.end(); ++i ){
        r = i->toDouble(&found);
        if(found == true)break;
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

    QString materialstring = rawMaterial();

//    checking both local saved version and uploaded from unit

    if(materialstring == "Sand"||
       materialstring == QObject::tr("Sand")) material = Sand;
    if(materialstring == "Gravel"||
       materialstring == QObject::tr("Gravel")) material = Gravel;
    if(materialstring == "Cr." ||
       materialstring == QObject::tr("Cr.")) material = Crushed_Stone;
    if(materialstring == "Direct" ||
       materialstring == QObject::tr("Direct")) material = Direct;

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

    if(output.isValid() == false){//not from an upload
        for(auto i = Words.begin(); i != Words.end();++i){
            output = QDate::fromString( *i, DateFormat);
            if(output.isValid() == true){
                break;
            }else{
                output = bad;
            }
        }
    }
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

    if(output.isValid() == false){//not from an upload
        if(TimeFormat.contains("AP")){
            for(auto i = Words.begin(), j = Words.begin(); i != Words.end();++i){
                if(*i == "AM"||*i == "PM"){
                    j = i;
                    --j;
                    QString temptime = *j;
                    temptime.append(' ');
                    temptime.append( *i );
                    qDebug()<<temptime;
                    output = QTime::fromString( temptime, TimeFormat);
                    output = output.isValid() == true? output : bad;
                    break;
                }else{
                    output = bad;
                }
            }
        }else{
            for(auto i = Words.begin(); i != Words.end();++i){
                output = QTime::fromString( *i, TimeFormat);
                if(output.isValid() == true){
                    break;
                }else{
                    output = bad;
                }
            }
        }
    }
    qDebug() << output;
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
