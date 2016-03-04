#ifndef HELPFUNCTION_H
#define HELPFUNCTION_H

#include "constdef.h"

class QString;

class HelpFunction
{
public:
    HelpFunction();
    //numbers
    static long     abs(long x);
    static double   abs(double x);
    static long     max(long x, long y);
    static long     max(long a, long b, long c, long d);
    static long     min(long x, long y);
    static long     min(long a, long b, long c, long d);
    static long     sign(double x);
    static long     sign (long x);
    static long     toLong(double x);
    //matrix
    static void     div_rows(double *T, int i, double div);
    static double  *inverse(double T[]);
    static void     lin_comb_rows(double *T, int i, int j, double a);
    static QString  showMatrix(double U[]);
    static void     swap_rows(double *T, int i, int j);
};

#endif // HELPFUNCTION_H
