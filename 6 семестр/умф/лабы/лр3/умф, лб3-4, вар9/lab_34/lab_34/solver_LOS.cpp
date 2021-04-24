#include "solver_LOS.h"

void LOS::init(int *s_ig, int *s_jg, double *s_gu, double *s_gl, double *s_di, int s_n){
	ig = s_ig;
	jg = s_jg;
	gu = s_gu;
	gl = s_gl;
	di = s_di;
	n = s_n;

	precond();
}

void LOS::set_rp(double *s_rp){
	rp = s_rp;
}
void LOS::precond(){

	double sum_l, sum_u, sum_d; //Промежуточные переменные, для вычисления сумм

	
	int copy_end = ig[n];

	Ll = new double [copy_end];
	Uu = new double [copy_end];
	Ld = new double [n];
	//Копируем старые в новые
	for(int i = 0; i < copy_end; i++){
		Ll[i] = gl[i];
		Uu[i] = gu[i];
	}

	for(int i = 0; i < n; i++)
		Ld[i] = di[i];

	for(int k = 1, k1 = 0; k <= n; k++, k1++){
		sum_d = 0;

		int i_s = ig[k1], i_e = ig[k];

		for(int m = i_s; m < i_e; m++){

			sum_l = 0; sum_u = 0;
			int j_s = ig[jg[m]], j_e = ig[jg[m]+1];
			for(int i = i_s; i < m; i++){
				for(int j = j_s ; j < j_e; j++){
					if(jg[i] == jg[j]){
						sum_l += Ll[i]*Uu[j];
						sum_u += Ll[j]*Uu[i];
						j_s++;
					}
				}
			}
			Ll[m] = Ll[m] - sum_l;
			Uu[m] = (Uu[m] - sum_u) / Ld[jg[m]];
			sum_d += Ll[m]*Uu[m];
		}
		Ld[k1] = Ld[k1] - sum_d;
	}
}

double LOS::dot_prod(double *a, double *b){
	double dp = 0;
	for(int i = 0; i < n; i++)
		dp += a[i]*b[i];
	return dp;
}

void LOS::mull_A(double *f, double *&x){
	for(int i = 0; i < n; i++){
		double v_el = f[i];
		x[i] = di[i]*v_el;
		for(int k = ig[i], k1 = ig[i+1]; k < k1; k++){
			int j = jg[k];
			x[i] += gl[k]*f[j];
			x[j] += gu[k]*v_el;
		}
	}
}

void LOS::solve_L(double *f, double *&x){
	for(int k = 1, k1 = 0; k <= n; k++, k1++){
		double sum = 0;

		for(int i = ig[k1]; i < ig[k]; i++)
			sum += Ll[i]*x[jg[i]];

		x[k1] = (f[k1] - sum)/Ld[k1];
	}
}

void LOS::solve_U(double *f, double *&x){

	double* f1 = new double [n];
	for(int i = 0; i < n; i++)
		f1[i] = f[i];

	for(int k = n, k1 = n-1; k > 0; k--, k1--){

		x[k1] = f1[k1]/Ld[k1];
		double v_el = x[k1];

		for(int i = ig[k1]; i < ig[k]; i++)
			f1[jg[i]] -= Uu[i]*v_el;
	}

	delete[] f1;
}

void LOS::solve(double *&solution, int &its){

	//Параметры решателя
	int max_iter = 100000;
	double eps = 1E-16;
	double end_cycle = false;

	//Норма правой части, для выхода
	double rp_norm = sqrt(dot_prod(rp, rp));

	//Начинаем решение
	double* x0 = new double [n]; //Приближение
	for(int i = 0; i < n; i++)
		x0[i] = 0;

	solution = new double [n];
	double* r = new double [n]; //Вектор невязки
	double* z = new double [n];
	double* p = new double [n];
	double* s = new double [n]; //Вспомогательный вектор
	double* t = new double [n]; //Вспомогательный вектор

	//r0 = L^(-1) * (f - Ax0)
	mull_A(x0, s);
	for(int i = 0; i < n; i++)
		s[i] = rp[i] - s[i];
	solve_L(s, r);

	//z0 = U^(-1)r0
	solve_U(r, z);

	//p0 = L^(-1)Az0
	mull_A(z, s);
	solve_L(s, p);

	int iter;

	for(iter = 0; iter < max_iter && !end_cycle; iter++){
		double discr = sqrt(dot_prod(r, r)); // Абсолютная невязка
		if( discr / rp_norm > eps){ //Проверка условия выхода
			double dot1 = dot_prod(p, p); //(p[k-1], p[k-1])
			double alpha = dot_prod(p ,r) / dot1; //a = (p[k-1], r[k-1]) / (p[k-1], p[k-1])

			for(int i = 0; i < n; i++){
				x0[i] = x0[i] + alpha*z[i]; //x[k] = x[k-1] + a*z[k-1]
				r[i] = r[i] - alpha*p[i]; //r[k] = r[k-1] - a*p[k-1]
			}
			//betta = -(p[k-1], L^(-1)*A*U^(-1)r[k]) / (p[k-1], p[k-1])

			solve_U(r, s); // s = U^(-1)r[k]
			mull_A(s, t);
			solve_L(t, t);
			double betta = - dot_prod(p, t) / dot1;

			for(int i = 0; i < n; i++){
				z[i] = s[i] + betta * z[i]; // z[k] = U^(-1)r[k] + b*z[k-1]
				p[i] = t[i] + betta * p[i]; // p[k] = L^(-1)*A*U^(-1)r[k] + b*p[k-1]
			}

			if(iter % n == 0){ //Обновление метода
				//r0 = L^(-1) * (f - Ax0)
				mull_A(x0, s);
				for(int i = 0; i < n; i++)
					s[i] = rp[i] - s[i];
				solve_L(s, r);

				//z0 = U^(-1)r0
				solve_U(r, z);

				//p0 = L^(-1)Az0
				mull_A(z, s);
				solve_L(s, p);
			}


		}
		else{
			end_cycle = true;
		}
	}

	for(int i = 0 ; i < n; i++)
		solution[i] = x0[i];

	its = iter;

	//И отчишаем память
	delete[] x0;
	delete[] p;
	delete[] r;
	delete[] z;
	delete[] s;
	delete[] t;

}

