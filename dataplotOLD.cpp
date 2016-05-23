/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: dataplot.cpp
**
** plots the data on a graph - aggralinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#include "dataplot.h"

DataPlot::DataPlot(QWidget *parent)
{
    Curve = new QwtPlotCurve;
    Grid = new QwtPlotGrid;
    Symbol = new QwtSymbol( QwtSymbol::Ellipse,
                QBrush( Qt::red ),
                QPen( Qt::red, 2 ),
                QSize( 8, 8 ) );

    SetPlotParameters();
    SetGridParameters();
    SetCurveParameters();
}

DataPlot::~DataPlot()
{
//delete symbol creates a problem.
    delete Curve;
    delete Grid;
}

void DataPlot::SetPlotParameters()
{
    setTitle( "Aggralinx" );
    setCanvasBackground( Qt::white );
    insertLegend( new QwtLegend() );
}

void DataPlot::SetGridParameters()
{
    Grid->attach(this);
}

void DataPlot::SetCurveParameters()
{
    Curve->setTitle( "Data Points" );
    Curve->setPen( Qt::red, 4 ),
//    Curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    Curve->setSymbol( Symbol );
}

void DataPlot::loadPoints(const QVector<QPointF>& points)
{
    Curve->setSamples( points );

    Curve->attach(this);

    resize( 600, 400 );
    replot();
    show();

}

void DataPlot::createPoints(const QString& rawdata)
{
    QString working = rawdata;
    QString line = "";

    for(int posendline = working.indexOf('\n',0);
        working.indexOf('\n',posendline) != endLineConst();
        posendline = working.indexOf('\n',0)){
        line = working.left( posendline);//end of line
        if(!loadPlotDataPoints(line)) return;
        working.remove(0, posendline+1);
     }
    loadPoints(plotDataPoints);
}

bool DataPlot::loadPlotDataPoints(const QString& line)
{
    bool success = false;
    QString reading = "";
    QString moisture = "";
    double dr = 0.0;
    double dm = 0.0;
    QPointF NewPoint;
    NewPoint.setX(0.0);
    NewPoint.setY(0.0);

    if(line.size() > lineSizeConst()){
        success = true;
        reading = line.mid(readingPosConst(),readingWidthConst());
        moisture = line.mid(moisturePosConst());
        dr = reading.toDouble();
        dm = moisture.toDouble();
        NewPoint.setX(dr);
        NewPoint.setY(dm);
        plotDataPoints.push_back(NewPoint);

    }else{
        QMessageBox::information(this,"Aggralinx",
            tr("Data Not Entered!"));
    }
    return(success);
}
