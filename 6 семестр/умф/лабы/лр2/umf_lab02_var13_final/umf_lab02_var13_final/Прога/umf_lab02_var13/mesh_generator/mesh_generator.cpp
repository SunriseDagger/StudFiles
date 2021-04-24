#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>

using namespace std;

static const double epsilon    = 1e-5;
static const double mesh_begin = 0.0;
static const double mesh_end   = 5.0;
static const double mesh_h     = 1.0;
static const double lambda_    = 1.0;
static const double gamma_     = 1.0;
static const double k_         = 1.0;
static const char* fn_area     = "../area.txt";
static const char* fn_bound    = "../bound.txt";

inline static double u(const double x)
{
    //return x + 2.0;
    return 2.0 * x * x;
    //return x * x * x;
    //return sin(x);
    //return exp(x);
    //return cos(x);
}

int main()
{
    FILE* fp_area = fopen(fn_area, "w");
    FILE* fp_bound = fopen(fn_bound, "w");
    vector<double> x;
    x.push_back(mesh_begin);
    int iter = 0;
    while(x[x.size() - 1] < mesh_end - epsilon)
    {
        x.push_back(x[x.size() - 1] + mesh_h * pow(k_,iter));
        iter++;
    }
    if(x[x.size() - 1] > mesh_end - epsilon)
        x.pop_back();
    x.push_back(mesh_end);
    fprintf(fp_area, "%i\n", x.size() - 1);
    for(int i = 0; i < x.size() - 1; i++)
    {
        fprintf(fp_area, "%lf  %lf  %lf  %lf\n", x[i], x[i+1], lambda_, gamma_);
    }
    fprintf(fp_bound, "1\n%lf\n", u(mesh_begin));
    fprintf(fp_bound, "1\n%lf\n", u(mesh_end));
    fprintf(fp_bound, "\n\n");
    fclose(fp_area);
    fclose(fp_bound);
    return 0;
}
