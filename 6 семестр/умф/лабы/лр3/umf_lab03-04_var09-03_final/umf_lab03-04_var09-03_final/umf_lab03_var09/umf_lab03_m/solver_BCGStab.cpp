#include "solver_BCGStab.h"

void BCGStab::init(int *s_ig, int *s_jg, double *s_gu, double *s_gl, double *s_di, int s_n){
	ig = s_ig;
	jg = s_jg;
	gu = s_gu;
	gl = s_gl;
	di = s_di;
	n = s_n;

	precond();
}

void BCGStab::set_rp(double *s_rp){
	rp = s_rp;
}
void BCGStab::precond(){

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

double BCGStab::dot_prod(double *a, double *b){
	double dp = 0;
	for(int i = 0; i < n; i++)
		dp += a[i]*b[i];
	return dp;
}

void BCGStab::mull_A(double *f, double *&x){
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

void BCGStab::solve_L(double *f, double *&x){
	for(int k = 1, k1 = 0; k <= n; k++, k1++){
		double sum = 0;

		for(int i = ig[k1]; i < ig[k]; i++)
			sum += Ll[i]*x[jg[i]];

		x[k1] = (f[k1] - sum)/Ld[k1];
	}
}

void BCGStab::solve_U(double *f, double *&x){

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

void BCGStab::solve(double *&solution, int &its){

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
	double* r0 = new double [n]; //Вектор начальной невязки
	double* z = new double [n];
	double* p = new double [n];
	double* s = new double [n]; //Вспомогательный вектор
	double* t = new double [n]; //Вспомогательный вектор
	double* w = new double [n]; //Вспомогательный вектор

	//r0 = f - Ax0
	//z0 = r0
	mull_A(x0, s);
	for(int i = 0; i < n; i++)
		w[i] = rp[i] - s[i];
	solve_L(w, r0);
	for(int i = 0; i < n; i++)
		r[i] = z[i] = r0[i];

	double dot1, dot2;
	dot1 = dot_prod(r0, r0);

	int iter;

	for(iter = 0; iter < max_iter && !end_cycle; iter++){
		double discr = sqrt(dot_prod(r, r)); // Абсолютная невязка
		if(discr / rp_norm > eps){ //Проверка условия выхода
			//s = L^(-1)AU^(-1)z[k-1]
			solve_U(z, s);
			mull_A(s, t);
			solve_L(t, s);
			double alpha = dot1 / dot_prod(r0, s); // a = (r[k-1], r[0]) / (r[0], L^(-1)AU^(-1)z[k-1])

			for(int i = 0; i < n; i++)
				p[i] = r[i] - alpha*s[i]; //p[k] = r[k-1] - a*L^(-1)AU^(-1)z[k-1]

			//t = L^(-1)AU^(-1)p[k]
			solve_U(p, t);
			mull_A(t, w);
			solve_L(w, t);

			double gamma = dot_prod(p, t) / dot_prod(t, t); //g = (p[k], L^(-1)AU^(-1)p[k]) / (L^(-1)AU^(-1)p[k], L^(-1)AU^(-1)p[k])
			
			for(int i = 0; i < n; i++){
				x0[i] = x0[i] + alpha*z[i] + gamma*p[i]; //x[k] = x[k-1] + a*z[k-1] + g*p[k]
				w[i] = r[i];
				r[i] = p[i] - gamma*t[i]; //r[k] = p[k] - g*L^(-1)AU^(-1)p[k]
			}

			dot2 = dot_prod(r, r0); // (r[k], r[0])

			double betta = alpha*dot2 / (gamma * dot1); //b = a*(r[k],r[0]) / (g*(r[k-1], r[0])
			dot1 = dot2;

			for(int i = 0; i < n; i++)
				z[i] = r[i] + betta*w[i] - betta*gamma * s[i]; //z[k] = r[k] + b*r[k-1] - b*g*L^(-1)AU^(-1)z[k-1]

		}
		else{
			end_cycle = true;
		}
	}


	solve_U(x0, solution);
	its = iter;

	//И отчишаем память
	delete[] x0;
	delete[] p;
	delete[] r;
	delete[] z;
	delete[] s;
	delete[] t;
	delete[] w;
}

