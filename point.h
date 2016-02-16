#ifndef POINT_H
#define POINT_H


class Point
{
public:
    Point();
    Point(long i, long j);
    Point(Point *p);
    long X();
    long Y();
    void addVector(Point *other);
    void addVector(double B[]);
    void subVector(Point *other);
    void mulMatrixLeft(double A[]);
    void mulNumber(double k);
    double length();
    bool isColinear(Point *p);
    bool isCodirectional(Point *p);
    long lenQuad();
private:
    long x, y, z;
};

#endif // POINT_H
