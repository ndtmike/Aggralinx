/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: dataplot.h
**
** Header file for dataplot.cpp - aggralinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#ifndef DATAPLOT_H
#define DATAPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_textlabel.h>
#include <QPointF>
#include <QMessageBox>
#include <QDialog>
#include <QtCore>

#include "regression.h"

#ifdef QT_DEBUG
//#define TEST_REG
#endif

class DataPlot : public QwtPlot
{
     Q_OBJECT
public:
    explicit DataPlot(QWidget*);
    ~DataPlot();

    void createPoints(const QString& rawdata);

private:

    void createClasses();
    void createRegLine(const QVector<QPointF>& in);
    void displayGraph(const QVector<QPointF>& points);
    void displayRegParameters( const QString& in);
    bool loadPlotDataPoints(const QString& line);
    qreal maxX(const QVector<QPointF>& in);
    qreal maxY(const QVector<QPointF>& in);
    void SetCurveParameters();
    void SetGridParameters();
    void SetPlotParameters();
    void SetRCurveParameters();

    QwtPlotCurve* Curve;
    QwtPlotCurve* rCurve;
    QwtPlotGrid* Grid;
    QwtPlot* Plot;
    QPolygonF plotDataPoints;
    QwtSymbol* rSymbol;
    QwtSymbol* Symbol;

    static int endLineConst(void){ return(-1); };
    static int readingPosConst(void){ return(15); };
    static int readingWidthConst(void){ return(5); };
    static int moisturePosConst(void){ return(28); };
    static int lineSizeConst(void){ return(27); };
    static qreal maxAD(void){return(409.6);};
    static qreal minAxisScale(void){return(0.0);};
    static qreal maxYAxisScale(void){return(20.0);};
    static qreal maxXAxisScale(void){return(409.5);};
};

#endif // DATAPLOT_H
