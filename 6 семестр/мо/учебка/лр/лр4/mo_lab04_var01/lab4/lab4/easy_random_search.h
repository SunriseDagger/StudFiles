#pragma once

#include <stdio.h>
#include <omp.h> //пареллель!
#include <vector>
#include <algorithm>

using namespace std;

#include "random_generator.h"

//Класс, реализующий простейщий случайный
class easy_random_search{
 public:
	 void init_rectangle(vect_2d s_x0, double s_hx, double s_hy); //Установка центра прямоугольника и растояний от центра до края
	 void init_tests(double eps, double P); //Установка числа генерируемых точек (устанавливается точность и требуемая вероятность нахождения, число итераций высчитывается)
	 void init_func(func_2d s_func); //Установка искомой функции


	 vect_2d search(); //Поиск минимума
 private:

	 vect_2d x0; //текущее приближение
	 func_2d min_f; //минимизиуемая функци
	 double min_f_val; //текущее значение минимума

	 vect_2d left_bottom, right_top; //Вершины прямоугольника

	 long int tests_N; //количество генерируеммых точек
	 random_generator rand_gen; //генератор случайных чисел
};