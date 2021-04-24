#ifndef TYPES_H
#define TYPES_H

#include <math.h>

#define N_dim 2

class vector2lf
{
public:
    double x, y;

    vector2lf()
    {
        x = y = 0.0;
    }

    vector2lf(double s_x, double s_y)
    {
        x = s_x;
        y = s_y;
    }
    vector2lf operator+ (vector2lf op2)
    {
        return vector2lf(x+op2.x, y+op2.y);
    }

    vector2lf operator- (vector2lf op2)
    {
        return vector2lf(x-op2.x, y-op2.y);
    }

    double operator* (vector2lf op2)
    {
        return x*op2.x + y*op2.y;
    }

    vector2lf operator* (double op2)
    {
        return vector2lf(op2*x, op2*y);
    }

    double norm()
    {
        return sqrt(x*x + y*y);
    }

    double& operator[] (int i){
        if(i==0) return x;
        if(i==1) return y;
    }
};

class matrix2lf
{
public:
    vector2lf col1, col2;

    matrix2lf(vector2lf s_c1, vector2lf s_c2)
    {
        col1 = s_c1;
        col2 = s_c2;
    }

    matrix2lf(double a11, double a12, double a21, double a22)
    {
        col1 = vector2lf(a11, a21);
        col2 = vector2lf(a12, a22);
    }

    vector2lf operator * (vector2lf op2)
    {
        return vector2lf(col1.x*op2.x + col2.x*op2.y, col1.y*op2.x + col2.y*op2.y);
    }

    matrix2lf operator* (matrix2lf op2)
    {
        vector2lf n_col1 = vector2lf(col1.x * op2.col1.x + col2.x*op2.col1.y, col1.x * op2.col2.x + col2.x*op2.col2.y);
        vector2lf n_col2 = vector2lf(col1.y * op2.col1.x + col2.y*op2.col1.y, col1.y * op2.col2.x + col2.y*op2.col2.y);
        return matrix2lf(n_col1, n_col2);
    }

    matrix2lf operator* (double op2)
    {
        return matrix2lf(col1*op2, col2*op2);
    }

    matrix2lf operator+(matrix2lf op2)
    {
        return matrix2lf(col1+op2.col1, col2+op2.col2);
    }
};

#endif // TYPES_H
