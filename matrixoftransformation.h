#ifndef MATRIXOFTRANSFORMATION_H
#define MATRIXOFTRANSFORMATION_H

#include "constdef.h"

class Point;

class MatrixOfTransformation
{
public:
    MatrixOfTransformation();
    MatrixOfTransformation(double U[]);
    MatrixOfTransformation(QList<Point> src, QList<Point> dst);

    double                  T(int i, int j) const;
    double                  det();
    MatrixOfTransformation *getInverseMatrix();
    double                 *getMatrix();
    bool                    isEq(MatrixOfTransformation *m);
    bool                    isEq(double *U);
    void                    mulOnTestMatrix(double U[]);
    void                    setMatrix(double *U);
    Point                  *setNewValue(QList<Point>src, QList<Point>dst);
    QString                 showMatrix();
    Point                   transformPoint(long x[]);
    Point                   transformPoint(Point p);
private:
    double _T[SIZE];      //has type:
                        //  (a11    a12     a13 )
                        //  (a21    a22     a23 )
                        //  (a31    a23     a33 )
    //для вычислений с количеством опорных точек не меньшим трех, не лежащих на одной прямой
    bool                    helper(QList<Point> &src, QList<Point> &dst);
};

#endif // MATRIXOFTRANSFORMATION_H
