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
    MatrixOfTransformation(double U[]);
    MatrixOfTransformation(QList<Point> src, QList<Point> dst);

    double T(int i, int j) const;
    double *getMatrix();
    MatrixOfTransformation *getInverseMatrix();
    double det();
    QString showMatrix();
    bool    isEq(MatrixOfTransformation *m);
    bool    isEq(double *U);
    Point transformPoint(long x[]);
    Point transformPoint(Point p);
    Point *setNewValue(QList<Point>src, QList<Point>dst);

    long toLong(double x);

//public slots:
    void mulOnTestMatrix(double U[]);
    void setMatrix(double *U);
private:
    double _T[SIZE];      //has type:
                        //  (a11    a12     a13 |   b1)
                        //  (a21    a22     a23 |   b2)
                        //  (a31    a23     a33 |   b3)
    bool helper(QList<Point> &src, QList<Point> &dst);
    //для вычислений с количеством опорных точек не меньшим трех, не лежащих на одной прямой

    QString showMatrix(double U[]);
};

#endif // MATRIXOFTRANSFORMATION_H
