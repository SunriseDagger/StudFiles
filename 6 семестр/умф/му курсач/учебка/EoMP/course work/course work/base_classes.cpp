#include "base_classes.h"

Grid::Grid(){
	nodes_number = elements_number = faces_number_sec = faces_number_thi = first_number = 0;
}

void Grid::input_nodes(FILE *nodes_f){
	fscanf(nodes_f,"%d",&nodes_number);
	nodes = new node [nodes_number];
	for(int i = 0; i < nodes_number; i++){
		fscanf(nodes_f,"%lf %lf %lf", &nodes[i].x, &nodes[i].y, &nodes[i].z);
		nodes[i].number = i;
	}

	weight_0 = new double [nodes_number];
	weight_1 = new double [nodes_number];
	weight_2 = new double [nodes_number];

}

void Grid::input_elements(FILE *el_f){
	fscanf(el_f,"%d",&elements_number);
	elements = new FE [elements_number];
	for(int i = 0; i < elements_number; i++){
		fscanf(el_f, "%d %d %d %d", &elements[i].node_n[0], &elements[i].node_n[1], &elements[i].node_n[2], &elements[i].node_n[3]);
		elements[i].number = i;
	}
}

void Grid::input_faces_s(FILE* face_f_sec){

	fscanf(face_f_sec, "%d", &faces_number_sec);
	faces_s = new face [faces_number_sec];
	for(int i = 0; i < faces_number_sec; i++){
		fscanf(face_f_sec, "%d %d %d",&faces_s[i].node_n[0],&faces_s[i].node_n[1], &faces_s[i].node_n[2]);
		faces_s[i].number = i;
	}
}

void Grid::input_faces_t(FILE* face_f_thi){

	fscanf(face_f_thi, "%d", &faces_number_thi);
	faces_t = new face [faces_number_thi];
	for(int i = 0; i < faces_number_thi; i++){
		fscanf(face_f_thi, "%d %d %d",&faces_t[i].node_n[0],&faces_t[i].node_n[1], &faces_t[i].node_n[2]);
		faces_t[i].number = i;
	}
}

void Grid::input_fir_nodes(FILE *fir_f){

	int node_n;

	fscanf(fir_f, "%d", &first_number);

	fir_b = new first_boundary [first_number];

	for(int i = 0; i < first_number; i++)
		fscanf(fir_f, "%d %d", &fir_b[i].node_numb, &fir_b[i].area);
}

void Grid::input_el_areas(FILE *el_a_f){

	int el_n, ar_n;
	for(int i = 0; i < elements_number; i++){
		fscanf(el_a_f, "%d %d",&el_n, &ar_n);
		elements[el_n].area = ar_n;
	}
}

void Grid::input_f_s_areas(FILE* fs_a_f){

	int f_n, ar_n;
	for(int i = 0; i < faces_number_sec; i++){
		fscanf(fs_a_f, "%d %d",&f_n, &ar_n);
		faces_s[f_n].area = ar_n;
	}
}

void Grid::input_f_t_areas(FILE* ft_a_f){

	int f_n, ar_n;
	for(int i = 0; i < faces_number_sec; i++){
		fscanf(ft_a_f, "%d %d",&f_n, &ar_n);
		faces_t[f_n].area = ar_n;
	}
}

void Grid::input_start(FILE *st0, FILE *st1, FILE *st2){

	for(int i = 0; i < nodes_number; i++)
		fscanf(st0, "%lf", &weight_0[i]);

	for(int i = 0; i < nodes_number; i++)
		fscanf(st1, "%lf", &weight_1[i]);

	for(int i = 0; i < nodes_number; i++)
		fscanf(st2, "%lf", &weight_2[i]);
}

void Grid::input_time_data(FILE *time_d){
	double t_start;
	double tau0;

	fscanf(time_d, "%lf %lf %lf", &t_start, &tau0, &k_time);

	t2 = t_start;
	t1 = t2 + tau0;
	tau0 *= k_time;
	t0 = t1 + tau0;
}


