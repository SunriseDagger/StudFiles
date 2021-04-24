#include "unlin_FEM.h"

void unlin_FEM::init(string node_file, string u0_file, string info_file){
	FILE* node_f = fopen(node_file.c_str(), "r");

	//Считывание узлов
	fscanf(node_f,"%d",&nodes_n);
	nodes = new double [nodes_n];

	for(int i = 0; i < nodes_n; i++)
		fscanf(node_f, "%lf", &nodes[i]);

	fclose(node_f);
	
	//Генерация элементов
	elements_n = nodes_n - 1;
	elements = new element [elements_n];

	for(int i = 0; i < elements_n; i++){
		elements[i].node1 = i;
		elements[i].node2 = i+1;
	}
	
	//Инициализация начальных условий
	q_last = new double [nodes_n];
	q_new = new double [nodes_n];
	q_temp = new double [nodes_n];
	q_min = new double [nodes_n];
	q_min_rp = new double [nodes_n];
	q_min_R = new double [nodes_n];

	FILE* u0_f = fopen(u0_file.c_str(), "r");

	for(int i = 0; i < nodes_n; i++)
		fscanf(u0_f, "%lf", &q_last[i]);
	fclose(u0_f);

	//Инициализация информации о сетки по времени
	FILE* info_f = fopen(info_file.c_str(), "r");
	fscanf(info_f,"%lf %lf %lf %d %lf", &t_now, &h_t, &k_t, &iter_max, &eps);
	fclose(info_f);

	//Инициализация матрицы
	dia_0 = new double [nodes_n];
	dia_p1 = new double [nodes_n - 1];
	dia_m1 = new double [nodes_n - 1];
}

void unlin_FEM::init_coefs(func_t set_lambda, func_t set_f){
	lambda = set_lambda;
	f = set_f;
}

void unlin_FEM::init_sigma(double set_c){
	sigma_c = set_c;
}

void unlin_FEM::set_analytic(func_t set_u){
	analytic = set_u;
}

void unlin_FEM::init_bounds(int set_b_l, int set_b_r, func bound_l1, func bound_l2, func bound_r1, func bound_r2){
	//Вот тут щас будет весело
	bound_type_left = set_b_l;
	bound_type_right = set_b_r;

	//Разбираемся с левыми краевыми
	switch(bound_type_left){
		case 1: case 2: {
			bounds_l = new func [1];
			bounds_l[0] = bound_l1;
		}break;
		case 3: {
			bounds_l = new func [2];
			bounds_l[0] = bound_l1;
			bounds_l[1] = bound_l2;
		}break;
	};
		
	//Разбираемся с правыми краевыми
	switch(bound_type_right){
		case 1: case 2: {
			bounds_r = new func [1];
			bounds_r[0] = bound_r1;
		}break;
		case 3: {
			bounds_r = new func [2];
			bounds_r[0] = bound_r1;
			bounds_r[1] = bound_r2;
		}break;
	};
}

double unlin_FEM::get_current_time(){
	return t_now;
}

double unlin_FEM::sigma(double u_x, double x){
	return (u_x*u_x + sigma_c)*x;
}

double unlin_FEM::sigma_ux(double u_x, double x){
	return 2*x*u_x;
}

