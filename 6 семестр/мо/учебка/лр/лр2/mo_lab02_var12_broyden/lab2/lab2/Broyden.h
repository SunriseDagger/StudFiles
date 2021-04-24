#pragma once

#include <vector>
#include <stdio.h>
#include <math.h>

#include "extra.h"

using namespace std;

//Реализация метода Бройдена

typedef double(*func_2d)(vect_2d);
typedef vect_2d(*grad_f)(vect_2d);


class Broyden{
 public:
	 void init(func_2d set_f, grad_f set_g, double set_eps, int set_iter); //установка вычисляемой функции, её градиента и точности
	 void minimization(vect_2d x0, vect_2d &x_m); //Минимизация функции, x0 - начальная точка, x_m - точка минимума

 private:
	 func_2d min_f; //минимизируеммая функция
	 grad_f min_f_grad;
	 long int calc_count; //количество вычислений функции
	 double meth_eps; //точтность метода
	 int max_iter; //максимальнео число итераций

	 vect_2d x_k; //приближение на преддущей операции
	 vect_2d grad_f_k; //градиент на предыдущей итерации
	 matrix_2d etta_k; //приближение матрицы Гессе не предыдущей итерации

	 double Fib(double a, double b); // Используемый одномерный метод поиска - метод Фибоначчи
	 void find_area(double x0, double& a, double& b); // Используемый метод для определения области одномреного минимума
	 double one_min_f(double lambda); // Функция для одномерной минимизации

};