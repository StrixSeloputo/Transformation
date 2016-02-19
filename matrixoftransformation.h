#ifndef MATRIXOFTRANSFORMATION_H
#define MATRIXOFTRANSFORMATION_H

#define SYMMETRY_NO             0x10
#define SYMMETRY_OX             0x11
#define SYMMETRY_OY             0x12
#define SIZE                    9
#define ORD                     3
class Point;

class MatrixOfTransformation
{
public:
    MatrixOfTransformation();
    MatrixOfTransformation(QList<Point> src, QList<Point> dst);
    double *getMatrix();
    MatrixOfTransformation *getInverseMatrix();
    double det();
    QString showMatrix();
    bool    isEq(MatrixOfTransformation *m);
    bool    isEq(double *U);
    Point *transformPoint(long x, long y, long z);
    Point *transformPoint(Point *p, bool resInThis = true);
    Point *setNewValue(QList<Point>src, QList<Point>dst);
public slots:
    void mulOnTestMatrix(double U[]);
    void setTestMatrix(double *U);
private:
    double T[SIZE];      //has type:
                        //  (a11    a12     a13 |   b1)
                        //  (a21    a22     a23 |   b2)
                        //  (a31    a23     a33 |   b3)
    bool helper(QList<Point> &src, QList<Point> &dst);
    //для вычислений с количеством опорных точек не меньшим трех, не лежащих на одной прямой
    double *B();
    double *A();
};

#endif // MATRIXOFTRANSFORMATION_H
