#include "areas.h"


namespace area_1{
	string path = "time_test_1/";

	int lambda_n = 1;
	int betta_n = 0;
	int u_betta_n = 0;
	int tetta_n = 1;
	int fir_n = 1;

	
	 double *lambda_values;
	 double *betta_values;
	 func *gamma_values;
	 func *sigma_values;
	 func *epsilon_values;
	 func *u_betta_values;
	 func *tetta_values;
	 func *fir_values;
	
	double gamma(double x, double y, double z, double t){
		return 0;
	}

	double sigma(double x, double y, double z, double t){
		return 0;
	}

	double epsilon(double x, double y, double z, double t){
		return 0;
	}

	double firval0(double x, double y, double z, double t){
		return y*t;
	}

	double tetta0(double x, double y, double z, double t){
		return 0.0;
	}

	void init_area(){
		lambda_values = new double [lambda_n];
		gamma_values = new func [lambda_n];
		sigma_values = new func [lambda_n];
		epsilon_values = new func [lambda_n];
		betta_values = new double [betta_n];
		u_betta_values = new func [u_betta_n];
		tetta_values = new func [tetta_n];
		fir_values = new func [fir_n];

		lambda_values[0] = 1.0;
		gamma_values[0] = gamma;
		sigma_values[0] = sigma;
		epsilon_values[0] = epsilon;

		tetta_values[0] = tetta0;


		fir_values[0] = firval0;
	}


	double sol_func(double x, double y, double z, double t){
		return y*t;
	}

	double equation_rp(double x, double y, double z, double t){
		return 0;
	}

}

namespace area_2{
	string path = "time_test_2/";

	int lambda_n = 1;
	int betta_n = 0;
	int u_betta_n = 0;
	int tetta_n = 0;
	int fir_n = 1;

	
	 double *lambda_values;
	 double *betta_values;
	 func *gamma_values;
	 func *sigma_values;
	 func *epsilon_values;
	 func *u_betta_values;
	 func *tetta_values;
	 func *fir_values;
	
	double gamma(double x, double y, double z, double t){
		return 0.0;
	}

	double sigma(double x, double y, double z, double t){
		return 10;
	}

	double epsilon(double x, double y, double z, double t){
		return 0.1;
	}

	double firval0(double x, double y, double z, double t){
		return exp(4*t)+y;
	}

	void init_area(){
		lambda_values = new double [lambda_n];
		gamma_values = new func [lambda_n];
		sigma_values = new func [lambda_n];
		epsilon_values = new func [lambda_n];
		betta_values = new double [betta_n];
		u_betta_values = new func [u_betta_n];
		tetta_values = new func [tetta_n];
		fir_values = new func [fir_n];

		lambda_values[0] = 2.0;
		gamma_values[0] = gamma;
		sigma_values[0] = sigma;
		epsilon_values[0] = epsilon;

		fir_values[0] = firval0;
	}


	double sol_func(double x, double y, double z, double t){
		return exp(4*t)+y;
	}

	double equation_rp(double x, double y, double z, double t){
		return 41.6*x*exp(4*t);
	}

}


namespace area_3{
	string path = "time_test_3/";

	int lambda_n = 1;
	int betta_n = 0;
	int u_betta_n = 0;
	int tetta_n = 0;
	int fir_n = 1;

	
	 double *lambda_values;
	 double *betta_values;
	 func *gamma_values;
	 func *sigma_values;
	 func *epsilon_values;
	 func *u_betta_values;
	 func *tetta_values;
	 func *fir_values;

	double p = 1E-16;
	double G = 1;
	
	double gamma(double x, double y, double z, double t){
		return 0.0;
	}

	double sigma(double x, double y, double z, double t){
		return 0.0;
	}

	double epsilon(double x, double y, double z, double t){
		return p/G;
	}

	double firval0(double x, double y, double z, double t){
		return 0;
	}

	void init_area(){
		lambda_values = new double [lambda_n];
		gamma_values = new func [lambda_n];
		sigma_values = new func [lambda_n];
		epsilon_values = new func [lambda_n];
		betta_values = new double [betta_n];
		u_betta_values = new func [u_betta_n];
		tetta_values = new func [tetta_n];
		fir_values = new func [fir_n];

		lambda_values[0] = 1.0;
		gamma_values[0] = gamma;
		sigma_values[0] = sigma;
		epsilon_values[0] = epsilon;

		fir_values[0] = firval0;
	}


	double sol_func(double x, double y, double z, double t){
		/*double x0 = 0.600006000020000, y0 = 0.60000600002000, z0 = 0.600006000020000;
		double r = sqrt((x-x0)*(x-x0) + (y-y0)*(y-y0) + (z-z0)*(z-z0));
		double pi = 4*atan(1.0);
		double v = sqrt(G/p);

		return sin(t - r/v)/(4.0*pi*r);*/
		return 0.0;
	}

	double equation_rp(double x, double y, double z, double t){
		return 0;
	}


}
