
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
    //единничная матрица
    for (int i = 0; i < SIZE; i++) T[i] = 0;
    for (int i = 1; i < ORD; i++) T[i+ORD*i] = 0;
}

MatrixOfTransformation::MatrixOfTransformation(QList<Point> src, QList<Point> dst)
{
    if (helper(src, dst)) return;

    //единничная матрица
    for (int i = 0; i < SIZE; i++) T[i] = 0;
    for (int i = 1; i < ORD; i++) T[i+ORD*i] = 0;

    Point p0, p1, q1, q0, r1, r0, *u, *v, *y, *z;

    switch (src.length()) {
    case 1:
        p0 = src.takeFirst();
        p1 = dst.takeFirst();
        z = new Point(&p1);
        z->subVector(&p0);              //сдвиг на вектор p1->p0
onlyOnePoint:
        qDebug("Только одна точка. Или больше, но все совпадают");
        // Обычный перенос, который не изменит вид изображения
        delete z;
        return;
    case 2:
        p0 = src.takeFirst();
        p1 = dst.takeFirst();
        q0 = src.takeFirst();
        q1 = dst.takeFirst();
        y = new Point(q1);
        y->subVector(&p1);          //вектор p1->q1 (на иск изобр-ии)
        z = new Point(q0);
        z->subVector(&p0);          //вектор p0->q0 (на исх изобр-ии)
  onlyTwoPoints:
        qDebug("Только две точки. Или больше, но все на одной прямой");
        // Растяжение вдоль вектора + поворот на плоскости до совпадения векторов на исх и иск изобр-ях
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
        // Полное аффинное преобразование, поворот в пространстве + растяжение по двум векторам
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
void swap_rows(double *T, int i, int j) {
    double s;
    for (int k = 0; k < ORD; k++) {
        s = T[i*ORD+k];
        T[i*ORD+k] = T[j*ORD+k];
        T[j*ORD+k] = s;
    }
}
void lin_comb_rows(double *T, int i, int j, double a)
{
    for (int k = 0; k < ORD; k++)
        T[i*ORD+k] += a*T[j*ORD+k];
}
void div_rows(double *T, int i, double div)
{
    for (int j = 0; j < ORD; j++)
        T[i*ORD+j] /= div;
}

double *inverse(double T[])
{
    // единичная матрица
    double *U = new double[SIZE];
    for (int i = 0; i < SIZE; i++) U[i] = 0;
    for (int i = 0; i < ORD; i++) U[i+i*ORD] = 1;

    bool find = false;
    for (int i = 0; i < ORD; i++) {
        // ищем ненулевой элемент в i-ом столбце
        // сначала проверяем i-ую строку
        if (T[i*ORD + i] == 0) {
            // потом все строки под ней
            for (int j = i+1; j < ORD; j++) {
                // если нашли нужную, меняем с i-ой
                if (T[j*ORD + i] != 0) {
                    swap_rows(T, i, j);
                    swap_rows(U, i, j);
                    find = true;
                }
            }
            // иначе, матрица необратима
            if (!find) {
                delete []U;
                return NULL;
            }
        }
        // вычитаем из строк ниже i-ую умноженную на такой коэффициент, чтобы получить столбец нулей (ниже)
        double div = T[i*ORD+i];
        for (int j = i+1; j < ORD; j++) {
            lin_comb_rows(U, j, i, -T[j*ORD+i]/div);
            lin_comb_rows(T, j, i, -T[j*ORD+i]/div);
        }
    }
    // обратный ход, переход к единичной матрице
    for (int i = ORD-1; i >= 0; i--) {
        double div = T[i*ORD+i];
        for (int j = i-1; j >= 0; j--) {
            lin_comb_rows(U, j, i, -T[j*ORD+i]/div);
            lin_comb_rows(T, j, i, -T[j*ORD+i]/div);
        }
        T[i*ORD+i] = 1.0;
        U[i*ORD+i] /= div;
    }
    return U;
}
bool MatrixOfTransformation::helper(QList<Point> &src, QList<Point> &dst)
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
        long X[ORD] = { src[k].X(), src[k].Y(), src[k].Z() };
        long U[ORD] = { dst[k].X(), dst[k].Y(), dst[k].Z() };
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
    double *InvXX = inverse(MXX);
    for (int i = 0; i < ORD; i++) {
        for (int j = 0; j < ORD; j++) {
            T[i*ORD+j] = 0;
            for (int k = 0; k < ORD; k++)
                T[i*ORD+j] += MUX[i*ORD+k]*InvXX[k*ORD+j];
        }
    }
    return true;
}

Point *MatrixOfTransformation::transformPoint(long x, long y, long z)
{
    Point *p = new Point(x, y, z);
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
    long u = toLong(T[0]*p->X()) + toLong(T[1]*p->Y()) + toLong(T[2]*p->Z());
    long v = toLong(T[3]*p->X()) + toLong(T[4]*p->Y()) + toLong(T[5]*p->Z());
    long w = toLong(T[6]*p->X()) + toLong(T[7]*p->Y()) + toLong(T[8]*p->Z());
    Point *t = new Point(u, v, w);
    if (!resInThis) return t;
    delete p;
    return p=t;
}

double MatrixOfTransformation::det()
{
//    double  det = 0;          // 0 1 2
//    double  sgn = 1;          // 3 4 5
//    bool    has_row = true;   // 6 7 8
//    double W[SIZE];
//    for (int i = 0; i < SIZE; i++) W[i] = T[i];
    return T[0]*(T[4]*T[8]-T[5]*T[7]) - T[1]*(T[3]*T[8]-T[5]*T[6]) + T[2]*(T[3]*T[7]-T[4]*T[6]);
}
MatrixOfTransformation *MatrixOfTransformation::getInverseMatrix()
{
//    MatrixOfTransformation *m = new MatrixOfTransformation();
//    double d = det();
//    if (d != 0) {
//        m->T[0] =  T[4]/d;
//        m->T[1] = -T[1]/d;
//        m->T[2] = -T[2];
//        m->T[3] = -T[3]/d;
//        m->T[4] =  T[0]/d;
//        m->T[5] = -T[5];
//    } else {
//        QMessageBox::information(0, "Transformation",
//                                 "Cannot reverse matrix of transformation");
//    }
//    return m;
}

QString MatrixOfTransformation::showMatrix()
{
///////////////////////
//    return QString::asprintf("\t%.2lf\t%.2lf\t%.2lf\n\t%.2lf\t%.2lf\t%.2lf\n\t0.00\t0.00\t1.00\n", T[0], T[1], T[2], T[3], T[4], T[5]);
}
bool MatrixOfTransformation::isEq(MatrixOfTransformation *m)
{
    bool eq = true;
    for(int i = 0; i < SIZE; i++) eq &= (T[i] == m->T[i]);
    return eq;
}
bool MatrixOfTransformation::isEq(double *U)
{
    bool eq = true;
    for(int i = 0; i < SIZE; i++) eq &= (T[i] == U[i]);
    return eq;
}

void MatrixOfTransformation::mulOnTestMatrix(double U[])
{
    // W = UT
    double W[SIZE];
    for (int i = 0; i < ORD; i++) {
        for (int j = 0; j < ORD; j++) {
            W[i+3*j] = 0;
            for (int k = 0; k < ORD; k++)
                W[i+ORD*j] += U[k+ORD*j]*T[i+ORD*k];
        }
    }
    for(int i = 0; i < SIZE; i++) T[i] = W[i];
}
void MatrixOfTransformation::setTestMatrix(double *U)
{
/////////////////////
//    qDebug("\t%.2lf\t%.2lf\t%.2lf\n\t%.2lf\t%.2lf\t%.2lf\n\t0.00\t0.00\t1.00\n", U[0], U[1], U[2], U[3], U[4], U[5]);
//    for (int i = 0; i < 6; i++) T[i] = U[i];
//    showMatrix();
}
