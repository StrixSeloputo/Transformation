
#include <float.h>
#include <math.h>

#include <QtGlobal>
#include <QtGui>
#include <QList>
#include <QMessageBox>
#include <QObject>

#include "matrixoftransformation.h"
#include "point.h"

MatrixOfTransformation::MatrixOfTransformation()
{
    //единничная матрица
    for (int i = 0; i < SIZE; i++) _T[i] = 0;
    for (int i = 0; i < ORD; i++) _T[i+ORD*i] = 1;
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

double MatrixOfTransformation::T(int i, int j) const
{
    return _T[i*ORD+j];
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
        long X[ORD] = { src[k].X(0), src[k].X(1), src[k].X(2) };
        long U[ORD] = { dst[k].X(0), dst[k].X(1), dst[k].X(2) };
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
            _T[i*ORD+j] = 0;
            for (int k = 0; k < ORD; k++)
                _T[i*ORD+j] += MUX[i*ORD+k]*InvXX[k*ORD+j];
        }
    }
    return true;
}

Point *MatrixOfTransformation::transformPoint(long x, long y, long z)
{
    //Point *p = new Point(x, y, z);
    //return transformPoint(p);
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
    long u = toLong(_T[0]*p->X(0)) + toLong(_T[1]*p->X(1)) + toLong(_T[2]*p->X(2));
    long v = toLong(_T[3]*p->X(0)) + toLong(_T[4]*p->X(1)) + toLong(_T[5]*p->X(2));
    long w = toLong(_T[6]*p->X(0)) + toLong(_T[7]*p->X(1)) + toLong(_T[8]*p->X(2));
    //Point *t = new Point(u, v, w);
    //if (!resInThis) return t;
    //delete p;
    //return p=t;
}

double MatrixOfTransformation::det()
{
//////////////////////
    return _T[0]*(_T[4]*_T[8]-_T[5]*_T[7]) - _T[1]*(_T[3]*_T[8]-_T[5]*_T[6]) + _T[2]*(_T[3]*_T[7]-_T[4]*_T[6]);
}
MatrixOfTransformation *MatrixOfTransformation::getInverseMatrix()
{
    MatrixOfTransformation *M = new MatrixOfTransformation();
    M->setMatrix(inverse(_T));
    return M;
}

QString MatrixOfTransformation::showMatrix()
{
    return showMatrix(_T);
}
QString MatrixOfTransformation::showMatrix(double U[])
{
    QString s = "";
    for (int i = 0; i < SIZE; i++) {
        if (i % ORD == 2)
            s.append(QString::asprintf("\t%.2lf\n", U[i]));
        else
            s.append(QString::asprintf("\t%.2lf", U[i]));
    }
    return s;
}

bool MatrixOfTransformation::isEq(MatrixOfTransformation *m)
{
    bool eq = true;
    for(int i = 0; i < SIZE; i++) eq &= (_T[i] == m->_T[i]);
    return eq;
}
bool MatrixOfTransformation::isEq(double *U)
{
    bool eq = true;
    for(int i = 0; i < SIZE; i++) eq &= (_T[i] == U[i]);
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
                W[i+ORD*j] += U[k+ORD*j]*_T[i+ORD*k];
        }
    }
    for(int i = 0; i < SIZE; i++) _T[i] = W[i];
}
void MatrixOfTransformation::setMatrix(double *U)
{
    qDebug() << showMatrix(U);
    for (int i = 0; i < SIZE; i++) _T[i] = U[i];
}
