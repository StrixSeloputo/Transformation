
#include <math.h>

#include <QtGlobal>

#include "helpfunction.h"
#include "matrixoftransformation.h"
#include "point.h"

Point::Point()
{
    for (int i = 0; i < DIM; i++)
        _X[i] = 0;
}
Point::Point(long *u)
{
    try {
        for (int i = 0; i < DIM; i++)
            _X[i] = u[i];
    } catch (...) {
        qFatal("Invalid initialization Point::Point(long[])");
    }
}
Point::Point(const Point &p)
{
    for (int i = 0; i < DIM; i++)
        _X[i] = p._X[i];
}

long Point::X(int i) const
{
    return _X[i];
}
const long *Point::X() const
{
    return _X;
}

Point operator+ (const Point &left, const Point &right)
{
    long u[DIM] = { 0 };
    for (int i = 0; i < DIM; i++)
        u[i] = left._X[i]+right._X[i];
    Point x(u);
    return x;
}
Point operator- (const Point &p)
{
    long u[DIM] = { 0 };
    for (int i = 0; i < DIM; i++)
        u[i] = -p._X[i];
    Point x(u);
    return x;
}
Point operator- (const Point &left, const Point &right)
{
    long u[DIM] = { 0 };
    for (int i = 0; i < DIM; i++)
        u[i] = left._X[i]-right._X[i];
    Point x(u);
    return x;
}

Point operator* (double k, const Point &p) {
    long u[DIM] = { 0 };
    for (int i = 0; i < DIM; i++)
        u[i] = HelpFunction::toLong(k*p.X(i));
    Point x(u);
    return x;
}

Point operator* (const Point &p, double k) {
    long u[DIM] = { 0 };
    for (int i = 0; i < DIM; i++)
        u[i] = HelpFunction::toLong(k*p.X(i));
    Point x(u);
    return x;
}

Point operator* (const MatrixOfTransformation &M, const Point &p)
{
    long u[DIM] = { 0 };
    for (int i = 0; i < DIM; i++)
        for (int j = 0; j < DIM; j++)
            u[i] += M.T(i, j) * p.X(j);
    Point x(u);
    return x;
}

MatrixOfTransformation operator* (const Point &left, const Point &right)
{
    double T[SIZE] = { 0 };
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            T[i*DIM+j] += left.X(i)*right.X(j);
        }
    }
    return MatrixOfTransformation(T);
}

double Point::length()
{
    double len = 0;
    for (int i = 0; i < DIM; i++)
        len += _X[i]*_X[i];
    return sqrt(len);
}
long Point::lenQuad()
{
    double len = 0;
    for (int i = 0; i < DIM; i++)
        len += _X[i]*_X[i];
    return len;
}

bool Point::isColinear(const Point &p)
{
    bool isCol = true;
    double prev = _X[0]/p._X[0];
    for (int i = 1; i < DIM; i++) {
        isCol &= (prev == _X[i]/p._X[i]);
        prev = _X[i]/p._X[i];
    }
    return isCol;
}
bool Point::isCodirectional(const Point &p)
{
    return isColinear(p)&&(_X[0]*p._X[0] > 0);
}
