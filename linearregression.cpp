/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: linearregression.h
**
** Linear curve fit.
**
** Author: Michael W. Hoag
** Copyright Michael W. Hoag 2016
** Email: mike@ndtjames.com
** -------------------------------------------------------------------------*/

#include "linearregression.h"

LinearRegression::LinearRegression()
{

}

double LinearRegression::gain()
{
    double g = bottom()!=0 ? top()/bottom():0;
    return (g);
}

double LinearRegression::offset()
{
    double o = sumY() - gain()*sumX()/(double)data.size();
    return(o);
}

double LinearRegression::top()
{
    double t = sumXY()-((sumX()*sumY())/ (double)data.size());
    return(t);
}

double LinearRegression::bottom()
{
    double b = sumXX()-(sumX()*sumY()/(double)data.size());
    return(b);
}
double LinearRegression::rr()
{
    double r = 0.0;

    return(r);
}

double LinearRegression::sumXY()
{
    double xy = 0.0;
    int count=0;
    for( QPolygonF::Iterator i = data.begin(); i!=data.end();++i )
    {
        xy += data[count].rx()*data[count].ry();
        ++count;
    }
    return(xy);
}

double LinearRegression::sumX()
{
    double x = 0.0;
    int count = 0;
    for( QPolygonF::Iterator i = data.begin(); i!=data.end();++i )
    {
        x += data[count].rx();
        ++count;
    }
    return(x);
}

double LinearRegression::sumY()
{
    double y = 0.0;
    int count = 0;
    for( QPolygonF::Iterator i = data.begin(); i!=data.end();++i )
    {
        y += data[count].ry();
        ++count;
    }
    return(y);
}

double LinearRegression::sumXX()
{
    double xx = 0.0;
    int count = 0;
    for( QPolygonF::Iterator i = data.begin(); i!=data.end();++i )
    {
        xx += data[count].rx()*data[count].rx();
        count++;
    }
    return(xx);
}

void LinearRegression::loadPoints(QPolygonF& points)
{
    data = points;
}