void Grid::gen_SLAE_port(){

	init_area();

	SLAE_gen.init(nodes_number);
	gi = new int [nodes_number+1];

	for(int i = 0; i < elements_number; i++)
		SLAE_gen.add_el(elements[i]);

	SLAE_gen.gen(gi, gj, SLAE_el_numbers);
	gg = new double [SLAE_el_numbers];
	di = new double [nodes_number];
	right_part = new double [nodes_number];



	SLAE_gen.~SLAE_port_gen();

}

void Grid::gen_SLAE(){

	t_new = t0 + k_time*(t0 - t1); //устанавливаем новое время

	for(int i = 0; i < SLAE_el_numbers; i++)
		gg[i] = 0;

	for(int i = 0; i < nodes_number; i++)
		right_part[i] = di[i] = 0;


	double **A_loc, *b_loc;

	int pos, cur_row;

	A_loc = new double* [4];
	D = new double* [4];
	alpha = new double* [4];
	b_loc = new double [4];
	prepared(4);

	for(int i = 0; i < 4; i++){
		A_loc[i] = new double [4];
		D[i] = new double [4];
		alpha[i] = new double [4];
	}

	// Сборка основной матрицы


	for(int k = 0; k < elements_number; k++){
		gen_local_el(A_loc, b_loc, k);
		int aaa_1 = 0;

		for(int i = 0; i < 4; i++){
			cur_row = elements[k].node_n[i];
			for(int j = 0 ; j < i ; j++){
				//if(cur_row > elements[k].node_n[j]){
					pos = find_el_pos(cur_row, elements[k].node_n[j]);
					gg[pos] += A_loc[i][j];
					aaa_1++;
				//}
			}

			di[cur_row] += A_loc[i][i];
			right_part[cur_row] += b_loc[i];
		}

	}




	// Учёт третьих краевых условий

	for(int k = 0; k < faces_number_thi; k++){
		gen_local_thi(A_loc, b_loc, k);

		for(int i = 0; i < 3; i++){
			cur_row = faces_t[k].node_n[i];
			for(int j = 0 ; j < 3 ; j++){
				if(cur_row > faces_t[k].node_n[j]){
					pos = find_el_pos(cur_row, faces_t[k].node_n[j]);
					gg[pos] += A_loc[i][j];
				}
			}

			di[cur_row] += A_loc[i][i];
			right_part[cur_row] += b_loc[i];
			}
		}

	//Учёт вторых краевых условий

	for(int k = 0; k < faces_number_sec; k++){
		gen_local_sec(b_loc, k);

		for(int i = 0; i < 3; i++)
			right_part[faces_s[k].node_n[i]] += b_loc[i];
	}

	//============== Спец. тест =============
	//Учёт точненого источника
	sourse = 66328;
	if(t_new < 1E-8)
		right_part[sourse] += 2.0;

	//Учёт первых краевых условий

	for(int k = 0; k < first_number; k++){
		cur_row = fir_b[k].node_numb;

		double val = get_func_value_fir(k);

		di[cur_row] = 1;
		right_part[cur_row] = val;


		int i_s = gi[cur_row], i_e = gi[cur_row+1];
		for(int i = i_s; i < i_e; i++){
			right_part[gj[i]] -= gg[i]*val;
			gg[i] = 0;
		}
		for(int p = cur_row + 1; p < nodes_number; p++){
			int i_s = gi[p], i_e = gi[p+1];
			for(int i = i_s; i < i_e; i++){
				if(gj[i] == cur_row){
					right_part[p] -= gg[i]*val;
					gg[i] = 0;
				}
			}
		}
	}
	
}


void Grid::solve_SLAE(){
	CGM solver; //решатель СЛАУ
	
	//Перебрасываем значения с предыдущего слоя на новый
	double *c = weight_0;
	weight_0 = weight_2;
	weight_2 = weight_1;
	weight_1 = c;
	
	t2 = t1;
	t1 = t0;
	t0 = t_new;

	string f_str;
	stringstream a;
	a << t0;
	string tmp_str;
	a >> tmp_str;

	/*f_str = path + "rp " + tmp_str + ".txt";
	FILE *f_out = fopen(f_str.c_str(), "w");
	for(int i = 0; i < nodes_number; i++){
		fprintf(f_out, "%d\t%.15lf\n", i, right_part[i]);
	}
	fclose(f_out);*/
	


	//Иниицализируем СЛАУ и решаем
	solver.init(gi, gj, di, gg, right_part, nodes_number);
	solver.solve(weight_0);
}