void unlin_FEM::simple_iteration_step(){
	
	//В качестве начального приближения берём решение на предыдушем слое
	for(int i = 0; i < nodes_n; i++)
		q_new[i] = q_last[i];

	t_new = t_now + h_t; //новое время

	//Общий цикл, по итерациям
	bool end_cycle = false;
	for(int iter = 0; iter < iter_max && !end_cycle; iter++){

		//Обнуление матрицы
		for(int i = 0; i < elements_n; i++){
			dia_0[i] = 0;
			dia_p1[i] = 0;
			dia_m1[i] = 0;
			q_temp[i] = 0;
		}
		dia_0[nodes_n-1] = 0;
		q_temp[nodes_n-1] = 0;

		//Генерация матрицы
		matrix_gen_si(q_new, q_temp);

			//Решаем слау
		solve_LU(dia_0, dia_p1, dia_m1, q_temp, nodes_n);

		//Релаксация
		double w = w_min();
		for(int i = 0; i < nodes_n; i++)
			q_temp[i] = w*q_temp[i] + (1-w)*q_new[i];

		//Проверка - нужно ли выходит из цикла
		//В q_min_R запишим вектор невязки, в q_new - вектор праовй части
		for(int i = 0; i < nodes_n; i++)
			q_new[i] = 0;

		//Обнуление матрицы
		for(int i = 0; i < elements_n; i++){
			dia_0[i] = 0;
			dia_p1[i] = 0;
			dia_m1[i] = 0;
		}
		dia_0[nodes_n-1] = 0;
	
		matrix_gen_si(q_temp, q_new);

		//Вычисление невязки: A(q)*q - b(q)
		q_min_R[0] = dia_0[0]*q_temp[0] + dia_p1[0]*q_temp[1];
		q_min_R[elements_n] = dia_0[elements_n]*q_temp[elements_n] + dia_m1[elements_n-1]*q_temp[elements_n-1];

		for(int i = 1; i < elements_n; i++)
			q_min_R[i] = dia_0[i]*q_temp[i] + dia_p1[i]*q_temp[i+1] + dia_m1[i-1]*q_temp[i-1];

		double norm1 = norm_q_dif(q_min_R, q_new);
		double norm2 = norm_q(q_new);
		double nev = norm1/norm2;

		if(nev < eps)
				end_cycle = true;
	
		//Переброс указателей
		double *ch_p;
		ch_p = q_new;
		q_new = q_temp;
		q_temp = ch_p;

		printf("SI\tTime:\t%.15lf\tIter:\t%d\tNev:\t%.3e\tRelax:\t%.3e\r",t_new, iter, nev, w);

	}

	printf("\n");

	//Когда досчитали опять меняем, только уже переходим на новый слой
	double* ch_pt;
	ch_pt = q_new;
	q_new = q_last;
	q_last = ch_pt;

	t_now = t_new;
	h_t *= k_t;
}

void unlin_FEM::Newton_step(){


	//В качестве начального приближения берём решение на предыдушем слое
	for(int i = 0; i < nodes_n; i++)
		q_new[i] = q_last[i];

	t_new = t_now + h_t; //новое время

	//Общий цикл, по итерациям
	bool end_cycle = false;
	for(int iter = 0; iter < iter_max && !end_cycle; iter++){

		//Обнуление матрицы
		for(int i = 0; i < elements_n; i++){
			dia_0[i] = 0;
			dia_p1[i] = 0;
			dia_m1[i] = 0;
			q_temp[i] = 0;
		}
		dia_0[nodes_n-1] = 0;
		q_temp[nodes_n-1] = 0;

		//Генерация матрицы
		matrix_gen_Newton(q_new, q_temp);

			//Решаем слау
		solve_LU(dia_0, dia_p1, dia_m1, q_temp, nodes_n);

		//Релаксация
		double w = w_min();
		for(int i = 0; i < nodes_n; i++)
			q_temp[i] = w*q_temp[i] + (1-w)*q_new[i];

		//Проверка - нужно ли выходит из цикла
		//В q_min_R запишим вектор невязки, в q_new - вектор праовй части
		for(int i = 0; i < nodes_n; i++)
			q_new[i] = 0;

		//Обнуление матрицы
		for(int i = 0; i < elements_n; i++){
			dia_0[i] = 0;
			dia_p1[i] = 0;
			dia_m1[i] = 0;
		}
		dia_0[nodes_n-1] = 0;
	
		matrix_gen_si(q_temp, q_new);

		//Вычисление невязки: A(q)*q - b(q)
		q_min_R[0] = dia_0[0]*q_temp[0] + dia_p1[0]*q_temp[1];
		q_min_R[elements_n] = dia_0[elements_n]*q_temp[elements_n] + dia_m1[elements_n-1]*q_temp[elements_n-1];

		for(int i = 1; i < elements_n; i++)
			q_min_R[i] = dia_0[i]*q_temp[i] + dia_p1[i]*q_temp[i+1] + dia_m1[i-1]*q_temp[i-1];

		double norm1 = norm_q_dif(q_min_R, q_new);
		double norm2 = norm_q(q_new);
		double nev = norm1/norm2;

		if(nev < eps)
				end_cycle = true;
	
		//Переброс указателей
		double *ch_p;
		ch_p = q_new;
		q_new = q_temp;
		q_temp = ch_p;

		printf("Ne\tTime:\t%.15lf\tIter:\t%d\tNev:\t%.3e\tRelax:\t%.3e\r",t_new, iter, nev, w);

	}

	printf("\n");

	//Когда досчитали опять меняем, только уже переходим на новый слой
	double* ch_pt;
	ch_pt = q_new;
	q_new = q_last;
	q_last = ch_pt;

	t_now = t_new;
	h_t *= k_t;
}


