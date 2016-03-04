
#include <QDebug>
#include <QtGlobal>
#include <QtGui>
#include <QList>
#include <QMessageBox>
#include <QObject>

#include "helpfunction.h"
#include "matrixoftransformation.h"
#include "point.h"

MatrixOfTransformation::MatrixOfTransformation()
{
    //единничная матрица
    for (int i = 0; i < SIZE; i++) _T[i] = 0;
    for (int i = 0; i < ORD; i++) _T[i+ORD*i] = 1;
}
MatrixOfTransformation::MatrixOfTransformation(QList<Point> src, QList<Point> dst)
{
    if (helper(src, dst)) return;

    //единничная матрица
    for (int i = 0; i < SIZE; i++) _T[i] = 0;
    for (int i = 0; i < ORD; i++) _T[i+ORD*i] = 1;

    Point p0, p1, q1, q0, r1, r0, u, v, y, z;

    switch (src.length()) {
    case 1:
        p0 = src.takeFirst();
        p1 = dst.takeFirst();
        z = p1-p0;                  //сдвиг на вектор p1->p0
onlyOnePoint:
        qDebug("Только одна точка. Или больше, но все совпадают");
        // Обычный перенос, который не изменит вид изображения
        return;
    case 2:
        p0 = src.takeFirst();
        p1 = dst.takeFirst();
        q0 = src.takeFirst();
        q1 = dst.takeFirst();
        y = q1-p1;                  //вектор p1->q1 (на иск изобр-ии)
        z = q0-p0;                  //вектор p0->q0 (на исх изобр-ии)
  onlyTwoPoints:
        qDebug("Только две точки. Или больше, но все на одной прямой");
        // Растяжение вдоль вектора + поворот на плоскости до совпадения векторов на исх и иск изобр-ях
        if (z.X(0) != 0) {
            _T[0] = y.X(0) / z.X(0);
            _T[1] = 0;
            _T[2] = p1.X(0) - _T[0]*p0.X(0);
            _T[3] = y.X(1) / z.X(0);
            _T[4] = 0;
            _T[5] = p1.X(1) - _T[3]*p0.X(0);
        } else if (z.X(1) != 0) {
            _T[0] = 0;
            _T[1] = y.X(0) / z.X(1);
            _T[2] = p1.X(0) - _T[1]*p0.X(1);
            _T[3] = 0;
            _T[4] = y.X(1) / z.X(1);
            _T[5] = p1.X(1) - _T[4]*p0.X(1);
        } else {
            goto onlyOnePoint;
        }
        return;
    default:
        qDebug("Было введено больше 3-х точек, но система совместна");
        // Полное аффинное преобразование, поворот в пространстве + растяжение по двум векторам
        p0 = src.takeFirst();
        p1 = dst.takeFirst();
        q0 = src.takeFirst();
        q1 = dst.takeFirst();
        y = q1-p1;          //вектор p1->r1
        z = q0-p0;          //вектор p0->r0
        r0 = src.takeFirst();
        r1 = dst.takeFirst();
        u = r1-p1;
        v = r0-p0;
        for(;v.isColinear(z) && u.isColinear(y);) {
            if (src.isEmpty()) {
                goto onlyTwoPoints;
            }
            r0 = src.takeFirst();
            r1 = dst.takeFirst();
            u = r1-p1;
            v = r0-p0;
        }
        double det0 = v.X(1)*z.X(0) - z.X(1)*v.X(0);
        if (det0 == 0) {
            qDebug("No colinear but colinear");
            return;
        }
        _T[1] = (z.X(0)*u.X(0) - y.X(0)*v.X(0)) / det0;
        if (z.X(0) != 0) {
            _T[0] = _T[1]*z.X(1)/z.X(0) + y.X(0)/z.X(0);
        } else if (v.X(0) != 0) {
            _T[0] = _T[1]*v.X(1)/v.X(0) + u.X(0)/v.X(0);
        } else {
            qDebug("No colinear but colinear");
            return;
        }
        _T[2] = p1.X(0) - _T[0]*p0.X(0) - _T[1]*p0.X(1);

        _T[4] = (z.X(0)*u.X(1) - y.X(1)*v.X(0)) / det0;
        if (z.X(1) != 0) {
            _T[3] = _T[1]*z.X(1)/z.X(0) + y.X(1)/z.X(0);
        } else if (v.X(1) != 0) {
            _T[3] = _T[1]*v.X(1)/v.X(0) + u.X(1)/v.X(0);
        } else {
            qDebug("No colinear but colinear");
            return;
        }
        _T[5] = p1.X(1) - _T[3]*p0.X(0) - _T[4]*p0.X(1);
        return;
    }
}
MatrixOfTransformation::MatrixOfTransformation(double U[])
{
    try {
        for (int i = 0; i < SIZE; i++)
            _T[i] = U[i];
    } catch (...) {
        qFatal("Invalid initialization MatrixOfTransformation::MatrixOfTransformation(double[])");
    }
}

