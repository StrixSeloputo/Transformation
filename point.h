#ifndef POINT_H
#define POINT_H

#define     SIZE    9
#define     ORD     3

class MatrixOfTransformation;

class Point
{
public:
    Point();
    Point(long u[]);
    Point(const Point &p);

    long X(int i) const;

    double length();
    bool isColinear(const Point &p);
    bool isCodirectional(const Point &p);
    long lenQuad();

    friend Point operator+ (const Point &left, const Point &right);
    friend Point operator- (const Point &p);
    friend Point operator- (const Point &left, const Point &right);

    friend Point operator* (const MatrixOfTransformation &M, const Point &p);
    friend MatrixOfTransformation operator* (const Point &left, const Point &right);
private:
    long _X[ORD];
    //unsigned char r, g, b;
};

#endif // POINT_H
