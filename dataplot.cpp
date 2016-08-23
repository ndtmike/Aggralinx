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

DataPlot::DataPlot( QWidget* )
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

void DataPlot::createPoints(const QString& rawdata)
{
    QString working = rawdata;
    QString line = "";
    qDebug()<< rawdata;
    QStringList datalines = rawdata.split("\n", QString::SkipEmptyParts);

    plotDataPoints.clear();

    foreach (line, datalines) {
        if(!loadPlotDataPoints(line)) return;
    }

    displayGraph(plotDataPoints);
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

    QString p = QString("Correlation Line:") + '\n' +
                QString("Gain: ") + QString::number(analysis->slope(),'f', 4.4) + '\n' +
                QString("Offset: ") + QString::number(analysis->offset(),'f', 4.3)+ '\n' +
                QString("R Squared: ")+ QString::number(analysis->rsqu(),'f',4.4);

    displayRegParameters(p);
    rCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol, false);
    delete analysis;
}

void DataPlot::displayGraph(const QVector<QPointF>& points)
{
    setAxisScale(QwtPlot::yLeft, minAxisScale(), maxY(points));
    setAxisScale(QwtPlot::xBottom, minAxisScale(), maxX(points));
    Curve->setSamples( points );
    Curve->attach(this);

    createRegLine(plotDataPoints);

    resize( 1200, 800 );
    replot();
    show();
}

void DataPlot::displayRegParameters(const QString &in){

    QwtPlotTextLabel* text_label = new QwtPlotTextLabel;
    QwtText qwt_text(in);
    QFont font = qwt_text.font();
    font.setPointSize(14);
    qwt_text.setFont(font);
    qwt_text.setColor(Qt::black);
    qwt_text.setRenderFlags( Qt::AlignLeft | Qt::AlignTop );

    text_label->setText(qwt_text);
    text_label->attach( this );
    text_label->show();
}

bool DataPlot::loadPlotDataPoints(const QString& line)
{
    bool success = false;
    double dr = 0.0;
    double dm = 0.0;
    QString buffer;
    QTextStream bufferstream(& buffer);

    QStringList words = line.split(" ");
    QStringList::iterator wordsiterator = words.end()-1;
    buffer = *wordsiterator;
    if(buffer.count('%') != 0){
        buffer = buffer.remove('%');
        bufferstream.setString(&buffer); //had to reset string in buffer in order to double result
        bufferstream >> dm;
        --wordsiterator;
        buffer = *wordsiterator;
        bufferstream.setString(&buffer); //reset string in buffer in order to get double output
        bufferstream >> dr;
        success = true;
    }else{
        QMessageBox::information(this,"Aggralinx",
            tr("Data Not Entered!"));
    }

    qDebug()<< dr << dm;

    plotDataPoints << QPointF(dr,dm);
    return(success);
}

void DataPlot::SetPlotParameters()
{
    setTitle(tr("Aggralinx Regression"));
    setAxisTitle( QwtPlot::xBottom, "Aggrameter Value");
    setAxisTitle( QwtPlot::yLeft, "Moisture Value");
    setCanvasBackground( Qt::white );
    insertLegend( new QwtLegend() );

}

void DataPlot::SetCurveParameters()
{
    Curve->setTitle( "Data Points" );
    Curve->setStyle(QwtPlotCurve::NoCurve);
    Curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    Curve->setSymbol( Symbol );
}

void DataPlot::SetGridParameters()
{
    Grid->attach(this);
}

void DataPlot::SetRCurveParameters()
{
    rCurve->setStyle(QwtPlotCurve::Lines);
    rCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
}


qreal DataPlot::maxY(const QVector<QPointF> &in)
{
    qreal rvalue = 0.0;
    qreal test = 0.0;
    QVector<QPointF> buffer = in;

    for(int i = 0; i<in.size();++i){
        test = buffer[i].ry();
        rvalue = qMax( test, rvalue);
    }
    return(rvalue);
}

qreal DataPlot::maxX(const QVector<QPointF> &in)
{
    qreal rvalue = 0.0;
    qreal test = 0.0;
    QVector<QPointF> buffer = in;

    for(int i = 0; i<in.size();++i){
        test = buffer[i].rx();
        rvalue = qMax( test, rvalue);
    }
    return(rvalue);
}
