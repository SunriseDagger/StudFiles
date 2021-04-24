#ifndef CSRS_H
#define CSRS_H

#include <stdio.h>
#include <omp.h>
#include <vector>
#include <algorithm>

using namespace std;

#include "crand.h"
#include "types.h"


//Class simple random search
class Csrs
{
 public:
     void init_rectangle(vector2lf s_x0, double s_hx, double s_hy); //Установка центра прямоугольника и растояний от центра до края
     void init_tests(double eps, double P); //Установка числа генерируемых точек (устанавливается точность и требуемая вероятность нахождения, число итераций высчитывается)
     void init_func(func2lf s_func); //Установка искомой функции


     vector2lf search(); //Поиск минимума
 private:

     vector2lf x0; //текущее приближение
     func2lf min_f; //минимизиуемая функция
     double min_f_val; //текущее значение минимума

     vector2lf left_bottom, right_top; //Вершины прямоугольника

     long int tests_N; //количество генерируеммых точек
     Crand rand_gen; //генератор случайных чисел
};

#endif // CSRS_H
