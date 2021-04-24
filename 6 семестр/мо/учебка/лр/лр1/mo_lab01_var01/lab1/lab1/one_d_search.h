#pragma once

#include <math.h>
#include <stdio.h>
#include <vector>

typedef double (*func)(double);

using namespace std;

namespace one_d_search{
	extern int iter_dich;
	extern int iter_gold;
	extern int iter_Fib;
	extern FILE* f_out;

	void find_area(func f, double delta, double x0, double& a, double& b);
	double dichotomy(func f, double eps, double a, double b);
	double golden(func f, double eps, double a, double b);
	double Fib(func f, double eps, double a, double b);
};