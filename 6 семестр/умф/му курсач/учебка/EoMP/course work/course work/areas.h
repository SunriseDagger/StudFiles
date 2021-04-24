#pragma once

#include <math.h>
#include <string>

typedef double(*func)(double, double, double, double);
using namespace std;

namespace area_1{
	extern string path; //путь папке с данными
	extern int lambda_n, betta_n, u_betta_n, tetta_n, fir_n; //количество тех или иных данных

	//Сами данные
	extern double *lambda_values;
	extern double *betta_values;
	extern func *gamma_values;
	extern func *sigma_values;
	extern func *epsilon_values;
	extern func *u_betta_values;
	extern func *tetta_values;
	extern func *fir_values;

	double sol_func(double x, double y, double z, double t); //решение
	double equation_rp(double x, double y, double z, double t); //правая часть уравнения(точнее функция V(x,y,z,t), f = dV/dx)

	void init_area(); //инициализировать данные
	
};
	
namespace area_2{
	extern string path; //путь папке с данными
	extern int lambda_n, betta_n, u_betta_n, tetta_n, fir_n; //количество тех или иных данных

	//Сами данные
	extern double *lambda_values;
	extern double *betta_values;
	extern func *gamma_values;
	extern func *sigma_values;
	extern func *epsilon_values;
	extern func *u_betta_values;
	extern func *tetta_values;
	extern func *fir_values;

	double sol_func(double x, double y, double z, double t); //решение
	double equation_rp(double x, double y, double z, double t); //правая часть уравнения(точнее функция V(x,y,z,t), f = dV/dx)

	void init_area(); //инициализировать данные
	
};
	
namespace area_3{
	extern string path; //путь папке с данными
	extern int lambda_n, betta_n, u_betta_n, tetta_n, fir_n; //количество тех или иных данных

	//Сами данные
	extern double *lambda_values;
	extern double *betta_values;
	extern func *gamma_values;
	extern func *sigma_values;
	extern func *epsilon_values;
	extern func *u_betta_values;
	extern func *tetta_values;
	extern func *fir_values;

	double sol_func(double x, double y, double z, double t); //решение
	double equation_rp(double x, double y, double z, double t); //правая часть уравнения(точнее функция V(x,y,z,t), f = dV/dx)

	void init_area(); //инициализировать данные
	
};