void unlin_FEM::matrix_gen_si(double* q_n, double* q_t){

	double A_loc[2][2], b_loc[2]; //локальная матрица и вектор правой части

	//Генерация матрицы
	for(int i = 0; i < elements_n; i++){
		double x1 = nodes[elements[i].node1]; //левый узел для элемента
		double x2 = nodes[elements[i].node2]; //правый узел для элемента

		double h_k = x2 - x1; //шаг по сетки
		double x_midd = (x2+x1)/2; //центральная точка сетки

		double G_11_val, M_12_val; //значения элементов в матирце жескости и массы(чтобы кучу раз не считать одно и тоже)

		G_11_val = (lambda(x1, t_new) + lambda(x2, t_new))/(2*h_k);
		double gamma_aver = sigma((q_n[i+1] - q_n[i])/h_k, x_midd)/h_t;
		M_12_val = gamma_aver * h_k / 6;

		A_loc[0][0] = A_loc[1][1] = G_11_val + 2 * M_12_val;
		A_loc[1][0] = A_loc[0][1] = -G_11_val + M_12_val;

		double f1 = f(x1, t_new), f2 = f(x2, t_new); //значения правой части
		b_loc[0] = h_k*(2*f1 + f2)/6.0 + M_12_val * (2*q_last[i] + q_last[i+1]);
		b_loc[1] = h_k*(f1 + 2*f2)/6.0 + M_12_val * (q_last[i] + 2*q_last[i+1]);

		dia_0[i] += A_loc[0][0];
		dia_0[i+1] += A_loc[1][1];
		dia_p1[i] += A_loc[0][1];
		dia_m1[i] += A_loc[1][0];

		q_t[i] += b_loc[0];
		q_t[i+1] += b_loc[1];


	}

	//Учёт краевых условий
	//Левых
	switch(bound_type_left){
			case 1:{
				dia_0[0] = 1;
				dia_p1[0] = 0;
				q_t[0] = bounds_l[0](t_new);
				   }break;

			case 2:{
				q_t[0] += bounds_l[0](t_new);
				   }break;

			case 3:{
				dia_0[0] += bounds_l[0](t_now);
				q_t[0] += bounds_l[0](t_new) * bounds_l[1](t_new);
				   }break;
	};

	//Правых
	switch(bound_type_right){
			case 1:{
				dia_0[nodes_n-1] = 1;
				dia_m1[nodes_n-2] = 0;
				q_t[nodes_n-1] = bounds_r[0](t_new);
				   }break;

			case 2:{
				q_t[nodes_n-1] += bounds_r[0](t_new);
				   }break;

			case 3:{
				dia_0[nodes_n-1] += bounds_r[0](t_new);
				q_t[nodes_n-1] += bounds_r[0](t_new) * bounds_r[1](t_new);
				   }break;
	};

}

