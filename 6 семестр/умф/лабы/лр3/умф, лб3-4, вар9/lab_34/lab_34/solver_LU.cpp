#include "solver_LU.h"

void solver_LU::init(int* s_ig, int* s_jg, double* s_gu, double* s_gl, double* s_di, int s_n){
	n = s_n;

	di = new double [n];
	ig = new int [n+1];

	//Перенос диагонали
	for(int i = 0; i < n; i++)
		di[i] = s_di[i];

	//Формируем новый массив ig
	ig[0] = 0;
	for(int i = 1; i <= n; i++){
		int k = s_ig[i] - s_ig[i-1]; //Количество не нулевых элементов в строке
		if(k > 0){ //Если такие есть
			int total_n = i - s_jg[s_ig[i-1]]; // вычисляем сколько всего элементов в строке, как в профильной
			ig[i] = ig[i-1] + total_n;
		}
		else
			ig[i] = ig[i-1];
	}



	gu = new double [ig[n]];
	gl = new double [ig[n]];

	//Формируем новые gl и gu

	for(int i = 0; i < n; i++){
		int j_s = ig[i]; // начало строки
		int j_e = ig[i+1]; //конец
		int column = i - (j_e - j_s); //номер текущей колонки
		int s_point = s_ig[i];
		for(int j = j_s; j < j_e; j++, column++){
			if(column == s_jg[s_point]){
				gu[j] = s_gu[s_point];
				gl[j] = s_gl[s_point];
				s_point++;
			}
			else{
				gu[j] = gl[j] = 0;
			}
		}
	}

	dec();
}

void solver_LU::set_rp(double *s_rp){
	rp = new double [n];
	for(int i = 0; i < n; i++)
		rp[i] = s_rp[i];
}

void solver_LU::solve(double *&solution, int &its){

	solution = new double [n];

	//Прямой ход
	for(int i = 0; i < n; i++){
		double sum = 0;
		int j_start = ig[i], j_end = ig[i+1];
		int vect_iter = i - (j_end - j_start);
		for(int j = j_start; j < j_end; j++, vect_iter++)
			sum += gl[j]*rp[vect_iter];
		rp[i] = (rp[i] - sum)/di[i];
	}

	//Обраный ход
	for(int i = n-1; i >= 0; i--){
		int j_start = ig[i], j_end = ig[i+1];
		int vect_iter = i - (j_end - j_start);
		for(int j = j_start; j<j_end; j++, vect_iter++)
			rp[vect_iter] -= gu[j]*rp[i];
	}

	for(int i = 0; i < n; i++)
		solution[i] = rp[i];

	its = -1;

}

void solver_LU::dec(){
	for(int i = 0; i < n; i++){
		int i0 = ig[i]; 
		int i1 = ig[i+1];
		int j = i - (i1-i0);
		double sd = 0;
		for(int m = i0; m < i1; m++,j++){
			double sl = 0;
			double su = 0;

			int j0 = ig[j]; 
			int j1 = ig[j+1];

			int mi = i0;
			int mj = j0;

			int kol_i = m - i0;
			int kol_j = j1 - j0;
			int kol_r = kol_i - kol_j;
			if(kol_r < 0) mj -= kol_r;
			else mi += kol_r;
			


			for(mi=mi; mi<m; mi++,mj++){
				sl += gl[mi]*gu[mj];
				su += gu[mi]*gl[mj];
			}


			gl[m] = gl[m] - sl;
			gu[m] = (gu[m] - su ) / di[j];
			sd += gl[m]*gu[m];
		}

		di[i] = di[i] - sd;


	}
}