void Grid::gen_local_el(double **A_loc, double *b_loc, int el_n){

	//Формирование L-координат
	double mes;

	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			if(i==j)
				alpha[i][j] = 1;
			else
				alpha[i][j] = 0;
			A_loc[i][j] = 0;
		}
		b_loc[i] = 0;
	}

	//Матрица D

	for(int i = 0; i < 4; i++)
		D[0][i] = 1;

	for(int i = 0; i < 4; i++)
		D[1][i] = nodes[elements[el_n].node_n[i]].x;

	for(int i = 0; i < 4; i++)
		D[2][i] = nodes[elements[el_n].node_n[i]].y;

	for(int i = 0; i < 4; i++)
		D[3][i] = nodes[elements[el_n].node_n[i]].z;


	mes = (D[1][1]-D[1][0])*((D[2][2]-D[2][0])*(D[3][3]-D[3][0]) - (D[2][3]-D[2][0])*(D[3][2]-D[3][0]));
	mes += (D[2][1]-D[2][0])*((D[3][2]-D[3][0])*(D[1][3]-D[1][0]) - (D[3][3]-D[3][0])*(D[1][2]-D[1][0]));
	mes += (D[3][1]-D[3][0])*((D[1][2]-D[1][0])*(D[2][3]-D[2][0]) - (D[1][3]-D[1][0])*(D[2][2]-D[2][0]));

	mes = fabs(mes)/6.0;

	//Сами координаты

	for(int i = 0; i < 4; i++)
		solve_Gauss(D, alpha[i], 4);

	for(int i = 0 ; i < 4; i++)
		for(int j = 0; j < 4; j++)
			D[i][j] = alpha[j][i];

	for(int i = 0 ; i < 4; i++)
		for(int j = 0; j < 4; j++)
			alpha[i][j] = D[i][j];

	//Формирование локальной матрицы

	//Матрица жескости
	double G[4][4];

	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			double sum = 0;
			for(int k = 1; k < 4; k++)
				sum += alpha[i][k]*alpha[j][k];

			G[i][j] = sum*mes;
		}
	}


	//Матрица массы
	double M[4][4];
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			if(i==j) M[i][j] = 2;
			else M[i][j] = 1;
			M[i][j] *= mes/20.0;
		}
	}

	//Локальные значения коэффициентов
	double loc_lambda = get_lambda_value(el_n);
	double loc_gamma = get_gamma_value(el_n);
	double loc_sigma = get_sigma_value(el_n, t_new);
	double loc_epsilon = get_epsilon_value(el_n, t_new);

	double tau[3] = {t_new - t0, t0 - t1, t1 - t2}; //шаги по времени

	double tetta[4]; //коэффициенты при аппроксимации первой производной
	double miu[4]; //коэффициенты при аппроксимации второй поизводной
	
	double coef_denoms[4]; //знаменатели коэффциентов
	coef_denoms[3] = (tau[0] + tau[1] + tau[2]) * (tau[1] + tau[2]) * tau[2];
	coef_denoms[2] = tau[2] * tau[1] * (tau[0] + tau[1]);
	coef_denoms[1] = tau[0] * tau[1] * (tau[1] + tau[2]);
	coef_denoms[0] = tau[0] * (tau[0] + tau[1]) * (tau[0] + tau[1] + tau[2]);

	tetta[3] = -tau[0]*(tau[0] + tau[1])/coef_denoms[3];
	tetta[2] = tau[0]*(tau[0] + tau[1] + tau[2]) / coef_denoms[2];
	tetta[1] = -(tau[0] + tau[1])*(tau[0] + tau[1] + tau[2]) / coef_denoms[1];
	tetta[0] = 1.0/tau[0] + 1.0/(tau[0] + tau[1]) + 1.0/(tau[0] + tau[1] + tau[2]);

	miu[3] = -2.0*(2.0*tau[0] + tau[1]) / coef_denoms[3];
	miu[2] = 2.0*(2.0*tau[0] + tau[1] + tau[2]) / coef_denoms[2];
	miu[1] = -2.0*(2.0*tau[0] + 2.0*tau[1] + tau[2]) / coef_denoms[1];
	miu[0] = 2.0*(3.0*tau[0] + 2.0*tau[1] + tau[2]) / coef_denoms[0];


	//Сама локальная матрица
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			A_loc[i][j] += loc_lambda*G[i][j] + (loc_gamma + loc_sigma*tetta[0] + loc_epsilon*miu[0])*M[i][j];
		}
	}

	//Формирование локального вектора
	double f_loc = 0;
	//Часть от исходного уравнения
	for(int i = 0; i < 4 ;i++)
		f_loc += alpha[i][1]*get_rp_value(elements[el_n].node_n[i]);


	//Часть от дискретизации по времени
	double loc_q0[4], loc_q1[4], loc_q2[4];

	for(int i = 0; i < 4; i++){
		loc_q0[i] = loc_q1[i] = loc_q2[i] = 0;
		for(int j = 0; j < 4; j++){
			loc_q0[i] += M[i][j]*weight_0[elements[el_n].node_n[j]];
			loc_q1[i] += M[i][j]*weight_1[elements[el_n].node_n[j]];
			loc_q2[i] += M[i][j]*weight_2[elements[el_n].node_n[j]];

		}
		loc_q0[i] *= (loc_sigma*tetta[1] + loc_epsilon*miu[1]);
		loc_q1[i] *= (loc_sigma*tetta[2] + loc_epsilon*miu[2]);
		loc_q2[i] *= (loc_sigma*tetta[3] + loc_epsilon*miu[3]);
	}

	//Суммируем их
	for(int i = 0; i < 4; i++)
		b_loc[i] += f_loc*mes/4.0 - loc_q0[i] - loc_q1[i] - loc_q2[i];

}


