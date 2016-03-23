/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: linearregression.h
**
** Header file for analydialog.cpp - aggralinx software
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/
#ifndef LINEARREGRESSION_H
#define LINEARREGRESSION_H

#include <QPointF>
#include <QPolygonF>

class LinearRegression
{
public:
//    struct LRData{
//    public:
//        double x;
//        double y;
//    }

    LinearRegression();
    void loadPoints(QPolygonF& points);
    double gain();
    double offset();
    double rr();

private:
//    QList<LRData> data;
    QPolygonF data;

    double top();
    double bottom();
    double sumXY();
    double sumX();
    double sumY();
    double sumXX();

};

#endif // LINEARREGRESSION_H
