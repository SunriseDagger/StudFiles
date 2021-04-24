#pragma once

#include <math.h>
#include <stdio.h>

#include <string>
#include <vector>

using namespace std;

typedef vector<double> dvector; 

// Генератор одномерных сеток

class grid_gen{
 public:
	 //===================Интерфейс класса геренации сетки===================
	 //============ Генерация неравномерной сетки ============
	 //a - начало отрезка
	 //b - конец отрезка
	 //h_min - минимальный шаг
	 //k - коэффициент разрядки
	 //Возвращает true, если сетка может быть сгенерированна, false - в противном случае
	 //n - возвращаемый параметр, число узлов сетки
	static bool generate_unreg_grid(double a, double b, double h_min, double k, string file_name); //генерация в файл с именем file_name
	static bool generate_unreg_grid(double a, double b, double h_min, double k, dvector &grid_vector); //генерация в вектор grid_vector
	static bool generate_unreg_grid(double a, double b, double h_min, double k, int &n, double*& grid_mass); //генерация в массив grid_vector

	//============ Генерация равномерной сетки ============
	//По сути, в принцепи тоже самое
	static bool generate_reg_grid(double a, double b, double h, string file_name); //генерация в файл с именем file_name
	static bool generate_reg_grid(double a, double b, double h, dvector &grid_vector); //генерация в вектор grid_vector
	static bool generate_reg_grid(double a, double b, double h, int &n, double*& grid_mass); //генерация в массив grid_vector

	//============ Генерация вложенных сеток ============
	//Для неравномерных
	static bool generate_unreg_grid_ins(double a, double b, double h_min, double k, string file_name);
	static bool generate_unreg_grid_ins(double a, double b, double h_min, double k, dvector &grid_vector);
	static bool generate_unreg_grid_ins(double a, double b, double h_min, double k, int &n, double*& grid_mass);

	//Для равномерных
	static bool generate_reg_grid_ins(double a, double b, double h, string file_name);
	static bool generate_reg_grid_ins(double a, double b, double h, dvector &grid_vector);
	static bool generate_reg_grid_ins(double a, double b, double h, int &n, double*& grid_mass);

	 //============ Получение дополнительной информации ============
	static int nodes_number_gen_unreg(double a, double b, double h_min, double k); //Поличение количества узлов при указанных выходных данных
	static int nodes_number_gen_reg(double a, double b, double h); //Поличение количества узлов при указанных выходных данных
	static bool check_data(double a, double b, double h_min, double k); //Проверяет возможно ли построить сетку с такими входыми данными, если можно - true,  если незлья - false


 private:

	 static int generate_unreg_grid_pr(double a, double b, double h_min, double k, double *&grid_mass); // сосбтвенно генерация сетки
	 //grid_mass - здесь будет хранится сетка

	 static int generate_reg_grid_pr(double a, double b, double h, double *&grid_mass); // сосбтвенно генерация сетки
	 //grid_mass - здесь будет хранится сетка

};


