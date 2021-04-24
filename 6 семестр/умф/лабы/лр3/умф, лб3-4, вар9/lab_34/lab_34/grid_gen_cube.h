#pragma once

#include "grid_gen_rect.h"

//Генератор трёхмерных паралелипипедальных сеток, с оринтацией на МКЭ

class grid_gen_cube{
 public:
	 //===================Интерфейс класса геренации сетки===================
	 //============ Генерация неравномерной сетки ============
	 //left_bottom - нижний левый край параллепипеда
	 //right_top - правый верхний край параллепипеда
	 //h_xmin, h_ymin, h_zmin - минимальный шаг по x, y и z соответственно
	 //k_x, k_y, k_z - коэффициенты разрядки по каждой переменной
	 //Возвращает true, если сетка может быть сгенерированна, false - в противном случае
	 //n - возвращаемый параметр, число узлов сетки
	 static bool generate_unreg_grid(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, double k_x, double k_y, double k_z, string file_name);  //генерация в файл с именем file_name
	 static bool generate_unreg_grid_FEM(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, double k_x, double k_y, double k_z, string file_cords, string file_elements, string file_faces); 
	 //генерация МКЭ-сетки
	 //file_cords - количество узлов, затем - координаты
	 //file_elements - количество элементов, затем номера узлов элементов
	 //file_faces - количество граней, затем номера узлов граней

	 //============ Генерация равномерной сетки ============
	 static bool generate_reg_grid(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, string file_name);  //генерация в файл с именем file_name
	  static bool generate_reg_grid_FEM(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, string file_cords, string file_elements, string file_faces); 
 private:
	static int generate_unreg_grid_pr(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, double k_x, double k_y, double k_z, gen_point *&grid_mass); //собственно генерация сетки
	static int generate_reg_grid_pr(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, gen_point *&grid_mass); //собственно генерация сетки
	 //grid_mass - здесь будет хранится сетка
};