#pragma once

#include <stdio.h>
#include <sstream>
#include <string>

#include "additional_classes.h"
#include "areas.h"
#include "CGM.h"
#include "Gauss.h"

using namespace area_3;
using namespace std;

struct first_boundary{
	int node_numb;
	int area;
};

//Класс для решения гиперболиечского уравнения
//Дискретизация по времени: неявная 4-х слойная схема
//Базисные функции: линейные на тетраэдрах

class Grid{
 public:

	 Grid();

	//Ввод
	 void input_nodes(FILE* nodes_f);
	 void input_elements(FILE* el_f);
	 void input_faces_s(FILE* face_f_sec);
	 void input_faces_t(FILE* faces_f_thi);
	 void input_fir_nodes(FILE *fir_f);

	 void input_time_data(FILE* time_d); //ввод информации о времени: начальный шаг и коэффциент разрядки
	 void input_start(FILE* st0, FILE* st1, FILE* st2);  //Ввод начальных условий на i-2, i-1 и i-ой итерациях

	 void input_el_areas(FILE *el_a_f);
	 void input_f_s_areas(FILE *fs_a_f);
	 void input_f_t_areas(FILE *ft_a_f);

	 double u_in_el(double x, double y, double z); //получение значения функции с текущего временого слоя в точке


	 void gen_SLAE_port(); //запуск генерации портрета СЛАУ
	 void gen_SLAE(); //запуск генерации СЛАУ
	 void solve_SLAE(); //запуск решения СЛАУ

	 void init_areas(FILE* elem_areas, FILE* fir_areas, FILE* sec_areas, FILE* thi_areas);

	 void output_weight(FILE* outp_f); //вывод текущего решения
	 void output_diff(FILE* outp_f);  //вывод текущей погрешности

 private:
	 node* nodes; //массив узлов
	 FE* elements; //массив элементов
	 face *faces_s, *faces_t; //массивы для вторых и третьих краевых
	 first_boundary* fir_b; //массив для первых краевых

	 double *weight_0; //веса с i-ой итерации
	 double *weight_1; //веса с (i-1)-ой итерации
	 double *weight_2; //веса с (i-2)-ой итерации

	 //Соответсвующие весам времена
	 double t0;
	 double t1;
	 double t2;
	 double t_new; //время на i+1 итерации
	 double k_time; //коэффциент разрядки по времени

	 int nodes_number, elements_number, faces_number_sec, faces_number_thi, first_number; //количество узлов, элементов и каждого из краевых условий

	 //Массивы для матрицы
	 int *gi, *gj;
	 double *gg, *di, *right_part;
	 int SLAE_el_numbers;
	 
	 SLAE_port_gen SLAE_gen; //класс для генерации портрета СЛАУ

	//Получение локальных матриц указанного элемента или грани
	 void gen_local_el(double **A_loc, double *b_loc, int el_n);
	 void gen_local_thi(double **A_loc, double *b_loc, int f_n);
	 void gen_local_sec(double *b_loc, int f_n);

	//Получение значения коэффицента на элементе
	 double get_lambda_value(int el_n);
	 double get_gamma_value(int el_n);
	 double get_sigma_value(int el_n, double t);
	 double get_epsilon_value(int el_n, double t);
	 double get_rp_value(int node_n);

	 //Получение значения коэффициента на грани
	 double get_betta_value(int f_n);
	 double get_u_betta_value(int f_n, int node_n);
	 double get_tetta_value(int f_n, int node_n);

	 //Получение значения первого краевого в узле
	 double get_func_value_fir(int node_n);

	//Получение площади грани
	 double get_face_mes(int boun, int f_n);

	//Нахождение номера элемента СЛАУ в указаном предстовлении	
	 int find_el_pos(int i, int j);

	 //вспомогетельные матрицы для формирования локальных
	 double **D, **alpha;

	 //Для доп задания
	 int sourse; //номер узла в котором источник

};
