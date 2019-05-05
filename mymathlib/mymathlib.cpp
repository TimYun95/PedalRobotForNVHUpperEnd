#include "mymathlib.h"

#include <cmath>

int MyMathLib::Sgn(const double &num)
{
    if(num < 0.0){
        return -1;
    }else if(num == 0.0){
        return 0;
    }else{
        return 1;
    }
}

double MyMathLib::CalcEuclideanDistance(const double* startArray, const double* endArray, int dim)
{
    double squareSum = 0.0;
    for(int i=0; i<dim; ++i){
        double delta = startArray[i] - endArray[i];
        squareSum += std::pow(delta, 2);
    }

    return std::sqrt(squareSum);
}

double MyMathLib::AngleToRad(const double &x)
{
    return x*pi/180.0;
}

double MyMathLib::RadToAngle(const double &x)
{
    return x*180.0/pi;
}
