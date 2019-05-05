#ifndef MYMATHLIB_H
#define MYMATHLIB_H

class MyMathLib
{
public:
    static constexpr double pi = 3.1415926535898;
    static constexpr double smallValue = 0.000001;

    static int Sgn(const double& num);

    static double CalcEuclideanDistance(const double* startArray, const double* endArray, int dim);

    static double AngleToRad(const double& x);//角度转弧度
    static double RadToAngle(const double& x);//弧度转角度
};

#endif // MYMATHLIB_H
