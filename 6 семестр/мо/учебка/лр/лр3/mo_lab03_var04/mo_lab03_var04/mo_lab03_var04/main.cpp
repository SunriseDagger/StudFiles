#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "vector2.h"
#include "tabu.h"
using namespace std;

void interval(double &a, double &b, class vect x, class vect s)
{
    double lambda0 = 0.0;
    double delta = 1.0e-8;
    double lambda_k_minus_1 = lambda0;
    double f_k_minus_1 = f(x + s * lambda_k_minus_1);
    double lambda_k;
    double f_k;
    double lambda_k_plus_1;
    double f_k_plus_1;
    double h;
    if (f(x + s * lambda0) > f(x + s * (lambda0 + delta)))
    {
        lambda_k = lambda0 + delta;
        h = delta;
    }
    else
    {
        lambda_k = lambda0 - delta;
        h = -delta;
    }
    f_k = f(x + s * lambda_k);
    while (true)
    {
        h *= 2.0;
        lambda_k_plus_1 = lambda_k + h;
        f_k_plus_1 = f(x + s * lambda_k_plus_1);
        if (f_k > f_k_plus_1)
        {
            lambda_k_minus_1 = lambda_k;
            f_k_minus_1 = f_k;
            lambda_k = lambda_k_plus_1;
            f_k = f_k_plus_1;
        }
        else
        {
            a = lambda_k_minus_1;
            b = lambda_k_plus_1;
            if (b < a)
                swap(a, b);
            return;
        }
    }
}

inline double fib(int n)
{
    double sqrt5 = sqrt(5.0), pow2n = pow(2.0, n);
    return ( pow(1.0 + sqrt5, n) / pow2n - pow(1.0 - sqrt5, n) / pow2n ) / sqrt5;
}

double fibonacci(class vect x, class vect s)
{
    double a = -2.0, b = 2.0;
    interval(a, b, x, s);
    int iter;
    double len = fabs(a - b);
    int n = 0;
    while (fib(n) < (b - a) / eps) n++;
    iter = n - 3;
    double lambda1 = a + (fib(n-2) / fib(n)) * (b - a);
    double f1 = f(x + s * lambda1);
    double lambda2 = a + (fib(n - 1) / fib(n)) * (b - a);
    double f2 = f(x + s * lambda2);
    for (int k = 0; k < n - 3; k++)
    {
        if (f1 <= f2)
        {
            b = lambda2;
            lambda2 = lambda1;
            f2 = f1;
            lambda1 = a + (fib(n - k - 3) / fib(n - k - 1)) * (b - a);
            f1 = f(x + s * lambda1);
        }
        else
        {
            a = lambda1;
            lambda1 = lambda2;
            f1 = f2;
            lambda2 = a + (fib(n - k - 2) / fib(n - k - 1)) * (b - a);
            f2 = f(x + s * lambda2);
        }
        len = b - a;
    }
    lambda2 = lambda1 + eps;
    f2 = f(x + s * lambda2);
    if (f1 <= f2)
        b = lambda1;
    else
        a = lambda1;
    return (a + b) / 2.0;
}

bool allow_stop(class vect x1, class vect x)
{
    if (fabs(f(x1) - f(x)) <= eps)
        return true;
    if (fabs(x1[0] - x[0]) <= eps && fabs(x1[1] - x[1]) <= eps)
        return true;
    return false;
}

int main()
{
    class vect x, xold, s[2], a[2], b;
    double lambda1, lambda2;
    int iter = 0;
    f_calc = 0;
    x[0] = x_0;
    x[1] = y_0;
    // Начальные ортогональные направления
    s[0][0] = s[1][1] = 1.0;
    s[0][1] = s[1][0] = 0.0;
    // Сам метод (с ортогонализацией Палмера)
    while(iter <= maxiter &&(!iter || !allow_stop(x, xold)))
    {
        xold = x;
        lambda1 = fibonacci(x, s[0]);
        x = x + s[0] * lambda1;
        lambda2 = fibonacci(x, s[1]);
        x = x + s[1] * lambda2;
        // Построение новых ортогональных направлений
        a[0] = s[0] * lambda1 + s[1] * lambda2;
        a[1] = s[1] * lambda2;
        double a0_norm = a[0].norm();
        double a1_norm = a[1].norm();
        s[0] = a[0] / a0_norm;
        s[1] = (a[1] * a0_norm * a0_norm - a[0] * a1_norm * a1_norm) /
               (a0_norm * a1_norm * sqrt(a0_norm * a0_norm - a1_norm * a1_norm));
        iter++;
        tabu_names::r_print(cout);
        tabu_names::r_correct(x);
        cout << f_calc << '\t' << iter << '\t' << x << '\t' << tabu_names::f(x) << endl;
    }
    cout << endl << endl << "FINAL:" << endl << f_calc << '\t' << iter << '\t' << x << '\t' << tabu_names::f(x) << endl;
    return 0;
}
