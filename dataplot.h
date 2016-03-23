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
#include <QMessageBox>
#include <QDialog>

#include "regression.h"

class DataPlot : public QwtPlot
{
     Q_OBJECT
public:
    explicit DataPlot(QWidget* /*parent*/);
    ~DataPlot();

    void createPoints(const QString& rawdata);

private:

    bool loadPlotDataPoints(const QString& line);
    void displayGraph(const QVector<QPointF>& points);
    void createRegLine(const QVector<QPointF>& in);

    void createClasses();
    void SetPlotParameters();
    void SetGridParameters();
    void SetCurveParameters();
    void SetRCurveParameters();

    QwtPlot* Plot;
    QwtPlotCurve* Curve;
    QwtPlotCurve* rCurve;
    QwtPlotGrid* Grid;
    QwtSymbol* Symbol;
    QwtSymbol* rSymbol;

    QPolygonF plotDataPoints;

    static int endLineConst(void){ return(-1); };
    static int readingPosConst(void){ return(15); };
    static int readingWidthConst(void){ return(5); };
    static int moisturePosConst(void){ return(28); };
    static int lineSizeConst(void){ return(27); };
    static qreal maxAD(void){return(409.6);};
};

#endif // DATAPLOT_H
