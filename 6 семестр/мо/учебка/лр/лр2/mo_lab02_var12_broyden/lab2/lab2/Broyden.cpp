#include "Broyden.h"

void Broyden::init(func_2d set_f, grad_f set_g, double set_eps, int set_iter){
	min_f = set_f;
	min_f_grad = set_g;
	meth_eps = set_eps;
	max_iter = set_iter;
}

void Broyden::minimization(vect_2d x0, vect_2d &x_m){
	x_k = x0;
	bool end_cycle = false;
	etta_k = matrix_2d(1,0,0,1); //начальное приблежение - единичная матрица
	calc_count = 0;
	grad_f_k = min_f_grad(x_k);
	int iter = 0;
	FILE *out_f = fopen("bro.txt", "w");
	FILE *out_fc = fopen("broc.txt", "w");

	fprintf(out_fc,"%.15lf\t%.15lf\t%.15lf\n", x_k.x, x_k.y, -5.0);
	fprintf(out_f,"%d\t%.15lf\t%.15lf\t%.5e\t%d\n", -1, x_k.x, x_k.y, grad_f_k.norm(), calc_count);
	while(!end_cycle && iter < max_iter){

		if(iter == 11818)
			iter = iter;
		
		double a, b; //отрезок одномерной минимизации
		find_area(0, a, b); //назодим отрезок
		double lambda_k = Fib(a,b); //поличаем лямбду
		vect_2d x_k1 = x_k - (etta_k*grad_f_k)*lambda_k; //новое приближение
		vect_2d grad_f_k1 = min_f_grad(x_k1); //новый градиент
		vect_2d dg = grad_f_k1 - grad_f_k;
		vect_2d dx = x_k1 - x_k;
		vect_2d add_v = dx - etta_k*dg; //вспомогательный вектор
		if(add_v.norm() != 0){
			double denom = 1.0/(add_v * dg);

			//Комнотенты новой матрицы
			double a11 = add_v.x * add_v.x;
			double a12 = add_v.x * add_v.y;
			double a22 = add_v.y * add_v.y;

			matrix_2d d_etta = matrix_2d(a11,a12,a12,a22) * denom;

			etta_k = etta_k + d_etta;

			x_k = x_k1;
			grad_f_k = grad_f_k1;

			double g_norm = grad_f_k.norm();

			if(g_norm < meth_eps)
				end_cycle = true;


			printf("%d\t%.15lf\t%.15lf\t%.5e\t%d\r", iter, x_k.x, x_k.y, g_norm, calc_count);
			fprintf(out_f,"%d\t%.15lf\t%.15lf\t%.5e\t%d\n", iter, x_k.x, x_k.y, g_norm, calc_count);
			fprintf(out_fc,"%.15lf\t%.15lf\t%.15lf\n", x_k.x, x_k.y, -5.0);
			iter++;

			//Обновление метода
			if(iter%1000 == 0){
				etta_k = matrix_2d(1,0,0,1); //начальное приблежение - единичная матрица
				grad_f_k = min_f_grad(x_k);
			}
		}
		else{
			end_cycle = true;
			printf("\nVector add_v = 0");
		}


	};
	printf("\n");
	x_m = x_k;
}

double Broyden::one_min_f(double lambda){
	calc_count++;  //увеличиваем счётчик числа вычислений функции
	return min_f(x_k - (etta_k*grad_f_k)*lambda);
}

double Broyden::Fib(double a, double b){
	double eps = 1E-8;
	double x1, x2, f1, f2;
	double fib_max = (b-a)/eps;

	long long int add_fib;
	int n = 2;

	int point_num;

	vector<long long int> fib_numbers;
	fib_numbers.push_back(1);
	fib_numbers.push_back(1);

	do{
		add_fib = fib_numbers[n-1] + fib_numbers[n-2];
		fib_numbers.push_back(add_fib);
		n++;
	}while(fib_max > add_fib);

	n = fib_numbers.size() - 3;

	x1 = a + fib_numbers[n]*(b-a)/fib_numbers[n+2];
	x2 = a + b - x1;

	f1 = one_min_f(x1);
	f2 = one_min_f(x2);

	if(f1 < f2){
		b = x2;
		x2 = x1;
		f2 = f1;
		point_num = 1;
	}
	else{
		a = x1;
		x1 = x2;
		f1 = f2;
		point_num = 2;
	}


	const int true_iters = n;

	for(int k = 1; k < true_iters; k++){
		switch(point_num){
				case 1:{
					x1 = a + fib_numbers[n-k+1]*(b-a)/fib_numbers[n-k+3];
					f1 = one_min_f(x1);
					 }break;
				case 2:{
					x2 = a + fib_numbers[n-k+2]*(b-a)/fib_numbers[n-k+3];
					f2 = one_min_f(x2);
				}break;
		};

		if(f1 < f2){
			b = x2;
			x2 = x1;
			f2 = f1;
			point_num = 1;
		}
		else{
			a = x1;
			x1 = x2;
			f1 = f2;
			point_num = 2;
		}

	}

	return (a+b)/2 ;
}

void Broyden::find_area(double x0, double& a, double& b){
		double delta = 1E-5;
		double f0 = one_min_f(x0);
		double x1 = x0+delta;
		double f1 = one_min_f(x1);
		double h = delta;
		int k = 1;
	
		if(f0 < f1){
			x1 = x0 - delta;
			h *= -1;
		} 

		bool end_cycle = false;

		while(!end_cycle){
			h *= 2;
			x0 = x1 + h;
			f0 = one_min_f(x0);

			k++;

			if(f1 > f0){
				x1 = x0;
				f1 = f0;
			}
			else{
				end_cycle = true;
				x1 = x0;
				x0 -= h + h/2;
			}

		};

		if(x1>x0){
			a = x0;
			b = x1;
		}
		else{
			a = x1;
			b = x0;
		}
}