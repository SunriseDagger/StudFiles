#include "grid_gen.h"

//======================================= Основная часть ======================================= 

int grid_gen::generate_unreg_grid_pr(double a, double b, double h_min, double k, double *&grid_mass){
	int n; //число узлов сетки

	n = nodes_number_gen_unreg(a, b, h_min, k);
	
	grid_mass = new double [n];
	double h = h_min; //текущий шаг
	grid_mass[0] = a;
	for(int i = 1; i < n; i++){
		grid_mass[i] = grid_mass[i-1] + h;
		h *= k;
	}
	grid_mass[n-1] = b;

	return n;	
}

int grid_gen::generate_reg_grid_pr(double a, double b, double h, double *&grid_mass){
	int n; //число узлов сетки

	n = (b-a)/h;
	n++;

	grid_mass = new double [n];
	grid_mass[0] = a;
	for(int i = 1; i < n; i++){
		grid_mass[i] = grid_mass[i-1] + h;
	}
	grid_mass[n-1] = b;

	return n;

}

int grid_gen::nodes_number_gen_unreg(double a, double b, double h_min, double k){
	double n_d = log(1 - (b-a)*(1-k)/h_min) / log(k);
	int n = n_d;
	n += 2;
	return n;
}

bool grid_gen::check_data(double a, double b, double h_min, double k){
	if(a > b || h_min <= 0 || k <= 1) return false; //проверка корректности данных
	return true;
}


//========================================== Интерефейс ============================================ 

bool grid_gen::generate_unreg_grid(double a, double b, double h_min, double k, int &n, double *&grid_mass){
	if(!check_data(a, b, h_min, k)) return false; //проверка корректности данных

	n = generate_unreg_grid_pr(a, b, h_min, k, grid_mass);

	return true;
}

bool grid_gen::generate_unreg_grid(double a, double b, double h_min, double k, dvector &grid_vector){
	if(!check_data(a, b, h_min, k)) return false; //проверка корректности данных

	double *grid_mass;

	int n = generate_unreg_grid_pr(a, b, h_min, k, grid_mass);

	grid_vector.clear();
	for(int i = 0; i < n; i++)
		grid_vector.push_back(grid_mass[i]);

	delete[] grid_mass;

	return true;
}

bool grid_gen::generate_unreg_grid(double a, double b, double h_min, double k, std::string file_name){
	if(!check_data(a, b, h_min, k)) return false; //проверка корректности данных

	double *grid_mass;
	int n = generate_unreg_grid_pr(a, b, h_min, k, grid_mass);

	FILE *out_f = fopen(file_name.c_str(), "w");

	fprintf(out_f, "%d\n", n);
	for(int i = 0; i < n; i++){
		fprintf(out_f, "%.15lf\n", grid_mass[i]);
	}

	delete[] grid_mass;
	fclose(out_f);

	return true;
}

bool grid_gen::generate_reg_grid(double a, double b, double h, int &n, double *&grid_mass){
	if(a > b || h <= 0) return false; //проверка корректности данных

	n = generate_reg_grid_pr(a, b, h, grid_mass);

	return true;
}

bool grid_gen::generate_reg_grid(double a, double b, double h, dvector &grid_vector){
	if(a > b || h <= 0) return false; //проверка корректности данных

	double *grid_mass;

	int n = generate_reg_grid_pr(a, b, h, grid_mass);

	grid_vector.clear();
	for(int i = 0; i < n; i++)
		grid_vector.push_back(grid_mass[i]);

	delete[] grid_mass;

	return true;
}

bool grid_gen::generate_reg_grid(double a, double b, double h, std::string file_name){
	if(a > b || h <= 0) return false; //проверка корректности данных

	double *grid_mass;
	int n = generate_reg_grid_pr(a, b, h, grid_mass);

	FILE *out_f = fopen(file_name.c_str(), "w");

	fprintf(out_f, "%d\n", n);
	for(int i = 0; i < n; i++){
		fprintf(out_f, "%.15lf\n", grid_mass[i]);
	}

	delete[] grid_mass;
	fclose(out_f);

	return true;
}

//========================================== Для вложенных сеток ============================================ 
bool grid_gen::generate_unreg_grid_ins(double a, double b, double h_min, double k, string file_name){
	double k1 = sqrt(k);
	double h_min1 = h_min/(1+k1);
	return generate_unreg_grid(a, b, h_min1, k1, file_name);
}

bool grid_gen::generate_unreg_grid_ins(double a, double b, double h_min, double k, dvector &grid_vector){
	double k1 = sqrt(k);
	double h_min1 = h_min/(1+k1);
	return generate_unreg_grid(a, b, h_min1, k1, grid_vector);
}

bool grid_gen::generate_unreg_grid_ins(double a, double b, double h_min, double k, int &n, double*& grid_mass){
	double k1 = sqrt(k);
	double h_min1 = h_min/(1+k1);
	return generate_unreg_grid(a, b, h_min1, k1, n, grid_mass);
}

bool grid_gen::generate_reg_grid_ins(double a, double b, double h, string file_name){
	return generate_reg_grid(a, b, h/2, file_name);
}

bool grid_gen::generate_reg_grid_ins(double a, double b, double h, dvector &grid_vector){
	return generate_reg_grid(a, b, h/2, grid_vector);
}

bool grid_gen::generate_reg_grid_ins(double a, double b, double h, int &n, double*& grid_mass){
	return generate_reg_grid(a, b, h/2, n, grid_mass);
}

