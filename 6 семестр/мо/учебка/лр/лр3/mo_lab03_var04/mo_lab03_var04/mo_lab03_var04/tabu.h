#ifndef TABU_H_INCLUDED
#define TABU_H_INCLUDED
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "vector2.h"
using namespace std;

static const double eps = 1e-7;
static const double x_0 = -2.0;
static const double y_0 = -2.0;
static const double r_0 = 1.0;
static const int maxiter = 100;
static int f_calc;

namespace tabu_names
{

inline double f(class vect z)
{
    f_calc++;
    double x = z[0], y = z[1];
    return -(2.0/(1.0+((x-1.0)/2.0)*((x-1.0)/2.0)+((y-2.0)/1.0)*((y-2.0)/1.0))+
            1.0/(1.0+((x-3.0)/3.0)*((x-3.0)/3.0)+((y-1.0)/3.0)*((y-1.0)/3.0)));
}

inline double g(class vect x)
{
    return 3.0 * x[0] + x[1] - 3.0;
}

inline double G(class vect x)
{
    // Штрафные
    return pow(0.5 * (g(x) + fabs(g(x))), 2.0);
    // Барьерные
    return -1.0 / g(x);
    return -log(-g(x));
}

double r = r_0;

inline void r_correct(class vect x)
{
    if(r * G(x) > eps)
    // Штрафные
        r *= 10.0;
        //r += 1.0;
        //r = pow(r+1.0, 4.0);
    // Барьерные
        //r /= 2.0;
        //r = pow((0.5 * r), 2.0);
}

inline void r_print(ostream& ostream_)
{
    ostream_.setf(ios::scientific);
    ostream_.precision(8);
    ostream_ << "r = " << r << endl;
}

};

inline double f(class vect x)
{
    return tabu_names::f(x) + tabu_names::r * tabu_names::G(x);
}

#endif // TABU_H_INCLUDED
