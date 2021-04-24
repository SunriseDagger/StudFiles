#pragma once

#include <stdio.h>

#include <omp.h>
#include <vector>
#include <algorithm>

#include "random_generator.h"

using namespace std;

//Класс, реализующий метод наилучшей пробы с направляющим гиперквадратом, в области
//x - y <= 1
//0 <= x <= 4
class best_sample_search{
 public:
	 void init_rectangle(vect_2d s_x0, double s_hx, double s_hy); //Установка центральной точки прямоугольника и начальных шагов по сеткам
	 void init_tests(double s_eps, double s_P); //Установка числа генерируемых точек(оно вычиялется каждый раз через точность, вероятность и площадь прямоугольника)
	 void init_func(func_2d s_func); //Установка искомой функции
	 void init_max_iters(int s_max_iters = 100); //Установка максимального числа итераций
	 void init_eps_grow(double s_eps_grow = 1E-1); //Установить значеине разности функций, при котором следует увеличить квадрат


	 vect_2d search(); //Поиск минимума
 private:

	 vect_2d x0; //текущее приближение
	 func_2d min_f; //минимизиуемая функци
	 double min_f_val; //текущее значение минимума

	 int max_iters; //Максимальное число смен прямоугольников

	 vect_2d left_bottom, right_top; //Вершины прямоугольника
	 double hx, hy; //Длины старон прямоугольника
	 void change_rect(); //Сменить прямоугольник
	 double eps; //Точность (минимальная длина стороны прямоугольника)
	 double P; //вероятность, нахождение минимума с нужной точностью
	 double eps_grow; //Значеине разности функций, при котором следует увеличить квадрат

	 int tests_N; //количество генерируеммых точек(в одном прямоуголльнике)
	 random_generator rand_gen; //генератор случайных чисел

	 bool check_bounds(vect_2d ch_p); //Проверяет принадлежит ли точка ch_p указанным ограничениям, если точка в области - true, иначе - false

};