void Grid::gen_local_thi(double **A_loc, double *b_loc, int f_n){

	double mes = get_face_mes(3, f_n);

	double betta = get_betta_value(f_n);

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			A_loc[i][j] = betta*mes/12;
		}
		A_loc[i][i] += betta*mes/12;
	}

	for(int i = 0; i < 3; i++){
		b_loc[i] = 0;
		for(int j = 0; j < 3; j++)
			b_loc[i] += A_loc[i][j]*get_u_betta_value(f_n, faces_t[f_n].node_n[j]);
	}
	
}


void Grid::gen_local_sec(double *b_loc, int f_n){

	double mes = get_face_mes(2, f_n);

	double tetta[3];
	double sum_tetta = 0;

	for(int i = 0; i < 3; i++){
		tetta[i] = get_tetta_value(f_n, faces_s[f_n].node_n[i]);
		sum_tetta += tetta[i];
	}
	for(int i = 0; i < 3; i++){
		b_loc[i] = mes*(tetta[i] + sum_tetta)/12;
	}
}


double Grid::get_face_mes(int boun, int f_n){
	double mes;
	double p[3];
	node loc_node[3];

	if(boun == 3)
		for(int i = 0; i < 3; i++)
			loc_node[i] = nodes[faces_t[f_n].node_n[i]];
	else
		for(int i = 0; i < 3; i++)
			loc_node[i] = nodes[faces_s[f_n].node_n[i]];

	p[0] = (loc_node[1].y - loc_node[0].y) * (loc_node[2].z - loc_node[0].z) - (loc_node[2].y - loc_node[0].y) * (loc_node[1].z - loc_node[0].z);
	p[1] = (loc_node[2].x - loc_node[0].x) * (loc_node[1].z - loc_node[0].z) - (loc_node[1].x - loc_node[0].x) * (loc_node[2].z - loc_node[0].z);
	p[2] = (loc_node[1].x - loc_node[0].x) * (loc_node[2].y - loc_node[0].y) - (loc_node[2].x - loc_node[0].x) * (loc_node[1].y - loc_node[0].y);

	mes = 0;
	for(int i = 0; i < 3; i++)
		mes += p[i]*p[i];
	mes = 0.5*sqrt(mes);

	return mes;
}