double                  MatrixOfTransformation::det()
{
//////////////////////
    return _T[0]*(_T[4]*_T[8]-_T[5]*_T[7]) - _T[1]*(_T[3]*_T[8]-_T[5]*_T[6]) + _T[2]*(_T[3]*_T[7]-_T[4]*_T[6]);
}
MatrixOfTransformation *MatrixOfTransformation::getInverseMatrix()
{
    MatrixOfTransformation *M = new MatrixOfTransformation();
    M->setMatrix(HelpFunction::inverse(_T));
    return M;
}
bool                    MatrixOfTransformation::helper(QList<Point> &src, QList<Point> &dst)
{
    // Определяем преобразование по множеству точек совмещения:
    // если точек < 3 или точки коллинеарны, то false и выход
    Q_ASSERT(src.length()==dst.length());
    long count = src.length();
    if (count < 3) return false;

    // обозначаем: (x,y,z)<=>Src, (u,v,w)<=>Dst
    // TX=U
    // TXX^t=UX^t
    // T=UX^t(XX^t)^(-1)
    // X = (src[1] src[2] ... src[len]) n x m
    // U = (dst[1] dst[2] ... dst[len]) n x m
    // вычисляем элементы матриц после домножения на X^t
    double MX[ORD] = { 0 }, MU[ORD] = { 0 };    // {Sx, Sy, Su}, {Su, Sv, Sw}
    double MXX[SIZE] = { 0 }, MUX[SIZE] = { 0 };    // n x n
    for (long k = 0; k < count; k++)
    {
        const long *X = src[k].X();
        const long *U = dst[k].X();
        for (int i = 0; i < ORD; i++) {
            MX[i] += X[i];
            MU[i] += U[i];
            for (int j = 0; j < ORD; j++) {
                MXX[i+j*ORD] += X[i]*X[j];
                MUX[i+j*ORD] += U[i]*X[j];
            }
        }
    }

    // вычисляем (XX^t)^(-1)
    double *InvXX = HelpFunction::inverse(MXX);
    for (int i = 0; i < ORD; i++) {
        for (int j = 0; j < ORD; j++) {
            _T[i*ORD+j] = 0;
            for (int k = 0; k < ORD; k++)
                _T[i*ORD+j] += MUX[i*ORD+k]*InvXX[k*ORD+j];
        }
    }
    return true;
}
///
bool                    MatrixOfTransformation::isEq(MatrixOfTransformation *m)
{
    bool eq = true;
    for(int i = 0; i < SIZE; i++) eq &= (_T[i] == m->_T[i]);
    return eq;
}
bool                    MatrixOfTransformation::isEq(double *U)
{
    bool eq = true;
    for(int i = 0; i < SIZE; i++) eq &= (_T[i] == U[i]);
    return eq;
}
///
void                    MatrixOfTransformation::mulOnTestMatrix(double U[])
{
    // W = UT
    double W[SIZE];
    for (int i = 0; i < ORD; i++) {
        for (int j = 0; j < ORD; j++) {
            W[i+3*j] = 0;
            for (int k = 0; k < ORD; k++)
                W[i+ORD*j] += U[k+ORD*j]*_T[i+ORD*k];
        }
    }
    for(int i = 0; i < SIZE; i++) _T[i] = W[i];
}
void                    MatrixOfTransformation::setMatrix(double *U)
{
    qDebug() << HelpFunction::showMatrix(U).toLatin1().data();
    for (int i = 0; i < SIZE; i++) _T[i] = U[i];
}
QString                 MatrixOfTransformation::showMatrix()
{
    return HelpFunction::showMatrix(_T);
}
double                  MatrixOfTransformation::T(int i, int j) const
{
    return _T[i*ORD+j];
}
Point                   MatrixOfTransformation::transformPoint(long x[])
{
    Point p(x);
    return transformPoint(p);
}
Point                   MatrixOfTransformation::transformPoint(Point p)
{
    return (*this)*(p);
}

