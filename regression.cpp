/* ---------------------------------------------------------------------------
** This software is furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** File Name: regression.cpp
** CPP file for regression analysis
**
** Author: Michael W. Hoag
** Email: mike@ndtjames.com
** Copyright Michael W. Hoag 2015
** -------------------------------------------------------------------------*/

//error checking is currently assumed to be from calling function.


#include <math.h>
#include "regression.h"

Regression::Regression()
{
    QPointF init_point = QPointF(1.0, 1.0);
    reg_data<<init_point;
    num_data_points = 1.0;
    bad_init_data = true; //says first element just crap data
}

Regression::Regression( const QVector <QPointF>& in)
{
    reg_data = in;
    num_data_points = (qreal) in.size();
    bad_init_data = false; //says first element just crap data
}

Regression::~Regression(){

}

void Regression::addData( QPointF data ){

    if( bad_init_data == false){ //check if class just initialized?
        reg_data.append( data ); //no great!
        num_data_points = reg_data.size();
    }else{
        reg_data[0] = data; //yes replace first element
        bad_init_data = false;
    }
}

qreal Regression::sumX(){

    qreal total_sum_x = 0.0;

    for (int i = 0; i < reg_data.size(); ++i) {
        total_sum_x += reg_data[i].x();
    }
    return(total_sum_x);
}

qreal Regression::sumY(){

    qreal total_sum_y = 0.0;

    for (int i = 0; i < reg_data.size(); ++i) {
        total_sum_y += reg_data[i].y();
    }
    return(total_sum_y);
}

qreal Regression::sumXY(){

    qreal total_sum_xy = 0.0;

    for (int i = 0; i < reg_data.size(); ++i) {
        total_sum_xy += ((reg_data[i].y()) * (reg_data[i].x()));
    }
    return(total_sum_xy);
}

qreal Regression::sumXX(){

    qreal total_sum_xx = 0.0;

    for (int i = 0; i < reg_data.size(); ++i) {
        total_sum_xx += ((reg_data[i].x()) * (reg_data[i].x()));
    }
    return(total_sum_xx);
}

qreal Regression::sumYY(){

    qreal total_sum_yy = 0.0;

    for (int i = 0; i < reg_data.size(); ++i) {
        total_sum_yy += ((reg_data[i].y()) * (reg_data[i].y()));
    }
    return(total_sum_yy);
}

qreal Regression::mean_x(){

    qreal mean = 0.0;

    mean = sumX() / num_data_points;

    return( mean );
}

qreal Regression::mean_y(){

    qreal mean = 0.0;

    mean = sumY() / num_data_points;

    return( mean );
}

qreal Regression::stand_dev_x(){ // Calculates the standard deviation of all x values
//http://onlinestatbook.com/2/summarizing_distributions/variability.html


    qreal stand_dev = 0.0;
    qreal variance = 0.0;
    qreal sumxsq = 0.0;
    qreal sumx_sqn = 0.0;

    sumxsq = sumXX();
    sumx_sqn = pow( sumX(), 2) / num_data_points;

    variance = (sumxsq-sumx_sqn)/( num_data_points - 1 );

    stand_dev = sqrt(variance);

    return(stand_dev);
}

qreal Regression::stand_dev_y(){ // Calculates the standard deviation of all y values
//http://onlinestatbook.com/2/summarizing_distributions/variability.html
    qreal stand_dev = 0.0;
    qreal variance = 0.0;
    qreal sumysq = 0.0;
    qreal sumy_sqn = 0.0;

    sumysq = sumYY();
    sumy_sqn = pow( sumY(), 2) / num_data_points ;

    variance = (sumysq-sumy_sqn)/( num_data_points - 1 );

    stand_dev = sqrt(variance);

    return(stand_dev);
}

qreal Regression::r(){
// http://onlinestatbook.com/2/regression/intro.html
    qreal correlation = 0.0;
    qreal numerator = 0.0;
    qreal denominator = 0.0;
    qreal sumXSQ = 0.0;
    qreal sumYSQ = 0.0;

    for(int i = 0; i < reg_data.size(); ++i){
        numerator += ( reg_data[i].x() - mean_x()) *
                     ( reg_data[i].y() - mean_y());
        sumXSQ += pow( reg_data[i].x() - mean_x(), 2);
        sumYSQ += pow( reg_data[i].y() - mean_y(), 2);
    }
    denominator = sqrt( sumYSQ * sumXSQ );
    correlation = numerator / denominator;

    return( correlation );
}

qreal Regression::rsqu(){

    qreal correlation = 0.0;
    qreal numerator = 0.0;
    qreal denominator = 0.0;
    qreal sumXSQ = 0.0;
    qreal sumYSQ = 0.0;

    for(int i = 0; i < reg_data.size(); ++i){
        numerator += ( reg_data[i].x() - mean_x()) *
                     ( reg_data[i].y() - mean_y());
        sumXSQ += pow( reg_data[i].x() - mean_x(), 2);
        sumYSQ += pow( reg_data[i].y() - mean_y(), 2);
    }
    denominator = sqrt( sumYSQ * sumXSQ );
    correlation = numerator / denominator;

    return( correlation * correlation );
}

qreal Regression::slope(){

    qreal m  = 0.0;

    m = r()*stand_dev_y()/stand_dev_x();

    return(m);
}

qreal Regression::offset(){

    qreal b  = 0.0;
    b = mean_y() - slope()* mean_x();

    return(b);
}
