#include <windows.h>

#include "Gauss.h"

//Штрафные функции
double pen1(func_2d f, vect_2d arg){
	return (f(arg) + fabs(f(arg))) / 2.0;
}
double pen2(func_2d f, vect_2d arg){
	return pow((f(arg) + fabs(f(arg))), 2) / 2.0;
}
double pen3(func_2d f, vect_2d arg){
	return pow((f(arg) + fabs(f(arg))), 4) / 2.0;
}

//Барьерные функции
double pen4(func_2d f, vect_2d arg){
	return -1/f(arg);
}
double pen5(func_2d f, vect_2d arg){
	return -log(-f(arg));
}

double bound1(vect_2d arg){
	return arg.x - arg.y - 1;
}
double bound2(vect_2d arg){
	return -arg.x;
}
double bound3(vect_2d arg){
	return arg.x - 4;
}

double func1(vect_2d arg){
	return (arg.x + 5) * (arg.x + 5) + (arg.y + 1) * (arg.y + 1);
}
double func2(vect_2d arg){
	return (-3.0/(1.0 + (arg.x-3)*(arg.x-3)/4.0 + (arg.y-2)*(arg.y-2)) - 3.0/(1.0 + (arg.x-1)*(arg.x-1) + (arg.y-2)*(arg.y-2)/9.0));
}

int main(){

	//Данные для функтора
	vector<func_2d> bound_v;
	vector<penalty_func> pen_v;
	vector<double> coef_v;
	double r0 = 100.0;

	//Установка ограничений
	bound_v.push_back(bound1);
	bound_v.push_back(bound2);
	bound_v.push_back(bound3);

	//Установка функций штрафа
	pen_v.push_back(pen1);
	pen_v.push_back(pen1);
	pen_v.push_back(pen1);

	//Установка коэффициентов штрафа
	coef_v.push_back(1.0);
	coef_v.push_back(1.0);
	coef_v.push_back(1.0);

	functor min_funct(func1, r0, bound_v, pen_v, coef_v);

	//Данные для метода
	double eps = 1E-6;
	vect_2d start(0, 1);
	vect_2d arg_min;

	Gauss our_meth;
	our_meth.set_func(min_funct);
	our_meth.set_start(start);
	our_meth.set_eps(eps);

	arg_min = our_meth.find_min();

	printf("%.15lf\t%.15lf\n", arg_min.x, arg_min.y);
	system("pause");


	return 0;
}