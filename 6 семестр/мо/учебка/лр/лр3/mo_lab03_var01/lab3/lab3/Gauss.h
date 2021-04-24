#pragma once

#include <stdio.h>

#include "functor.h"

//Метод Гаусса, для оптимизции функции двух аргументов с ограничениями, в качестве одномерного поиска используется метод Фиббоначи

class Gauss{
 public:
	Gauss(){} //Конструктор по-умолчанию
	void set_func(functor set_min_f); //Установка минимизируемой функции
	void set_start(vect_2d set_x0); //Установка начального приближения
	void set_eps(double set_eps_G); //Установка требуемой точности
	vect_2d find_min(); //Поиск минимума
 private:
	 functor min_f; //Минимизируемая функция
	 vect_2d x_k; //Приближение
	 double eps_G; //точность
	 int func_calc; //количество вычислений функции

	 //x_i - это номер компоненты, которая сейчас минимизируется
	 double Fib(double a, double b, int x_i); // Используемый одномерный метод поиска - метод Фибоначчи
	 void find_area(double x0, double& a, double& b, int x_i); // Используемый метод для определения области одномреного минимума
	 double one_min_f(double x0, int x_i); // Функция для одномерной минимизации
	

};