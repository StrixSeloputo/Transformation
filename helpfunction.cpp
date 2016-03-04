
#include <float.h>
#include <math.h>
#include <stdio.h>

#include <QString>

#include "helpfunction.h"

HelpFunction::HelpFunction()
{ }

// numbers
double  HelpFunction::abs(double x)
{
    return x*sign(x);
}
long    HelpFunction::abs(long x)
{
    return x*sign(x*1.0);
}
long    HelpFunction::max(long x, long y)
{
    return (x < y) ? y : x;
}
long    HelpFunction::max(long a, long b, long c, long d)
{
    return max(max(a,b), max(c,d));
}
long    HelpFunction::min(long x, long y)
{
    return (x < y) ? x : y;
}
long    HelpFunction::min(long a, long b, long c, long d)
{
    return min(min(a,b),min(c,d));
}
long    HelpFunction::sign(double x)
{
    if (x == 0) return 0;
    return x < 0 ? -1 : 1;
}
long    HelpFunction::sign (long x)
{
    if (x == 0) return 0;
    return (x < 0) ? -1 : 1;
}
long    HelpFunction::toLong(double x)
{
    long y = lrint(x);
//    qDebug("%.2lf %ld %.2lf", x, y, (abs(x) - abs(y)));
    double  xy = abs(x) - abs(y),
            epsilon = DBL_EPSILON*fmax(abs(x), fmax(1, abs(y)));
    if ( xy > epsilon )
        y += sign(x);
    return y;
}

// matrix
void    HelpFunction::div_rows(double *T, int i, double div)
{
    for (int j = 0; j < ORD; j++)
        T[i*ORD+j] /= div;
}
double *HelpFunction::inverse(double T[])
{
    // единичная матрица
    double *U = new double[SIZE];
    for (int i = 0; i < SIZE; i++) U[i] = 0;
    for (int i = 0; i < ORD; i++) U[i+i*ORD] = 1;

    bool find = false;
    for (int i = 0; i < ORD; i++) {
        // ищем ненулевой элемент в i-ом столбце
        // сначала проверяем i-ую строку
        if (T[i*ORD + i] == 0)
        {
            // потом все строки под ней
            for (int j = i+1; j < ORD; j++)
            {
                // если нашли нужную, меняем с i-ой
                if (T[j*ORD + i] != 0)
                {
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
        for (int j = i+1; j < ORD; j++)
        {
            lin_comb_rows(U, j, i, -T[j*ORD+i]/div);
            lin_comb_rows(T, j, i, -T[j*ORD+i]/div);
        }
    }
    // обратный ход, переход к единичной матрице
    for (int i = ORD-1; i >= 0; i--)
    {
        double div = T[i*ORD+i];
        for (int j = i-1; j >= 0; j--)
        {
            lin_comb_rows(U, j, i, -T[j*ORD+i]/div);
            lin_comb_rows(T, j, i, -T[j*ORD+i]/div);
        }
        T[i*ORD+i] = 1.0;
        U[i*ORD+i] /= div;
    }
    return U;
}
void    HelpFunction::lin_comb_rows(double *T, int i, int j, double a)
{
    for (int k = 0; k < ORD; k++)
        T[i*ORD+k] += a*T[j*ORD+k];
}
QString HelpFunction::showMatrix(double U[])
{
    QString s = "";
    for (int i = 0; i < SIZE; i++)
    {
        if (i % ORD == ORD-1)
            s.append(QString::asprintf("\t %.2lf \n", U[i]));
        else
            s.append(QString::asprintf("\t %.2lf", U[i]));
    }
    return s;
}
void    HelpFunction::swap_rows(double *T, int i, int j)
{
    double s;
    for (int k = 0; k < ORD; k++)
    {
        s = T[i*ORD+k];
        T[i*ORD+k] = T[j*ORD+k];
        T[j*ORD+k] = s;
    }
}
