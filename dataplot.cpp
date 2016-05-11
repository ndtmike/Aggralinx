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

DataPlot::DataPlot( QWidget* /*parent*/ )
{
    createClasses();
    SetPlotParameters();
    SetGridParameters();
    SetCurveParameters();
    SetRCurveParameters();

    plotDataPoints<< QPointF (0.0,0.0);
}

DataPlot::~DataPlot()
{
//delete symbol creates a problem.
    delete Curve;
    delete Grid;
}

void DataPlot::createClasses()
{
    Curve = new QwtPlotCurve;
    Grid = new QwtPlotGrid;
    Symbol = new QwtSymbol( QwtSymbol::Ellipse,
                QBrush( Qt::red ),
                QPen( Qt::red, 2 ),
                QSize( 8, 8 ) );
    rCurve = new QwtPlotCurve;
    rSymbol = new QwtSymbol(QwtSymbol::Ellipse,
                            QBrush( Qt::red ),
                            QPen( Qt::red, 2 ),
                            QSize( 8, 8 ));
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
    Curve->setStyle(QwtPlotCurve::NoCurve);
    Curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    Curve->setSymbol( Symbol );
}

void DataPlot::SetRCurveParameters()
{
    rCurve->setStyle(QwtPlotCurve::Lines);
    rCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
}

void DataPlot::displayGraph(const QVector<QPointF>& points)
{
    Curve->setSamples( points );
    Curve->attach(this);

    createRegLine(plotDataPoints);

    resize( 600, 400 );
    replot();
    show();
}

void DataPlot::createPoints(const QString& rawdata)
{
    QString working = rawdata;
    QString line = "";
    plotDataPoints.clear();

    for(int posendline = working.indexOf('\n',0);
        working.indexOf('\n',posendline) != endLineConst();
        posendline = working.indexOf('\n',0)){
        line = working.left( posendline);//end of line
        if(!loadPlotDataPoints(line)) return;
        working.remove(0, posendline+1);
     }
    displayGraph(plotDataPoints);
}

bool DataPlot::loadPlotDataPoints(const QString& line)
{
    bool success = false;
    QString reading = "";
    QString moisture = "";
    double dr = 0.0;
    double dm = 0.0;

    if(line.size() > lineSizeConst()){
        success = true;
        reading = line.mid(readingPosConst(),readingWidthConst());
        moisture = line.mid(moisturePosConst());
        dr = reading.toDouble();
        dm = moisture.toDouble();
    }else{
        QMessageBox::information(this,"Aggralinx",
            tr("Data Not Entered!"));
    }
    plotDataPoints << QPointF(dr,dm);

    return(success);
}

void DataPlot::createRegLine(const QVector<QPointF>& in)
{
    QVector<QPointF> regLineData;
    Regression* analysis = new Regression();

    for(qint64 i = 0; i < in.size(); ++i){
        exp_data loaddata;
        loaddata.x = in[i].x();
        loaddata.y = in[i].y();
        analysis->addData(loaddata);
    }

    regLineData << QPointF(0.0, analysis->offset())
                << QPointF(maxAD(), analysis->slope()*maxAD()
                            + analysis->offset());
    rCurve->setSamples( regLineData );
    rCurve->attach( this );

    QString p = QString("Correlation:") +
                '\n' +
                QString("Gain: ") +
                QString::number(analysis->slope(),'f', 4.4) +
                '\n' +
#ifdef TEST_REG
                QString("num_data: ") +
                QString::number(analysis->num_data_points,'f', 5.0) +
                '\n' +
                QString("sumXY: ") +
                QString::number(analysis->sumXY(),'f', 5.0) +
                '\n' +
                QString("sumXX: ") +
                QString::number(analysis->sumXX(),'f', 5.0) +
                '\n' +
                QString("sumX: ") +
                QString::number(analysis->sumX(),'f', 5.0) +
                '\n' +
#endif
                QString("Offset: ") +
                QString::number(analysis->offset(),'f', 4.3)+
                '\n' +
                QString("R Squared: ")+
                QString::number(analysis->rsqu(),'f',4.4);

    rCurve->setTitle(p);
    rCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol, false);

    delete analysis;
}
