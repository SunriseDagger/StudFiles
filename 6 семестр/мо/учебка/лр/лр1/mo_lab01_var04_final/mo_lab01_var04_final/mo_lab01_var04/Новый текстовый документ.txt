#include <math.h>
#include <stdio.h>

const double x_beg = 2.0, x_end = 200.0;

inline double f(double x)
{
    return (x - 15.0) * (x - 15.0) + 5.0;
}

double dichotomy(FILE *fp, double eps, int &iter)
{
    double a = x_beg;
    double b = x_end;
    double delta = eps / 2.0;
    iter = 1;
    double len = fabs(a - b);
    fprintf(fp, "eps = %.1E\n", eps);
    fprintf(fp, "Итерация\ta\t\t\tb\t\tДлина отрезка\t\tОтношение длин\n");
    while (true)
    {
        double x1 = (a + b - delta) / 2.0;
        double x2 = (a + b + delta) / 2.0;
        double f1 = f(x1);
        double f2 = f(x2);
        if (f1 <= f2)
            b = x2;
        else
            a = x1;
        fprintf(fp, "%i\t%.12E\t%.12E\t%.12E\t%.12E\n", iter, a, b, b - a, len / (b - a));
        len = b - a;
        if (fabs(a - b) < eps)
            return (a + b) / 2.0;
        iter++;
    }
}

double golden_ratio(FILE *fp, double eps, int &iter)
{
    double a = x_beg;
    double b = x_end;
    double q1 = (3.0 - sqrt(5.0)) / 2.0;
    double q2 = (sqrt(5.0) - 1.0) / 2.0;
    iter = 1;
    double len = fabs(a - b);
    fprintf(fp, "eps = %.1E\n", eps);
    double x1 = a + q1 * (b - a);
    double x2 = a + q2 * (b - a);
    double f1 = f(x1);
    double f2 = f(x2);
    int flag;
    fprintf(fp, "Итерация\ta\t\t\tb\t\tДлина отрезка\t\tОтношение длин\n");
    while (true)
    {
        if (f1 <= f2)
        {
            b = x2;
            x2 = x1;
            f2 = f1;
            flag = 1;
        }
        else
        {
            a = x1;
            x1 = x2;
            f1 = f2;
            flag = 2;
        }
        fprintf(fp, "%i\t%.12E\t%.12E\t%.12E\t%.12E\n", iter, a, b, b - a, len / (b - a));
        len = b - a;
        if (fabs(a - b) < eps)
            return (a + b) / 2.0;
        if(flag == 1)
        {
            x1 = a + q1 * (b - a);
            f1 = f(x1);
        }
        else
        {
            x2 = a + q2 * (b - a);
            f2 = f(x2);
        }
        iter++;
    }
}

double F(int n)
{
    double sqrt5 = sqrt(5.0), pow2n = pow(2.0, n);
    return ( pow(1.0 + sqrt5, n) / pow2n - pow(1.0 - sqrt5, n) / pow2n ) / sqrt5;
}

double fibonacci(FILE *fp, double eps, int& iter)
{
    double a = x_beg;
    double b = x_end;
    double len = fabs(a - b);
    fprintf(fp, "eps = %.1E\n", eps);
    int n = 0;
    while (F(n) < (b - a) / eps) n++;
    iter = n - 3;
    double x1 = a + (F(n-2) / F(n)) * (b - a);
    double f1 = f(x1);
    double x2 = a + (F(n - 1) / F(n)) * (b - a);
    double f2 = f(x2);
    fprintf(fp, "Итерация\ta\t\t\tb\t\tДлина отрезка\t\tОтношение длин\n");
    for (int k = 0; k < n - 3; k++)
    {
        if (f1 <= f2)
        {
            b = x2;
            x2 = x1;
            f2 = f1;
            x1 = a + (F(n - k - 3) / F(n - k - 1)) * (b - a);
            f1 = f(x1);
        }
        else
        {
            a = x1;
            x1 = x2;
            f1 = f2;
            x2 = a + (F(n - k - 2) / F(n - k - 1)) * (b - a);
            f2 = f(x2);
        }
        fprintf(fp, "%i\t%.12E\t%.12E\t%.12E\t%.12E\n", k + 1, a, b, b - a, len / (b - a));
        len = b - a;
    }
    x2 = x1 + eps;
    f2 = f(x2);
    if (f1 <= f2)
        b = x1;
    else
        a = x1;
    return (a + b) / 2.0;
}

