#include "one_d_search.h"

using namespace one_d_search;

const double pi = 4*atan((double)1.0);

double func1(double x){
	return x*x;
}

int main(){

	f_out = fopen("rez.txt","w");
	double a, b;
	double x0 = 1E-2;
	double eps = 1E-1;
	find_area(func1, eps, x0, a, b);

	fprintf(f_out, "a = %.15lf\nb = %.15lf\n", a, b);

	double min1 = dichotomy(func1, eps, -pi/2, pi/2);


	double min2 = golden(func1, eps, -pi/2, pi/2);

	double min3 = Fib(func1, eps, -pi/2, pi/2);
	

	return 0;
}