/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: parser.cpp
**
** parser.cpp - aggralinx software
** Divides the input stream up into data packets
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#include "parser.h"

Parser::Parser()
{
    QString df = QFileDialog::getOpenFileName(this); //not sure if this working?
    eraseTempFile();
    dataFile = df;
    if(!readInputData()){ //check that string passed is valid
        return;
    }
    if(!writeOutputData()){ //check that string passed is valid
        return;
    }
}

Parser::Parser( const QString &df )
{
    dataFile = df;
    outputFile = tFile();
    eraseTempFile();
    if(!readInputData()){ //check that string passed is valid
        return;
    }
    if(!writeOutputData()){ //check that string passed is valid
        return;
    }

}

Parser::Parser( const QString &df, const QString &of )
{
    dataFile = df;
    outputFile = of;
    eraseTempFile();
    if(!readInputData()){ //check that string passed is valid
        return;
    }
    if(!writeOutputData()){ //check that string passed is valid
        return;
    }
}

Parser::~Parser()
{
}

bool Parser::readInputData()
{
    QFile file(dataFile);
    QTextStream in(&file);
    QString linebuf,dataappend;
    bool ifs = true;

    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(this, "readInputData", tr("fail"));
        ifs = false;
    }else{
        ifs = true;
        while( !in.atEnd())
        {
            linebuf=in.readLine(maxLineLength());
            if(linebuf.size()>minLineLength()){
                dataappend = cleanLine(linebuf);
                inputData.append(linebuf);
            }
        }
   }
   file.close();
   return(ifs);
}

bool Parser::writeOutputData(){
    bool r = false;

    //Setup Output data
    QFile file(outputFile);
    QTextStream opl(&file);

    if(!file.open(QIODevice::WriteOnly|QIODevice::Append)){
        QMessageBox::information(this, "writeDataPacket", tr("fail"));
        r = false;
    }else{
        r = true;
        for(qint64 x= 0; x< inputData.size();++x){
            opl<<inputData.at(x)<<'\n';
        }
     }
    file.close();
    return r;
}

void Parser::eraseTempFile()
{
    QFile file(outputFile);
    file.remove();
}

QString Parser::cleanLine(QString& line)
{
    QString working=line;

    for(int i = 0; i < line.size(); ++i){
       if((!(line[i]>='0'&&line[i]<='9'))&&(!(line[i]>='A'&& line[i]<='Z'))&&
           (!(line[i]>='a'&& line[i]<='z'))&&
           line[i]!=':'&&line[i]!='/'&&line[i]!=' '&&line[i]!='.'){
           working = line.remove(i,1);
           --i;//set the counter back as you removed a character
        }
    }
    return(line);
}
