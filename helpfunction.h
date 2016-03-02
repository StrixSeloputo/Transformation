#ifndef HELPFUNCTION_H
#define HELPFUNCTION_H


class HelpFunction
{
public:
    HelpFunction();
    //numbers
    static long sign(double x);
    static long abs(long x);
    static double abs(double x);
    static long toLong(double x);
    //matrix
    static void swap_rows(double *T, int i, int j);
    static void lin_comb_rows(double *T, int i, int j, double a);
    static void div_rows(double *T, int i, double div);
    static double *inverse(double T[]);
};

#endif // HELPFUNCTION_H
