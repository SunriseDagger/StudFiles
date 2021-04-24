#include <windows.h>

#include "easy_random_search.h"
#include "best_sample_search.h"

double func1(vect_2d arg){
	return (arg.x+5) * (arg.x+5) + (arg.y+1)*(arg.y+1);
}

double func2(vect_2d arg){
	double x = arg.x;
	double y = arg.y;
	double pi = 4*atan(1.0);
	return -20*exp(-0.2*sqrt(0.5*(x*x+y*y))) - exp(0.5*(cos(2*pi*x)+cos(2*pi*y))) + 20 + exp(1.0);
}

double func3(vect_2d arg){
	double x = arg.x;
	double y = arg.y;
	return -exp(-abs(x)-y*y*y*y)*cos(2*x*x*x)*cos(10*y*y);
}

int main(){

	easy_random_search our_rs;

	our_rs.init_func(func2);
	our_rs.init_rectangle(vect_2d(0.1,0.1), 0.2, 0.2);
	our_rs.init_tests(1E-2, 0.9);

	vect_2d min = our_rs.search();

	printf("Minimun in:\t %.7lf\t%.7lf\nValue is:\t%.15lf\n", min.x, min.y, func2(min));
	system("pause");

/*	best_sample_search our_bss;

	our_bss.init_func(func1);
	our_bss.init_rectangle(vect_2d(2,1), 1E-1, 1E-1);
	our_bss.init_max_iters(500000);
	our_bss.init_tests(1E-4, 0.99);
	our_bss.init_eps_grow(1E-2);

	vect_2d min = our_bss.search();

	printf("Minimun in:\t %.7lf\t%.7lf\nValue is:\t%.15lf\n", min.x, min.y, func1(min));
	system("pause");*/


	return 0;
}