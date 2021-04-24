#ifndef CBSS_H
#define CBSS_H

#include <stdio.h>

#include <omp.h>
#include <vector>
#include <algorithm>

#include "crand.h"

using namespace std;

//Класс, реализующий метод наилучшей пробы с направляющим гиперквадратом, в области
//x - y <= 1
//0 <= x <= 4

//Class best sample search
class Cbss
{
 public:
     void init_rectangle(vector2lf s_x0, double s_hx, double s_hy); //Установка центральной точки прямоугольника и начальных шагов по сеткам
     void init_tests(double s_eps, double s_P); //Установка числа генерируемых точек(оно вычиялется каждый раз через точность, вероятность и площадь прямоугольника)
     void init_func(func2lf s_func); //Установка искомой функции
     void init_max_iters(int s_max_iters = 100); //Установка максимального числа итераций
     void init_eps_grow(double s_eps_grow = 1E-1); //Установить значение разности функций, при котором следует увеличить квадрат

     vector2lf search(); //Поиск минимума
 private:

     vector2lf x0; //текущее приближение
     func2lf min_f; //минимизиуемая функция
     double min_f_val; //текущее значение минимума

     int max_iters; //Максимальное число смен прямоугольников

     vector2lf left_bottom, right_top; //Вершины прямоугольника
     double hx, hy; //Длины старон прямоугольника
     void change_rect(); //Сменить прямоугольник
     double eps; //Точность (минимальная длина стороны прямоугольника)
     double P; //вероятность, нахождение минимума с нужной точностью
     double eps_grow; //Значеине разности функций, при котором следует увеличить квадрат

     int tests_N; //количество генерируеммых точек(в одном прямоуголльнике)
     Crand rand_gen; //генератор случайных чисел

     bool check_bounds(vector2lf ch_p); //Проверяет принадлежит ли точка ch_p указанным ограничениям, если точка в области - true, иначе - false

};

#endif // CBSS_H