void unlin_FEM::matrix_gen_Newton(double* q_n, double* q_t){
	double A_loc[2][2], b_loc[2]; //локальная матрица и вектор правой части

	//Генерация матрицы
	for(int i = 0; i < elements_n; i++){
		double x1 = nodes[elements[i].node1]; //левый узел для элемента
		double x2 = nodes[elements[i].node2]; //правый узел для элемента

		double h_k = x2 - x1; //шаг по сетки
		double x_midd = (x2+x1)/2; //центральная точка сетки

		double G_11_val, M_12_val; //значения элементов в матирце жескости и массы(чтобы кучу раз не считать одно и тоже)

		G_11_val = (lambda(x1, t_new) + lambda(x2, t_new))/(2*h_k);
		double gamma_aver = sigma((q_n[i+1] - q_n[i])/h_k, x_midd)/h_t;
		M_12_val = gamma_aver * h_k / 6;

		//Среднее значние производной
		double sigm_ux_aver = sigma_ux((q_n[i+1] - q_n[i])/h_k, x_midd);


		//Базовые значения
		A_loc[0][0] = A_loc[1][1] = G_11_val + 2 * M_12_val;
		A_loc[1][0] = A_loc[0][1] = -G_11_val + M_12_val;
		double reg_par = 1.0;

		//Прибавки от метода Ньютона
		A_loc[0][0] += reg_par * sigm_ux_aver * (-q_n[i]/3.0 - q_n[i+1]/6.0 + (2*q_last[i] + q_last[i+1])/6.0) / h_t;
		A_loc[0][1] += reg_par * sigm_ux_aver * (q_n[i]/3.0 + q_n[i+1]/6.0 - (2*q_last[i] + q_last[i+1])/6.0) / h_t;
		A_loc[1][0] += reg_par * sigm_ux_aver * (-q_n[i]/6.0 - q_n[i+1]/3.0 + (q_last[i] + 2*q_last[i+1])/6.0) / h_t;
		A_loc[1][1] += reg_par * sigm_ux_aver * (q_n[i]/6.0 + q_n[i+1]/3.0 - (q_last[i] + 2*q_last[i+1])/6.0) / h_t;

		double f1 = f(x1, t_new), f2 = f(x2, t_new); //значения правой части

		//Базовые значения
		b_loc[0] = h_k*(2*f1 + f2)/6.0 + M_12_val * (2*q_last[i] + q_last[i+1]);
		b_loc[1] = h_k*(f1 + 2*f2)/6.0 + M_12_val * (q_last[i] + 2*q_last[i+1]);

		//Прибавки от метода Ньютона

		b_loc[0] += reg_par * sigm_ux_aver * (-q_n[i]*q_n[i]/3.0 + q_n[i]*q_n[i+1]/6.0 + q_n[i+1]*q_n[i+1]/6.0 + (2*q_last[i] + q_last[i+1])*(q_n[i]-q_n[i+1])/6.0) / h_t;
		b_loc[1] += reg_par * sigm_ux_aver * (-q_n[i]*q_n[i]/6.0 - q_n[i]*q_n[i+1]/6.0 + q_n[i+1]*q_n[i+1]/3.0 + (q_last[i] + 2*q_last[i+1])*(q_n[i]-q_n[i+1])/6.0) / h_t;

		dia_0[i] += A_loc[0][0];
		dia_0[i+1] += A_loc[1][1];
		dia_p1[i] += A_loc[0][1];
		dia_m1[i] += A_loc[1][0];

		q_t[i] += b_loc[0];
		q_t[i+1] += b_loc[1];


	}

	//Учёт краевых условий
	//Левых
	switch(bound_type_left){
			case 1:{
				dia_0[0] = 1;
				dia_p1[0] = 0;
				q_t[0] = bounds_l[0](t_new);
				   }break;

			case 2:{
				q_t[0] += bounds_l[0](t_new);
				   }break;

			case 3:{
				dia_0[0] += bounds_l[0](t_now);
				q_t[0] += bounds_l[0](t_new) * bounds_l[1](t_new);
				   }break;
	};

	//Правых
	switch(bound_type_right){
			case 1:{
				dia_0[nodes_n-1] = 1;
				dia_m1[nodes_n-2] = 0;
				q_t[nodes_n-1] = bounds_r[0](t_new);
				   }break;

			case 2:{
				q_t[nodes_n-1] += bounds_r[0](t_new);
				   }break;

			case 3:{
				dia_0[nodes_n-1] += bounds_r[0](t_new);
				q_t[nodes_n-1] += bounds_r[0](t_new) * bounds_r[1](t_new);
				   }break;
	};


}


