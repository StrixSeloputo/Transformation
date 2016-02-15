
#include <float.h>
#include <math.h>

#include <QtGui>
#include <QList>
#include <QMessageBox>
#include <QObject>

#include "matrixoftransformation.h"
#include "point.h"

MatrixOfTransformation::MatrixOfTransformation()
{
    for (int i = 1; i < 6; i++) T[i] = 0;
    T[0] = T[1+3] = 1;           //единничная матрица
}

MatrixOfTransformation::MatrixOfTransformation(QList<Point> src, QList<Point> dst)
{
    if (helper(src, dst)) return;

    for (int i = 1; i < 6; i++) T[i] = 0;
    T[0] = T[1+3] = 1;
    Point p0, p1, q1, q0, r1, r0, *u, *v, *y, *z;

    switch (src.length()) {
    case 1:
        p0 = src.takeFirst();
        p1 = dst.takeFirst();
        z = new Point(&p1);
        z->subVector(&p0);              //сдвиг на вектор p1->p0
onlyOnePoint:
        qDebug("Только одна точка. Или больше, но все совпадают");
        T[2] = z->X();
        T[5] = z->Y();
        delete z;
        return;
    case 2:
        p0 = src.takeFirst();
        p1 = dst.takeFirst();
        q0 = src.takeFirst();
        q1 = dst.takeFirst();
        y = new Point(q1);
        y->subVector(&p1);          //вектор p1->r1
        z = new Point(q0);
        z->subVector(&p0);          //вектор p0->r0
  onlyTwoPoints:
        qDebug("Только две точки. Или больше, но все на одной прямой");
        if (z->X() != 0) {
            T[0] = y->X() / z->X();
            T[1] = 0;
            T[2] = p1.X() - T[0]*p0.X();
            T[3] = y->Y() / z->X();
            T[4] = 0;
            T[5] = p1.Y() - T[3]*p0.X();
        } else if (z->Y() != 0) {
            T[0] = 0;
            T[1] = y->X() / z->Y();
            T[2] = p1.X() - T[1]*p0.Y();
            T[3] = 0;
            T[4] = y->Y() / z->Y();
            T[5] = p1.Y() - T[4]*p0.Y();
        } else {
            delete y;
            goto onlyOnePoint;
        }
        delete y;
        delete z;
        return;
    default:
        qDebug("Было введено больше 3-х точек, но система совместна");
        p0 = src.takeFirst();
        p1 = dst.takeFirst();
        q0 = src.takeFirst();
        q1 = dst.takeFirst();
        y = new Point(q1);
        y->subVector(&p1);          //вектор p1->r1
        z = new Point(q0);
        z->subVector(&p0);          //вектор p0->r0
        r0 = src.takeFirst();
        r1 = dst.takeFirst();
        u = new Point(r1);
        u->subVector(&p1);
        v = new Point(r0);
        v->subVector(&p0);
        for(;v->isColinear(z) && u->isColinear(y);) {
            if (src.isEmpty()) {
                delete u;
                delete v;
                goto onlyTwoPoints;
            }
            r0 = src.takeFirst();
            r1 = dst.takeFirst();
            u = new Point(r1);
            u->subVector(&p1);
            v = new Point(r0);
            v->subVector(&p0);
        }
        double det0 = v->Y()*z->X() - z->Y()*v->X();
        if (det0 == 0) {
            qDebug("No colinear but colinear");
            return;
        }
        T[1] = (z->X()*u->X() - y->X()*v->X()) / det0;
        if (z->X() != 0) {
            T[0] = T[1]*z->Y()/z->X() + y->X()/z->X();
        } else if (v->X() != 0) {
            T[0] = T[1]*v->Y()/v->X() + u->X()/v->X();
        } else {
            qDebug("No colinear but colinear");
            return;
        }
        T[2] = p1.X() - T[0]*p0.X() - T[1]*p0.Y();

        T[4] = (z->X()*u->Y() - y->Y()*v->X()) / det0;
        if (z->Y() != 0) {
            T[3] = T[1]*z->Y()/z->X() + y->Y()/z->X();
        } else if (v->Y() != 0) {
            T[3] = T[1]*v->Y()/v->X() + u->Y()/v->X();
        } else {
            qDebug("No colinear but colinear");
            return;
        }
        T[5] = p1.Y() - T[3]*p0.X() - T[4]*p0.Y();

        delete u;
        delete v;
        delete y;
        delete z;
        return;
    }
}

