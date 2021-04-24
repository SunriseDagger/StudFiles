#pragma once

#include <math.h>

//Решатель ЛОС, для СЛАУ с несимметричной матрицей в разреженном формате c LU - предобуславливанием

class LOS{
 public:
	 void init(int* s_ig, int* s_jg, double* s_gu, double* s_gl, double* s_di, int s_n); //инициализация
	 void set_rp(double* s_rp); //Установка правой части
	 void solve(double *&solution, int &its); //Получение решения и количества итераций
 private:
	 int n; //Размерность СЛАУ

	 //Основные массивы
	 int *ig, *jg;
	 double *gu, *gl, *di;
	 double *rp;

	 //Массивы для преобславливателя
	 double *Uu, *Ll, *Ld;

	 void precond(); //Вычисление матриц L и U
	 double dot_prod(double *a, double *b); //скалярное произведение
	 void mull_A(double *f, double *&x); // x = Af
	 void solve_L(double *f, double *&x); //Lx = f, прямой ход
	 void solve_U(double *f, double *&x); //Ux = f, обратный ход
};