int Grid::find_el_pos(int i, int j){
	int k_s = gi[i], k_e = gi[i+1];
	int cur;
	bool find = false;
	for(int k = k_s; k < k_e && !find; k++){
		if(gj[k] == j){
			cur = k;
			find = true;
		}
	}
	return cur;
}

double Grid::get_lambda_value(int el_n){
	return lambda_values[elements[el_n].area];
}

double Grid::get_gamma_value(int el_n){
	double gamma = 0;
	int area = elements[el_n].area;

	node loc_nodes[4];

	for(int i = 0; i < 4; i++)
			loc_nodes[i] = nodes[elements[el_n].node_n[i]];

	for(int i = 0; i < 4; i++)
		gamma += gamma_values[area](loc_nodes[i].x, loc_nodes[i].y, loc_nodes[i].z, t_new);
	gamma /= 4;
	return gamma;
}

double Grid::get_sigma_value(int el_n, double t){
	double sigma = 0;
	int area = elements[el_n].area;

	node loc_nodes[4];

	for(int i = 0; i < 4; i++)
		loc_nodes[i] = nodes[elements[el_n].node_n[i]];

	for(int i = 0; i < 4; i++)
		sigma += sigma_values[area](loc_nodes[i].x, loc_nodes[i].y, loc_nodes[i].z, t_new);
	sigma /= 4;
	return sigma;
}

double Grid::get_epsilon_value(int el_n, double t){
	double epsilon = 0;
	int area = elements[el_n].area;

	node loc_nodes[4];

	for(int i = 0; i < 4; i++)
		loc_nodes[i] = nodes[elements[el_n].node_n[i]];

	for(int i = 0; i < 4; i++)
		epsilon += epsilon_values[area](loc_nodes[i].x, loc_nodes[i].y, loc_nodes[i].z, t_new);
	epsilon /= 4;
	return epsilon;
}


double Grid::get_betta_value(int f_n){
	return betta_values[faces_t[f_n].area];
}

double Grid::get_u_betta_value(int f_n, int node_n){
	return u_betta_values[faces_t[f_n].area](nodes[node_n].x, nodes[node_n].y, nodes[node_n].z, t_new);
}

double Grid::get_tetta_value(int f_n, int node_n){
	return tetta_values[faces_s[f_n].area](nodes[node_n].x, nodes[node_n].y, nodes[node_n].z, t_new);
}

double Grid::get_rp_value(int node_n){
	return equation_rp(nodes[node_n].x, nodes[node_n].y, nodes[node_n].z, t_new);
}

double Grid::get_func_value_fir(int node_n){
	int gl_node = fir_b[node_n].node_numb;
	return fir_values[fir_b[node_n].area](nodes[gl_node].x, nodes[gl_node].y, nodes[gl_node].z, t_new);
}

void Grid::output_weight(FILE *outp_f){
	//fprintf(outp_f, "Time =\t%.10lf\n", t0);
	//for(int i = 86; i < 89; i++)
	//	fprintf(outp_f,"%d\t%.15lf\n",i , weight_0[i]);
	fprintf(outp_f,"%.15lf\t\t%.15lf\t\t%.15lf\n", weight_0[sourse], weight_0[sourse+1],weight_0[sourse+2]);
}

void Grid::output_diff(FILE* outp_f){
	double diff = 0;
	double denom = 0;

	for(int i = 0; i < nodes_number; i++){
		double u_val = sol_func(nodes[i].x, nodes[i].y, nodes[i].z, t0);
		diff += (weight_0[i] - u_val) * (weight_0[i] - u_val);
		denom += u_val * u_val;
	}

	diff /= denom;
	fprintf(outp_f, "%.5lf\t%.3e\n", t0, diff);
}

