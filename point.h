#ifndef POINT_H
#define POINT_H

#include "constdef.h"

class MatrixOfTransformation;

class Point
{
public:
    Point();
    Point(long u[]);
    Point(const Point &p);

    long X(int i) const;
    const long *X() const;

    double length();
    bool isColinear(const Point &p);
    bool isCodirectional(const Point &p);
    long lenQuad();

    friend Point operator+ (const Point &left, const Point &right);
    friend Point operator- (const Point &p);
    friend Point operator- (const Point &left, const Point &right);

    friend Point operator* (double k, const Point &p);
    friend Point operator* (const Point &p, double k);
    friend Point operator* (const MatrixOfTransformation &M, const Point &p);
    friend MatrixOfTransformation operator* (const Point &left, const Point &right);
private:
    long _X[DIM];
    //unsigned char r, g, b;
};

#endif // POINT_H
