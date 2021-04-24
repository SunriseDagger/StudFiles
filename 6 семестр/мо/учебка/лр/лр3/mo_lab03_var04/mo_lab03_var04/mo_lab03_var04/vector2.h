#ifndef VECTOR2_H_INCLUDED
#define VECTOR2_H_INCLUDED
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
using namespace std;

class vect
{
private:
    double x[2];
public:
    inline vect()
    {
        x[0] = x[1] = 0.0;
    }
    inline vect(double z, double y)
    {
        x[0] = z;
        x[1] = y;
    }
    inline double norm()
    {
        return sqrt(x[0] * x[0] + x[1] * x[1]);
    }
    inline double& operator [] (unsigned i)
    {
        return x[i];
    }
    inline vect operator + (vect y)
    {
        return vect(x[0] + y[0], x[1] + y[1]);
    }
    inline vect operator - (vect y)
    {
        return vect(x[0] - y[0], x[1] - y[1]);
    }
    inline vect operator * (double c)
    {
        return vect(x[0] * c, x[1] * c);
    }
    inline vect operator / (double c)
    {
        return vect(x[0] / c, x[1] / c);
    }
    inline double mult (vect y)
    {
        return x[0] * y[0] + x[1] * y[1];
    }
    friend ostream& operator << (ostream& ostream_, const vect& v)
    {
        ostream_.setf(ios::scientific);
        ostream_.precision(8);
        ostream_ << "[ " << v.x[0] << ", " << v.x[1] << " ]";
        return ostream_;
    }
};

#endif // VECTOR2_H_INCLUDED
