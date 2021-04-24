#pragma once

#include "grid_gen.h"

//Вспомогательная структура для задания точек
struct gen_point{
	double x, y, z;

	gen_point(){
		x = y = z = 0;
	}

	gen_point(double set_x, double set_y){
		x = set_x;
		y = set_y;
		z = 0;
	} 

	gen_point(double set_x, double set_y, double set_z){
		x = set_x;
		y = set_y;
		z = set_z;
	} 
};

//Генератор двухмерных прямоугольных секток

class grid_gen_rect{
 public:
	 //===================Интерфейс класса геренации сетки===================
	 //============ Генерация неравномерной сетки ============
	 //left_bottom - нижний левый край прямоугольника
	 //right_top - правый верхний край прямоугольника
	 //h_xmin, h_ymin - минимальный шаг по x и по y соответственно
	 //k_x, k_y - коэффициенты разрядки по каждой переменной
	 //Возвращает true, если сетка может быть сгенерированна, false - в противном случае
	 //n - возвращаемый параметр, число узлов сетки
	static bool generate_unreg_grid(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, string file_name);  //генерация в файл с именем file_name
	static bool generate_unreg_grid(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, int &n, double  **&grid_mass);  //генерация в двумерный массив grid_vector

	//============ Генерация равномерной сетки ===========
	static bool generate_reg_grid(gen_point left_bottom, gen_point right_top, double h_x, double h_y, string file_name);
	static bool generate_reg_grid(gen_point left_bottom, gen_point right_top, double h_x, double h_y, int &n, double  **&grid_mass);

	
	//============ Генерация вложенных сеток ============
	//Для неравномерных
	static bool generate_unreg_grid_ins(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, string file_name);
	static bool generate_unreg_grid_ins(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, int &n, double  **&grid_mass);

	//Для равномерных
	static bool generate_reg_grid_ins(gen_point left_bottom, gen_point right_top, double h_x, double h_y, string file_name);
	static bool generate_reg_grid_ins(gen_point left_bottom, gen_point right_top, double h_x, double h_y, int &n, double  **&grid_mass);
 
 private:
	static int generate_unreg_grid_pr(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, double **&grid_mass); 
   //grid_mass - здесь будет хранится сетка
	static int generate_reg_grid_pr(gen_point left_bottom, gen_point right_top, double h_x, double h_y, double **&grid_mass); 
   //grid_mass - здесь будет хранится сетка

};
