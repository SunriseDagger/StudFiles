#pragma once

#include <math.h>

//Решатель методом LU-разложения для СЛАУ с несимметричной матрицей в профильном формате

class solver_LU{
 public:
	 void init(int* s_ig, int* s_jg, double* s_gu, double* s_gl, double* s_di, int s_n); //инициализация, перевод из разреженного в профильный формат
	 void set_rp(double* s_rp); //Установка правой части
	 void solve(double *&solution, int &its); //Получение решения и количества итераций
 private:

	 int n; //Размерность СЛАУ

	 //Массивы
	 int *ig;
	 double *gl, *gu, *di;
	 double *rp;

	 void dec(); //Простроение LU-разложения

};