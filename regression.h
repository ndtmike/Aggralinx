/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: regression.h
** header file for regression analysis
**
** Author: Michael W. Hoag
** Email: mike@ndtjames.com
** Copyright Michael W. Hoag 2015
** -------------------------------------------------------------------------*/

//nice reference...http://onlinestatbook.com/2/regression/intro.html

#ifndef REGRESSION_H
#define REGRESSION_H

#include <QVector>
#include <QPointF>
#include <QtGlobal>
#ifdef QT_DEBUG
#include <QMessageBox>
#endif

class Regression
{
public:

    QVector <QPointF> reg_data;

    Regression();
    Regression( const QVector <QPointF>& in);
    ~Regression();
    void addData( QPointF data );

    qreal num_data_points;
    qreal stand_dev_x();
    qreal stand_dev_y();
    qreal mean_x();
    qreal mean_y();
    qreal r();
    qreal rsqu();
    qreal slope();
    qreal offset();

private:

    bool bad_init_data;

    qreal sumX();
    qreal sumY();
    qreal sumXY();
    qreal sumXX();
    qreal sumYY();


};

#endif // REGRESSION_H
