#ifndef CRAND_H
#define CRAND_H

#include <stdlib.h>
#include "types.h"

typedef double(*func2lf)(vector2lf); //функция двух переменных

//Класс, реализующий генератор случайных двумерных точек в прямоугольнике
class Crand
{
 public:
     //Конструктуор по умолчанию - стандартное семя 0
     Crand(){
         srand(0);
     }

     //Конструктор с некоторым другим семенем
     Crand(unsigned int core){
         srand(core);
     }

     //Функция, генерирующая случайную точку в указанном прямоугольнике
     //left_bottom - нижняя левая вершина прямоугольника
     //right_top - правая верхняя вершина прямоугольника
     vector2lf generate(vector2lf left_bottom, vector2lf right_top)
     {
         //Два случайных числа в отрезке [0,1]
         double c_x = (rand()*1.0 + 1.0)/ RAND_MAX;
         double c_y = (rand()*1.0 + 1.0)/ RAND_MAX;

         //Предвиним числа в наш прямоугольник
         double h_x = fabs(right_top.x - left_bottom.x); //длина прямоугольника по оси x
         double h_y = fabs(right_top.y - left_bottom.y); //длина прямоугольника по оси y

         double x = c_x * h_x + left_bottom.x; //координата x генерируемой точки
         double y = c_y * h_y + left_bottom.y; //координата y генерируемой точки
         return vector2lf(x, y);
     }

};

#endif // CRAND_H