double Grid::u_in_el(double x, double y, double z){
	for(int k = 0; k < elements_number; k++){

		for(int i = 0; i < 4; i++)
			D[0][i] = 1;

		for(int i = 0; i < 4; i++)
			D[1][i] = nodes[elements[k].node_n[i]].x;

		for(int i = 0; i < 4; i++)
			D[2][i] = nodes[elements[k].node_n[i]].y;

		for(int i = 0; i < 4; i++)
			D[3][i] = nodes[elements[k].node_n[i]].z;

		double V[4] = {0, 0, 0, 0}, mes = 0;
		V[0] = (D[1][1]-x)*((D[2][2]-y)*(D[3][3]-z) - (D[2][3]-y)*(D[3][2]-z));
		V[0] += (D[2][1]-y)*((D[3][2]-z)*(D[1][3]-x) - (D[3][3]-z)*(D[1][2]-x));
		V[0] += (D[3][1]-z)*((D[1][2]-x)*(D[2][3]-y) - (D[1][3]-x)*(D[2][2]-y));

		V[1] = (x-D[1][0])*((D[2][2]-D[2][0])*(D[3][3]-D[3][0]) - (D[2][3]-D[2][0])*(D[3][2]-D[3][0]));
		V[1] += (y-D[2][0])*((D[3][2]-D[3][0])*(D[1][3]-D[1][0]) - (D[3][3]-D[3][0])*(D[1][2]-D[1][0]));
		V[1] += (z-D[3][0])*((D[1][2]-D[1][0])*(D[2][3]-D[2][0]) - (D[1][3]-D[1][0])*(D[2][2]-D[2][0]));

		V[2] = (D[1][1]-D[1][0])*((y-D[2][0])*(D[3][3]-D[3][0]) - (D[2][3]-D[2][0])*(z-D[3][0]));
		V[2] += (D[2][1]-D[2][0])*((z-D[3][0])*(D[1][3]-D[1][0]) - (D[3][3]-D[3][0])*(x-D[1][0]));
		V[2] += (D[3][1]-D[3][0])*((x-D[1][0])*(D[2][3]-D[2][0]) - (D[1][3]-D[1][0])*(y-D[2][0]));

		V[3] = (D[1][1]-D[1][0])*((D[2][2]-D[2][0])*(z-D[3][0]) - (y-D[2][0])*(D[3][2]-D[3][0]));
		V[3] += (D[2][1]-D[2][0])*((D[3][2]-D[3][0])*(x-D[1][0]) - (z-D[3][0])*(D[1][2]-D[1][0]));
		V[3] += (D[3][1]-D[3][0])*((D[1][2]-D[1][0])*(y-D[2][0]) - (x-D[1][0])*(D[2][2]-D[2][0]));

		mes = (D[1][1]-D[1][0])*((D[2][2]-D[2][0])*(D[3][3]-D[3][0]) - (D[2][3]-D[2][0])*(D[3][2]-D[3][0]));
		mes += (D[2][1]-D[2][0])*((D[3][2]-D[3][0])*(D[1][3]-D[1][0]) - (D[3][3]-D[3][0])*(D[1][2]-D[1][0]));
		mes += (D[3][1]-D[3][0])*((D[1][2]-D[1][0])*(D[2][3]-D[2][0]) - (D[1][3]-D[1][0])*(D[2][2]-D[2][0]));

		if(V[0] + V[1] + V[2] + V[3] <= mes){
			double val = 0;
			for(int i = 0; i < 4; i++){
				for(int j = 0; j < 4; j++){
					if(i==j)
						alpha[i][j] = 1;
					else
						alpha[i][j] = 0;
				}
			}

			for(int i = 0; i < 4; i++)
				solve_Gauss(D, alpha[i], 4);

			for(int i = 0 ; i < 4; i++)
				for(int j = 0; j < 4; j++)
					D[i][j] = alpha[j][i];

			for(int i = 0 ; i < 4; i++)
				for(int j = 0; j < 4; j++)
					alpha[i][j] = D[i][j];

			for(int i = 0; i < 4; i++)
				val += weight_0[elements[k].node_n[i]]*(alpha[i][0] + x*alpha[i][1] + y*alpha[i][2] + z*alpha[i][3]);
			return val;
		}
	}
	return 0;
}