void interval(double &a, double &b, double x0, double delta, int &iter)
{
    double x_k_minus_1 = x0;
    double f_x_k_minus_1 = f(x_k_minus_1);
    double x_k;
    double f_x_k;
    double x_k_plus_1;
    double f_x_k_plus_1;
    double h;
    if (f(x0) > f(x0 + delta))
    {
        x_k = x0 + delta;
        h = delta;
    }
    else
    {
        x_k = x0 - delta;
        h = -delta;
    }
    f_x_k = f(x_k);
    iter = 0;
    while (true)
    {
        h *= 2.0;
        x_k_plus_1 = x_k + h;
        f_x_k_plus_1 = f(x_k_plus_1);
        if (f_x_k > f_x_k_plus_1)
        {
            x_k_minus_1 = x_k;
            f_x_k_minus_1 = f_x_k;
            x_k = x_k_plus_1;
            f_x_k = f_x_k_plus_1;
            iter++;
        }
        else
        {
            a = x_k_minus_1;
            b = x_k_plus_1;
            if (b < a)
            {
                double t = b;
                b = a;
                a = t;
            }
            return;
        }
    }
}

int main()
{
    int iterations;
    double x;
    FILE *fp;
    fp = fopen("dichotomy.txt", "w");
    for (double eps = 1.0E-1; eps > 1.0E-14; eps *= 1.0E-1)
    {
        x = dichotomy(fp, eps, iterations);
        fprintf(fp, "Result:\t%.16lf\nIter:\t%i\nError:\t%.3E\n\n", x, iterations, fabs(x - 15.0) / 15.0);
    }
    fclose(fp);
    fp = fopen("golden_ratio.txt", "w");
    for (double eps = 1.0E-1; eps > 1.0E-14; eps *= 1.0E-1)
    {
        x = golden_ratio(fp, eps, iterations);
        fprintf(fp, "Result:\t%.16lf\nIter:\t%i\nError:\t%.3E\n\n", x, iterations, fabs(x - 15.0) / 15.0);
    }
    fclose(fp);
    fp = fopen("fibonacci.txt", "w");
    for (double eps = 1.0E-1; eps > 1.0E-14; eps *= 1.0E-1)
    {
        x = fibonacci(fp, eps, iterations);
        fprintf(fp, "Result:\t%.16lf\nIter:\t%i\nError:\t%.3E\n\n", x, iterations, fabs(x - 15.0) / 15.0);
    }
    fclose(fp);

    double a, b;
    fp = fopen("interval1.txt.txt", "w");
    fprintf(fp, "Eps\tInterval\t\tLength\tIterations\n");
    for (double eps = 1.0E-1; eps > 1.0E-14; eps *= 1.0E-1)
    {
        interval(a, b, 0.0, eps, iterations);
        fprintf(fp, "%.1E\t[%.6lf, %.6lf]\t%.6lf\t%i\n", eps, a, b, b - a, iterations);
    }
    fclose(fp);
    fp = fopen("interval2.txt.txt", "w");
    fprintf(fp, "x0\t|xmin-x0|\tInterval\t\tLength\tIterations\n");
    for (int i = -10; i <= 10; i++)
    {
        double x0 = 10.0 * i;
        interval(a, b, x0, 1E-8, iterations);
        fprintf(fp, "%.2lf\t%.2lf\t[%.6lf, %.6lf]\t%.6lf\t%i\n", x0, fabs(x0 - 15.0), a, b, b - a, iterations);
    }
    fclose(fp);
    return 0;
}

