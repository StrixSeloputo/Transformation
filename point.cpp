
#include <math.h>

#include <QtGlobal>

#include "point.h"
#include "matrixoftransformation.h"

Point::Point()
{
    for (int i = 0; i < ORD; i++)
        _X[i] = 0;
}
Point::Point(long *u)
{
    try {
        for (int i = 0; i < ORD; i++)
            _X[i] = u[i];
    } catch (...) {
        qFatal("Invalid initialization Point::Point(long[])");
    }
}

Point::Point(const Point &p)
{
    for (int i = 0; i < ORD; i++)
        _X[i] = p._X[i];
}

long Point::X(int i) const
{
    return _X[i];
}

Point operator+ (const Point &left, const Point &right)
{
    long u[ORD] = { 0 };
    for (int i = 0; i < ORD; i++)
        u[i] = left._X[i]+right._X[i];
    Point x(u);
    return x;
}
Point operator- (const Point &p)
{
    long u[ORD] = { 0 };
    for (int i = 0; i < ORD; i++)
        u[i] = -p._X[i];
    Point x(u);
    return x;
}
Point operator- (const Point &left, const Point &right)
{
    long u[ORD] = { 0 };
    for (int i = 0; i < ORD; i++)
        u[i] = left._X[i]-right._X[i];
    Point x(u);
    return x;
}

//void Point::mulNumber(double k)
//{
//    _x = lround(_x*k);
//    _y = lround(_y*k);
//    _z = lround(_z*k);
//}

//void Point::mulMatrixLeft(double A[])
//{
////    long X = x, Y = y, Z = z;
////    x = lround(A[0]*X+A[1]*Y+A[2]*Z);
////    y = lround(A[3]*X+A[4]*Y+A[5]*Z);
////    z = lround(A[6]*X+A[7]*Y+A[8]*Z);
//}

Point operator* (const MatrixOfTransformation &M, const Point &p)
{
    long u[ORD] = { 0 };
    for (int i = 0; i < ORD; i++)
        for (int j = 0; j < ORD; j++)
            u[i] += M.T(i, j) * p.X(j);
    Point x(u);
    return x;
}

MatrixOfTransformation operator* (const Point &left, const Point &right)
{
    double T[SIZE] = { 0 };
    for (int i = 0; i < ORD; i++) {
        for (int j = 0; j < ORD; j++) {
            T[i*ORD+j] += left.X(i)*right.X(j);
        }
    }
    return MatrixOfTransformation(T);
}

double Point::length()
{
    double len = 0;
    for (int i = 0; i < ORD; i++)
        len += _X[i]*_X[i];
    return sqrt(len);
}
long Point::lenQuad()
{
    double len = 0;
    for (int i = 0; i < ORD; i++)
        len += _X[i]*_X[i];
    return len;
}

bool Point::isColinear(const Point &p)
{
    bool isCol = true;
    double prev = _X[0]/p._X[0];
    for (int i = 1; i < ORD; i++) {
        isCol &= (prev == _X[i]/p._X[i]);
        prev = _X[i]/p._X[i];
    }
    return isCol;
}
bool Point::isCodirectional(const Point &p)
{
    return isColinear(p)&&(_X[0]*p._X[0] > 0);
}
