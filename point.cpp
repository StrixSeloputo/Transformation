
#include <math.h>

#include "point.h"

Point::Point()
{
    x = y = 0;
}
Point::Point(long i, long j)
{
    x = i;
    y = j;
}
Point::Point(Point *p)
{
    x = p->x;
    y = p->y;
}
long Point::X()
{
    return x;
}
long Point::Y()
{
    return y;
}

void Point::addVector(Point *other)
{
    x += other->x;
    y += other->y;
}
void Point::addVector(double B[])
{
    x += lround(B[0]);
    y += lround(B[1]);
}

void Point::subVector(Point *other)
{
    x -= other->x;
    y -= other->y;
}
void Point::mulNumber(double k)
{
    x = lround(x*k);
    y = lround(y*k);
}

void Point::mulMatrixLeft(double A[])
{
    long X = x, Y = y;
    x = lround(A[0]*X+A[1]*Y);
    y = lround(A[2]*X+A[3]*Y);
}

double Point::length()
{
    return hypot(x, y);
}
long Point::lenQuad()
{
    return x*x+y*y;
}

bool Point::isColinear(Point *p)
{
    return (x*p->y == y*p->x);
}
bool Point::isCodirectional(Point *p)
{
    return isColinear(p)&&(x*p->x > 0);
}
