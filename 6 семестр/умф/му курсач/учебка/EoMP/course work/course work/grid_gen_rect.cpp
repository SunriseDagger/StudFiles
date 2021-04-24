#include "grid_gen_rect.h"

//======================================= Основная часть ======================================= 

int grid_gen_rect::generate_unreg_grid_pr(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, double **&grid_mass){

	double* mass_x; //x-координаты точек
	int n_x; //количество точек на слое по x

	double* mass_y; //y-координаты точек
	int n_y; //количество точек на слое по y

	double a_x = left_bottom.x; //начало по x
	double b_x = right_top.x; //конец по x

	double a_y = left_bottom.y; //начало по y
	double b_y = right_top.y; //конец по y

	grid_gen::generate_unreg_grid(a_x, b_x, h_xmin, k_x, n_x, mass_x);
	grid_gen::generate_unreg_grid(a_y, b_y, h_ymin, k_y, n_y, mass_y);

	int n = n_x * n_y; //количество узлов сетки
	grid_mass = new double* [n];
	for(int i = 0; i < n; i++)
		grid_mass[i] = new double [2];

	for(int i = 0; i < n_x; i++){
		for(int j = 0; j < n_y; j++){
			grid_mass[j*n_x + i][0] = mass_x[i];
			grid_mass[j*n_x + i][1] = mass_y[j];
		}
	}

	delete[] mass_x;
	delete[] mass_y;
	return n;
}

int grid_gen_rect::generate_reg_grid_pr(gen_point left_bottom, gen_point right_top, double h_x, double h_y, double **&grid_mass){
	double* mass_x; //x-координаты точек
	int n_x; //количество точек на слое по x

	double* mass_y; //y-координаты точек
	int n_y; //количество точек на слое по y

	double a_x = left_bottom.x; //начало по x
	double b_x = right_top.x; //конец по x

	double a_y = left_bottom.y; //начало по y
	double b_y = right_top.y; //конец по y

	grid_gen::generate_reg_grid(a_x, b_x, h_x, n_x, mass_x);
	grid_gen::generate_reg_grid(a_y, b_y, h_y, n_y, mass_y);

	int n = n_x * n_y; //количество узлов сетки
	grid_mass = new double* [n];
	for(int i = 0; i < n; i++)
		grid_mass[i] = new double [2];

	for(int i = 0; i < n_x; i++){
		for(int j = 0; j < n_y; j++){
			grid_mass[j*n_x + i][0] = mass_x[i];
			grid_mass[j*n_x + i][1] = mass_y[j];
		}
	}

	delete[] mass_x;
	delete[] mass_y;
	return n;
}

//========================================== Интерефейс ============================================
bool grid_gen_rect::generate_unreg_grid(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, int &n, double  **&grid_mass){
	if(left_bottom.x < right_top.y || left_bottom.y < right_top.y || h_xmin < 0 || h_ymin < 0 || k_x <= 1 || k_y <= 1) return false;  //проверка корректности данных

	n = generate_unreg_grid_pr(left_bottom, right_top, h_xmin, h_ymin, k_x, k_y, grid_mass);

	return true;
}


bool grid_gen_rect::generate_unreg_grid(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, string file_name){
	if(left_bottom.x > right_top.x || left_bottom.y > right_top.y || h_xmin < 0 || h_ymin < 0 || k_x <= 1 || k_y <= 1) return false;  //проверка корректности данных

	double **grid_mass;
	int n = generate_unreg_grid_pr(left_bottom, right_top, h_xmin, h_ymin, k_x, k_y, grid_mass);

	FILE *out_f = fopen(file_name.c_str(), "w");

	fprintf(out_f, "%d\n", n);
	for(int i = 0; i < n; i++){
		fprintf(out_f, "%.15lf\t%.15lf\n", grid_mass[i][0], grid_mass[i][1]);
	}

	delete[] grid_mass;
	fclose(out_f);

	return true;
}

bool grid_gen_rect::generate_reg_grid(gen_point left_bottom, gen_point right_top, double h_x, double h_y, int &n, double  **&grid_mass){
	if(left_bottom.x < right_top.y || left_bottom.y < right_top.y || h_x < 0 || h_y < 0) return false;  //проверка корректности данных

	n = generate_reg_grid_pr(left_bottom, right_top, h_x, h_y, grid_mass);

	return true;
}


bool grid_gen_rect::generate_reg_grid(gen_point left_bottom, gen_point right_top, double h_x, double h_y, string file_name){
	if(left_bottom.x > right_top.x || left_bottom.y > right_top.y || h_x < 0 || h_y < 0) return false;  //проверка корректности данных

	double **grid_mass;
	int n = generate_reg_grid_pr(left_bottom, right_top, h_x, h_y, grid_mass);

	FILE *out_f = fopen(file_name.c_str(), "w");

	fprintf(out_f, "%d\n", n);
	for(int i = 0; i < n; i++){
		fprintf(out_f, "%.15lf\t%.15lf\n", grid_mass[i][0], grid_mass[i][1]);
	}

	delete[] grid_mass;
	fclose(out_f);

	return true;
}

//========================================== Для вложенных сеток ============================================ 
bool grid_gen_rect::generate_unreg_grid_ins(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, int &n, double  **&grid_mass){
	double k_x1 = sqrt(k_x), k_y1 = sqrt(k_y);
	double h_x1 = h_xmin/(1+k_x1), h_y1 = h_ymin/(1+k_y1);

	return generate_unreg_grid_ins(left_bottom, right_top, h_x1, h_y1, k_x1, k_y1, n, grid_mass);
}

bool grid_gen_rect::generate_unreg_grid_ins(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, string file_name){
	double k_x1 = sqrt(k_x), k_y1 = sqrt(k_y);
	double h_x1 = h_xmin/(1+k_x1), h_y1 = h_ymin/(1+k_y1);

	return generate_unreg_grid(left_bottom, right_top, h_x1, h_y1, k_x1, k_y1, file_name);
}

bool grid_gen_rect::generate_reg_grid_ins(gen_point left_bottom, gen_point right_top, double h_x, double h_y,  int &n, double  **&grid_mass){
	return generate_reg_grid(left_bottom, right_top, h_x/2, h_y/2, n, grid_mass);
}

bool grid_gen_rect::generate_reg_grid_ins(gen_point left_bottom, gen_point right_top, double h_x, double h_y, string file_name){
	return generate_reg_grid(left_bottom, right_top, h_x/2, h_y/2, file_name);
}

	