bool MatrixOfTransformation::helper(QList<Point> &src, QList<Point> &dst)
{
    // Определяем преобразование по множеству точек совмещения:
    // если точек < 3 или точки коллинеарны, то false и выход
    Q_ASSERT(src.length()==dst.length());
    long count = src.length();
    if (src.length() < 3) return false;

    // обозначаем: (x,y)<=>Src, (u,v)<=>Dst
    // TX=U
    // TXX^t=UX^t
    // вычисляем элементы матриц после домножения (формула выше)
    double Sx = 0, Sy = 0, Sv = 0, Su = 0;
    double  Sxx = 0, Syy = 0, Sxy = 0,
            Sxu = 0, Syu = 0, Sxv = 0, Syv = 0;
    for (long i = 0; i < count; i++)
    {
        long x = src[i].X(), y = src[i].Y();
        long u = dst[i].X(), v = dst[i].Y();
        Sx += x; Sy += y;   //channel src
        Su += u; Sv += v;   //channel dst
        Sxx += x*x; Sxy += x*y; Syy += y*y;
        Sxu += x*u; Syu += y*u;
        Sxv += x*v; Syv += y*v;
    }

    // вычисляем определитель матрицы XX^t
    long det = Sxx*Syy*count - Sxx*Sy*Sy - Sxy*Sxy*count + Sxy*Sx*Sy + Sx*Sxy*Sy - Sx*Sx*Syy;
    if (det == 0) return false;
    // коэффициенты прямого преобразования: (x,y)->(u,v) (SrcToDst)
    //      u = T[0]*x + T[1]*y + T[2]
    //      v = T[3]*x + T[4]*y + T[5]
    double A[9];
    A[0] = (count*Syy - Sy*Sy)/det;
    A[1] = -(Sxy*count - Sx*Sy)/det;
    A[2] = (Sxy*Sy - Syy*Sx)/det;
    A[3] = -(count*Sxy - Sx*Sy)/det;
    A[4] = (count*Sxx - Sx*Sx)/det;
    A[5] = -(Sxx*Sy - Sxy*Sx)/det;
    A[6] = (Sxy*Sy - Syy*Sx)/det;
    A[7] = -(Sxx*Sy - Sxy*Sx)/det;
    A[8] = (Sxx*Syy - Sxy*Sxy)/det;

    double B[9];
    B[0] = Sxu;
    B[1] = Syu;
    B[2] = Su;
    B[3] = Sxv;
    B[4] = Syv;
    B[5] = Sv;
    B[6] = Sx;
    B[7] = Sy;
    B[8] = count;

    double C[9];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            C[i+3*j] = B[0+3*j]*A[i+3*0] + B[1+3*j]*A[i+3*1] + B[2+3*j]*A[i+3*2];
        }
    }
    Q_ASSERT(C[6]==0);
    Q_ASSERT(C[7]==0);
    Q_ASSERT(C[8]==1);
    for(int i = 0; i < 6; i++) T[i] = C[i];
    return true;
}

Point *MatrixOfTransformation::transformPoint(long x, long y)
{
    Point *p = new Point(x, y);
    return transformPoint(p);
}
long sign(double x) {
    if (x == 0) return 0;
    return x < 0 ? -1 : 1;
}
double abs(double x) {
    return x*sign(x);
}
long abs(long x) {
    return x*sign(x*1.0);
}

long toLong(double x)
{
    long y = lrint(x);
//    qDebug("%.2lf %ld %.2lf", x, y, (abs(x) - abs(y)));
    double  xy = abs(x) - abs(y),
            epsilon = DBL_EPSILON*fmax(abs(x), fmax(1, abs(y)));
    if ( xy > epsilon )
        y += sign(x);
    return y;
}

Point *MatrixOfTransformation::transformPoint(Point *p, bool resInThis)
{
    long u = toLong(T[0]*p->X()) + toLong(T[1]*p->Y()) + toLong(T[2]);
    long v = toLong(T[3]*p->X()) + toLong(T[4]*p->Y()) + toLong(T[5]);
    Point *t = new Point(u, v);
    if (!resInThis) return t;
    delete p;
    return p=t;
}

double MatrixOfTransformation::det()
{
    return T[0]*T[4]-T[1]*T[3];
}
MatrixOfTransformation *MatrixOfTransformation::getInverseMatrix()
{
    MatrixOfTransformation *m = new MatrixOfTransformation();
    double d = det();
    if (d != 0) {
        m->T[0] =  T[4]/d;
        m->T[1] = -T[1]/d;
        m->T[2] = -T[2];
        m->T[3] = -T[3]/d;
        m->T[4] =  T[0]/d;
        m->T[5] = -T[5];
    } else {
        QMessageBox::information(0, "Transformation",
                                 "Cannot reverse matrix of transformation");
    }
    return m;
}

QString MatrixOfTransformation::showMatrix()
{
    return QString::asprintf("\t%.2lf\t%.2lf\t%.2lf\n\t%.2lf\t%.2lf\t%.2lf\n\t0.00\t0.00\t1.00\n", T[0], T[1], T[2], T[3], T[4], T[5]);
}
bool MatrixOfTransformation::isEq(MatrixOfTransformation *m)
{
    bool eq = true;
    for(int i = 0; i < 6; i++) eq &= (T[i] == m->T[i]);
    return eq;
}
bool MatrixOfTransformation::isEq(double *U)
{
    bool eq = true;
    for(int i = 0; i < 6; i++) eq &= (T[i] == U[i]);
    return eq;
}

void MatrixOfTransformation::mulOnTestMatrix(double U[])
{
    double W[6];
    W[0] = U[0]*T[0] + U[1]*T[3];
    W[1] = U[0]*T[1] + U[1]*T[4];
    W[2] = U[0]*T[2] + U[1]*T[5] + U[2];
    W[3] = U[3]*T[0] + U[4]*T[3];
    W[4] = U[3]*T[1] + U[4]*T[4];
    W[5] = U[3]*T[2] + U[4]*T[5] + U[5];
    for(int i = 0; i < 6; i++) T[i] = W[i];
}
void MatrixOfTransformation::setTestMatrix(double *U)
{
    qDebug("\t%.2lf\t%.2lf\t%.2lf\n\t%.2lf\t%.2lf\t%.2lf\n\t0.00\t0.00\t1.00\n", U[0], U[1], U[2], U[3], U[4], U[5]);
    for (int i = 0; i < 6; i++) T[i] = U[i];
    showMatrix();
}
