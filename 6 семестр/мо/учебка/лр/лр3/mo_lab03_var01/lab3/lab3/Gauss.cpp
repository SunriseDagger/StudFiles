#include "Gauss.h"

void Gauss::set_func(functor set_min_f){
	min_f = set_min_f;
}

void Gauss::set_start(vect_2d set_x0){
	x_k = set_x0;
}

void Gauss::set_eps(double set_eps_G){
	eps_G = set_eps_G;
}

vect_2d Gauss::find_min(){
	func_calc = 0;
	vect_2d arg_min; //точка минимума

	double func_prev; //значение функции в точке с предыдущей итерацией
	double func_new; //значение функции на текущей итерации
	double diff; //Разность между значениями функции на соседнийх итерациях

	double a_min, b_min; //отрезок для одномерной минимизациия
	int iter = 0; //счётчик итераций

	FILE* out_tr = fopen("trajectory.txt", "w"); //Данные для вывода траектории(для графика)
	FILE* out_f = fopen("iters_info.txt", "w"); //Данные для вывода итераций(для табличек)
	

	func_prev = min_f(x_k);

	fprintf(out_tr, "%.15lf\t%.15lf\t%.15lf\n", x_k.x, x_k.y, min_f.main_val(x_k));
	fprintf(out_f, "%d\t%.15lf\t%.15lf\t%.15lf\t%.3e\t%d\n", -1, x_k.x, x_k.y, min_f.main_val(x_k), 0.0, func_calc);
	do{
		for(int i = 0; i < N_dim; i++){
			find_area(x_k[i], a_min, b_min, i);
			x_k[i] = Fib(a_min, b_min, i);
		}
		func_new = min_f(x_k);
		diff = fabs(func_new - func_prev);
		func_prev = func_new;

		fprintf(out_tr, "%.15lf\t%.15lf\t%.15lf\n", x_k.x, x_k.y, min_f.main_val(x_k));
		fprintf(out_f, "%d\t%.15lf\t%.15lf\t%.15lf\t%.3e\t%d\n", iter, x_k.x, x_k.y, min_f.main_val(x_k), diff, func_calc);
		printf("%5d)%.15lf\t%.15lf\t%.3e\r",iter, x_k.x, x_k.y, diff);
		iter++;
		if(!min_f.check_penalty(x_k))
			min_f.panalty_grow();  //для штрафных увеличиваем, для барьерных уменьшаем

	}while(diff > eps_G);

	printf("\n");


	arg_min = x_k;

	fclose(out_tr);
	fclose(out_f);

	return arg_min;

}

double Gauss::one_min_f(double x0, int x_i){
	vect_2d arg1; //аргумент для минимизациия
	for(int i = 0; i < N_dim; i++){
		if(i == x_i)
			arg1[i] = x0;
		else
			arg1[i] = x_k[i];
	}

	return min_f(arg1);
}

double Gauss::Fib(double a, double b, int x_i){
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

	f1 = one_min_f(x1, x_i);
	f2 = one_min_f(x2, x_i);
	func_calc += 2;

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
					f1 = one_min_f(x1, x_i);
					 }break;
				case 2:{
					x2 = a + fib_numbers[n-k+2]*(b-a)/fib_numbers[n-k+3];
					f2 = one_min_f(x2, x_i);
				}break;
		};
		func_calc++;

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

void Gauss::find_area(double x0, double &a, double &b, int x_i){
		double delta = 1E-5;
		double f0 = one_min_f(x0, x_i);
		double x1 = x0+delta;
		double f1 = one_min_f(x1, x_i);
		double h = delta;
		int k = 1;

		func_calc += 2;
	
		if(f0 < f1){
			x1 = x0 - delta;
			h *= -1;
		} 

		bool end_cycle = false;

		while(!end_cycle){
			h *= 2;
			x0 = x1 + h;
			f0 = one_min_f(x0, x_i);
			func_calc++;

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