double unlin_FEM::norm_q_dif(double *q1, double *q2){
	double norm = 0;
	for(int i = 0; i < nodes_n; i++)
		norm += (q1[i] - q2[i])*(q1[i] - q2[i]);

	norm = sqrt(norm);
	return norm;
}

double unlin_FEM::norm_q(double *q1){
	double norm = 0;
	for(int i = 0; i < nodes_n; i++)
		norm += q1[i]*q1[i];

	norm = sqrt(norm);
	return norm;
}

void unlin_FEM::out_q(string file_out){
	FILE* out_f = fopen(file_out.c_str(), "a");

	//fprintf(out_f, "============================= Time: %.15lf ===================\n", t_now);

	double diff = 0;
	double u_norm = 0;

	for(int i = 0; i < nodes_n; i++){
		double u_val = analytic(nodes[i], t_now);
		//fprintf(out_f,"%d\t%.15lf\t%.15lf\n", i, q_last[i], u_val);
		diff += (q_last[i]-u_val)*(q_last[i]-u_val);
		u_norm += u_val*u_val;
	}

	diff = sqrt(diff/u_norm);

	fprintf(out_f, "Diff:\t%.3e\n", diff);

	fclose(out_f);
}



double unlin_FEM::w_min_func(double w){
		//Обнуление матрицы и вычисление аргумента
		for(int i = 0; i < elements_n; i++){
			dia_0[i] = 0;
			dia_p1[i] = 0;
			dia_m1[i] = 0;
			q_min[i] = w*q_temp[i] + (1-w)*q_new[i];
			q_min_rp[i] = 0;
		}
		dia_0[nodes_n-1] = 0;
		q_min[nodes_n-1] = w*q_temp[nodes_n-1] + (1-w)*q_new[nodes_n-1];
		q_min_rp[nodes_n-1] = 0;

		matrix_gen_si(q_min, q_min_rp);
		
		//Вычисление невязки: A(q_min)*q_min - b(q_min)
		q_min_R[0] = dia_0[0]*q_min[0] + dia_p1[0]*q_min[1] - q_min_rp[0];
		q_min_R[elements_n] = dia_0[elements_n]*q_min[elements_n] + dia_m1[elements_n-1]*q_min[elements_n-1] - q_min_rp[elements_n];

		for(int i = 1; i < elements_n; i++)
			q_min_R[i] = dia_0[i]*q_min[i] + dia_p1[i]*q_min[i+1]+dia_m1[i-1]*q_min[i-1] - q_min_rp[i];

		return norm_q(q_min_R);
}

double unlin_FEM::w_min(){
	double a, b; //Отрезок минимизации
	w_min_find_area(a, b, 1E-3); // Ищим отрезок
	
	return w_min_golden(a, b, 1E-5);
}

void unlin_FEM::w_min_find_area(double &a, double &b, double delta){

	//Инициализация
	double x0 = 1;
	double f0 = w_min_func(x0);
	double x1 = x0-delta;
	double f1 = w_min_func(x1);
	double h = -delta;
	int k = 1;

	//Определяем сторону, куда идти
	if(f0 < f1){
		x1 = x0 + delta;
		h *= -1;
	} 

	bool end_cycle = false;

	while(!end_cycle){
		h *= 2;
		x0 = x1 + h;
		f0 = w_min_func(x0);
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

	if(x0 > x1){
		a = x1;
		b = x0;
	}
	else{
		a = x0;
		b = x1;
	}
}

double unlin_FEM::w_min_golden(double a, double b, double eps_min){
	double x1, x2, f1, f2;
	const double mul = (3-sqrt((double)5.0))/2;

	int point_num;
	x1 = a + mul*(b-a);
	x2 = b - mul*(b-a);
	f1 = w_min_func(x1);
	f2 = w_min_func(x2);


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

	while(fabs(a-b)>eps_min){
		switch(point_num){
				case 1:{
					x1 = a + mul*(b-a);
					f1 = w_min_func(x1);
					   }break;
				case 2:{
					x2 = b - mul*(b-a);
					f2 = w_min_func(x2);
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
	return (a+b)/2;
}

