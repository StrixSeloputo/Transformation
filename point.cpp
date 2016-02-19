
#include <math.h>

#include "point.h"

Point::Point()
{
    x = y = 0;
}
Point::Point(long i, long j, long k)
{
    x = i;
    y = j;
    z = k;
}
Point::Point(Point *p)
{
    x = p->x;
    y = p->y;
    z = p->z;
}
long Point::X()
{
    return x;
}
long Point::Y()
{
    return y;
}
long Point::Z()
{
    return z;
}

void Point::addVector(Point *other)
{
    x += other->x;
    y += other->y;
    z += other->z;
}
void Point::addVector(double B[])
{
    x += lround(B[0]);
    y += lround(B[1]);
    z += lround(B[3]);
}

void Point::subVector(Point *other)
{
    x -= other->x;
    y -= other->y;
    z -= other->z;
}
void Point::mulNumber(double k)
{
    x = lround(x*k);
    y = lround(y*k);
    z = lround(z*k);
}

void Point::mulMatrixLeft(double A[])
{
    long X = x, Y = y, Z = z;
    x = lround(A[0]*X+A[1]*Y+A[2]*Z);
    y = lround(A[3]*X+A[4]*Y+A[5]*Z);
    z = lround(A[6]*X+A[7]*Y+A[8]*Z);
}

double Point::length()
{
    return sqrt(x*x + y*y + z*z);
}
long Point::lenQuad()
{
    return x*x + y*y + z*z;
}

bool Point::isColinear(Point *p)
{
    return (x*p->y == y*p->x) && (z*p->y == y*p->z);
}
bool Point::isCodirectional(Point *p)
{
    return isColinear(p)&&(x*p->x > 0);
}
