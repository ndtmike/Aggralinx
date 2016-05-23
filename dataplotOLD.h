/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: dataplot.h
**
** Header file for analydialog.cpp - aggralinx software
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
#include <QPointF>
#include<QMessageBox>

class DataPlot : QwtPlot
{
     Q_OBJECT

public:
    DataPlot(QWidget *parent = NULL);
    ~DataPlot();

    void loadPoints(const QVector<QPointF>& points);
    void createPoints(const QString& rawdata);

private:
    void SetPlotParameters();
    void SetGridParameters();
    void SetCurveParameters();
    bool loadPlotDataPoints(const QString& line);

    QwtPlot* Plot;
    QwtPlotCurve* Curve;
    QwtPlotGrid* Grid;
    QwtSymbol* Symbol;

    QVector<QPointF> plotDataPoints;

    static int endLineConst(void){ return(-1); };
    static int readingPosConst(void){ return(15); };
    static int readingWidthConst(void){ return(5); };
    static int moisturePosConst(void){ return(28); };
    static int lineSizeConst(void){ return(27); };
};

#endif // DATAPLOT_H
