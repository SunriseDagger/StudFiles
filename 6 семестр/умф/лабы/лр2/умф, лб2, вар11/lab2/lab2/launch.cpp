#include <windows.h>

#include "grid_gen.h"
#include "unlin_FEM.h"

double lam1(double x, double t){return x;}
double f1(double x, double t){return -1;}
double b_l1(double t){return 0;}
double b_r1(double t){return 1;}
double u1(double x, double t){return x;}

double lam2(double x, double t){return x;}
double f2(double x, double t){return  x*x*t*t - t;}
double b_l2(double t){return 0;}
double b_r2(double t){return t;}
double u2(double x, double t){return x*t;}

double lam3(double x, double t){return 1;}
double f3(double x, double t){return 0;}
double b_l3(double t){return t;}
double b_r3(double t){return t;}
double u3(double x, double t){return t;}

double lam4(double x, double t){return x*t;}
double f4(double x, double t){return exp(-t)*t*(x*sin(x)-cos(x))-x*exp(-3*t)*cos(x)*cos(x)*sin(x);}
double b_l4(double t){return 0;}
double b_r4(double t){return 1;}
double b_r42(double t){return exp(-t)*(-t*cos((double)1.0) + sin((double)1.0));}
double u4(double x, double t){return exp(-t)*sin(x);}


int main(){
	
	
	grid_gen::generate_reg_grid_ins(0, 1, 1E-1, "grid1.txt");
	vector<double> start_gen;
	grid_gen::generate_reg_grid_ins(0, 1, 1E-1, start_gen);

	FILE* out_f = fopen("u0.txt", "w");
	for(int i = 0; i < start_gen.size(); i++)
		fprintf(out_f, "%.15lf\n", u4(start_gen[i], 0));
	fclose(out_f);

	unlin_FEM our_sol_si, our_sol_Newton;

	our_sol_si.init("grid1.txt", "u0.txt", "pars.txt");
	our_sol_si.init_coefs(lam4, f4);
	our_sol_si.init_sigma((double)0.0);
	our_sol_si.init_bounds(1, 3, b_l4, NULL, b_r4, b_r42);
	our_sol_si.set_analytic(u4);

	our_sol_Newton.init("grid1.txt", "u0.txt", "pars.txt");
	our_sol_Newton.init_coefs(lam4, f4);
	our_sol_Newton.init_sigma((double)0.0);
	our_sol_Newton.init_bounds(1, 3, b_l4, NULL, b_r4, b_r42);
	our_sol_Newton.set_analytic(u4);

	for(int i = 0 ; i < 7; i++){
		our_sol_si.simple_iteration_step();
		our_sol_Newton.Newton_step();
		our_sol_si.out_q("out_si.txt");
		our_sol_Newton.out_q("out_Newton.txt");
	}

	system("pause");


	return 0;
}