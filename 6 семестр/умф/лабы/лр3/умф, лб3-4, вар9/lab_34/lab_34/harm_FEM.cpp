#include "harm_FEM.h"

template <typename solver_type> void harm_FEM<solver_type>::init(std::string file_cords, std::string file_elements, std::string file_faces){
	FILE* inp_cord = fopen(file_cords.c_str(), "r");

	//Ввод координат вершин
	fscanf(inp_cord, "%d", &nodes_n);
	nodes = new node [nodes_n];

	for(int i = 0; i < nodes_n; i++)
		fscanf(inp_cord, "%lf %lf %lf", &nodes[i].x, &nodes[i].y, &nodes[i].z);
	fclose(inp_cord);

	FILE* inp_els = fopen(file_elements.c_str(), "r");
	
	//Ввод элементов
	fscanf(inp_els, "%d", &elements_n);
	elements = new FE [elements_n];

	for(int i = 0; i < elemtnts_n; i++){
		for(int j = 0; j < 16; j++){
			fscanf(inp_els, "%d", elemtnts[i].node_n[j]);
		}
	}

	fclose(inp_els);
	
	FILE* inp_faces = fopen(file_faces.c_srt(), "r");

	//Ввод первых краевых
	fscanf(inp_faces, "%d", &faces_fir_n);

	faces_fir = new double [faces_fir_n];
	faces_fir_node = new int [faces_fir_n];

	for(int i = 0; i < faces_n_fir, i++){
			fscanf(inp_faces, "%d %lf", &faces_fir_node[i], &faces_fir[i]);
	}

	//Ввод вторых краевых
	fscanf(inp_faces, "%d", &faces_sec_n);

	faces_sec = new face [faces_sec_n];

	for(int i = 0; i < faces_sec_n, i++){
		for(int j = 0; j < 8; j++){
			fscanf(inp_faces, "%d", &faces_sec[i].node_n[j]);
		}
	}

	//Ввод третьих краевых
	fscanf(inp_faces, "%d", &faces_thi_n);

	faces_thi = new face [faces_thi_n];

	for(int i = 0; i < faces_thi_n, i++){
		for(int j = 0; j < 8; j++){
			fscanf(inp_faces, "%d", &faces_thi[i].node_n[j]);
		}
	}

	fclose(inp_faces);

}
template <typename solver_type> void harm_FEM<solver_type>::set_coefs(func set_lambda, func set_sigma, func set_epsilon){
	lambda = set_lambda;
	sigma = set_sigma;
	epsilon = set_epsilon;
}
template <typename solver_type> void harm_FEM<solver_type>::set_rp(func set_f_sin, func set_f_cos){
	f_sin = set_f_sin;
	f_cos = set_f_cos;
}
template <typename solver_type> void harm_FEM<solver_type>::set_w(double s_w){
	w = s_w;
}

template <typename solver_type> void harm_FEM<solver_type>::form_gmass(){
	port_gen.init(nodes_n);

	//Вс собираем
	ig = new int [nodes_n+1];

	for(int i = 0; i < elements_n; i++)
		port_gen.add_el(elemtnts[i]);

	port_gen.gen(ig, jg, SLAE_el_n); //получаем портрет

	gl = new double [SLAE_el_n];
	gu = new double [SLAE_el_n];
	di = new double [nodes_n];

	right_part = new double [n];
	solution = new double [n];

	//Обнуление
	for(int i = 0; i < SLAE_el_n; i++)
		gl[i] = gu[i] = 0;

	for(int i = 0; i < nodes_n; i++)
		di[i] = right_part[i] = solution[i] = 0;

	port_gen.~SLAE_port_gen();
}

template <typename solver_type> void harm_FEM<typename solver_type>::form_matrix(){
	double **A_loc, *b_loc; //локальные матрица и вектор правой части
	b_loc = new double [16];
	A_loc = new double* [16];

	for(int i = 0; i < 16; i++)
		A_loc[i] = new double [16];


	int cur_row; //текущая строка
	int pos; //Позиция в gu и gl

	//Генерация основной СЛАУ

	for(int k = 0; k < elements_n; k++){
		form_loc(A_loc, b_loc, k); //Получаем локальные матрицы

		for(int i = 0; i < 16; i++){
			cur_row = elements[k].node_n[i]; //Определяем элемент строки
			for(int j = 0 ; j < i ; j++){
				if(cur_row > elements[k].node_n[j]){ //Если элементы содержаться в строке
					pos = find_el_pos(cur_row, elements[k].node_n[j]); //Находим позицию в gu и gl
					gl[pos] += A[i][j];
					gu[pos] += A[j][i];
				}
				else{
					pos = find_el_pos(elements[k].node_n[j], cur_row); //Находим позицию в gu и gl
					gu[pos] += A[i][j];
					gl[pos] += A[j][i];
				}
			}

			di[cur_row] += A_loc[i][i];
			right_part[cur_row] += b_loc[i];
		}
	}

		// Учёт третьих краевых условий

	for(int k = 0; k < faces_thi_n; k++){
		gen_local_thi(A_loc, b_loc, k);

		for(int i = 0; i < 8; i++){
			cur_row = faces_t[k].node_n[i];
			for(int j = 0 ; j < i ; j++){
				if(cur_row > faces_thi[k].node_n[j]){ //Если элементы содержаться в строке
					pos = find_el_pos(cur_row, faces_thi[k].node_n[j]); //Находим позицию в gu и gl
					gl[pos] += A[i][j];
					gu[pos] += A[j][i];
				}
				else{
					pos = find_el_pos(faces_thi[k].node_n[j], cur_row); //Находим позицию в gu и gl
					gu[pos] += A[i][j];
					gl[pos] += A[j][i];
				}
			}

			di[cur_row] += A_loc[i][i];
			right_part[cur_row] += b_loc[i];
			}
		}

	//Учёт вторых краевых условий

	for(int k = 0; k < faces_sec_n; k++){
		gen_local_sec(b_loc, k);

		for(int i = 0; i < 3; i++)
			right_part[faces_s[k].node_n[i]] += b_loc[i];
	}

	//Учёт первых краевых условий

	for(int k = 0; k < first_number; k++){
		cur_row = faces_fir_node[k]; //Узел, в котором заданно краевое

		double val = faces_fir[k]; //Получаем значение

		di[cur_row] = 1;
		right_part[cur_row] = val;

		//Обнуляем верхную часть столбца
		int i_s = ig[cur_row], i_e = ig[cur_row+1];
		for(int i = i_s; i < i_e; i++){
			right_part[gj[i]] -= gl[i]*val;
			gl[i] = 0;
			gu[i] = 0;
		}
		//обнуляем нижную часть столбца
		for(int p = cur_row + 1; p < nodes_number; p++){
			int i_s = ig[p], i_e = ig[p+1];
			for(int i = i_s; i < i_e; i++){
				if(jg[i] == cur_row){ //Проверка - тот ли столбец
					right_part[p] -= gu[i]*val;
					gl[i] = 0;
					gu[i] = 0;
				}
			}
		}
	}
}

template <typename solver_type> void harm_FEM<solver_type>::form_loc(double **A_loc, double *b_loc, int el_n){
	 //Матрицы жескости и массы для обычного МКЭ
	 double G[8][8], M[8][8];
	 double hx = fabs(nodes[elements[el_n].node_n[2]].x - nodes[elements[el_n].node_n[0]].x); //Шаг по x
	 double hy = fabs(nodes[elements[el_n].node_n[4]].y - nodes[elements[el_n].node_n[0]].y); //Шаг по y
	 double hz = fabs(nodes[elements[el_n].node_n[8]].z - nodes[elements[el_n].node_n[0]].z); //Шаг по z

	 double G1[2][2] = {{1.0, -1.0}, {-1.0, 1.0}}; // Дополнительная матрица
	 double M1[2][2] = {{1.0/3.0, 1.0/6.0}, {1.0/6.0, 1.0/3.0}}; // Ещё одна дополнительная матрица

	 double labmda_aver = 0; //Усреднённое лямбда
	 double sigma_aver = 0; //Усреднённое сигма
	 double epsilon_aver = 0; //Усреднённое эпсилон

	 
	 //Получаем средние значения
	 for(int i = 0; i < 8; i++){
		 double x = nodes[elements[el_n].node_n[2*i]].x; //координаты точки
		 double y = nodes[elements[el_n].node_n[2*i]].y; //координаты точки
		 double z = nodes[elements[el_n].node_n[2*i]].z; //координаты точки
		 lambda_aver += lambda(x,y,z);
		 sigma_aver += sigma(x,y,z);
		 epsilon_aver += epsilon(x, y, z);
	 }
	 lambda /= 8.0;
	 sigma /= 8.0;
	 epsilon /= 8.0;

	 //Получение матриц G и M
	 for(int i = 0; i < 8; i++){
		 for(int j = 0; j < 8; j++){
			 G[i][j] = hy*hz * G1[mu(i)][mu(j)]*M1[nu(i)][nu(j)]*M1[v(i)][v(j)] / hx;
			 G[i][j] += hx*hz * M1[mu(i)][mu(j)]*G1[nu(i)][nu(j)]*M1[v(i)][v(j)] / hy;
			 G[i][j] += hx*hy * M1[mu(i)][mu(j)]*M1[nu(i)][nu(j)]*G1[v(i)][v(j)] / hz;

			 G[i][j] *= lambda_aver;

			 M[i][j] = hx*hy*hz*M1[mu(i)][mu(j)]*M1[nu(i)][nu(j)]*M1[v(i)][v(j)];
		 }
	 }

	 //Собираем локальную матрицу, как блочную
	 for(int i = 0; i < 8; i++){
		 for(int j = 0; j < 8; j++){
			 A_loc[2*i+1][2*j+1] = A_loc[2*i][2*j] = G[i][j] - w*w*epsilon_aver*M[i][j];
			 A_loc[2*i][2*j+1] = - w*sigma_aver*M[i][j];
			 A_loc[2*i+1][2*j] = w*sigma_aver*M[i][j];
		 }
	 }

	 //Теперь начинаем собрать локлаьную правую часть
	 double val_f_sin[8], val_f_cos[8], b_sin[8], b_cos[8];
	 
	 //Вычисляем значения
	 for(int i = 0; i < 8; i++){
	 	 double x = nodes[elements[el_n].node_n[2*i]].x; //координаты точки
		 double y = nodes[elements[el_n].node_n[2*i]].y; //координаты точки
		 double z = nodes[elements[el_n].node_n[2*i]].z; //координаты точки

		 val_f_sin[i] = f_sin(x,y,z);
		 val_f_cos[i] = f_cos(x,y,z);
	 }
	 //Вычисляем подвекторы правой части
	 for(int i = 0; i < 8; i++){
		 b_sin[i] = b_cos[i] = 0;
		 for(int j = 0; j < 8; j++){
			 b_sin[i] += M[i][j]*val_f_sin[j];
			 b_cos[i] += M[i][j]*val_f_cos[j];
		 }
	 }

	 //Соединяем два вектора в один
	 for(int i = 0; i < 8; i++){
		 b_loc[2*i] = b_sin[i];
		 b_loc[2*i+1] = b_cos[i];
	 }


}

template <typename solver_type> int harm_FEM<solver_type>::mu(int i){
	return i % 2;
}
template <typename solver_type> int harm_FEM<solver_type>::nu(int i){
	return (i/2) % 2;
}
template <typename solver_type> int harm_FEM<solver_type>::v(int i){
	return (i/4) % 2;
}


template <typename solver_type> int harm_FEM<solver_type>::find_el_pos(int i, int j){
	int k_s = ig[i], k_e = ig[i+1];
	int cur;
	bool find = false;
	for(int k = k_s; k < k_e && !find; k++){
		if(jg[k] == j){
			cur = k;
			find = true;
		}
	}
	return cur;
}


template <typename solver_type> void harm_FEM<solver_type>::transf_grid(string file_cords, string file_elements, string file_faces){

	//Преобразование файла координат
	FILE* cords_f_in = fopen(file_cords.c_str(), "r"); //Файл исходной сетки
	FILE* cords_f_out = fopen((file_cords + "tr").c_str(), "w"); //Файл модифицированной сетки

	int n; //количество
	fscanf(cords_f_in, "%d", &n);
	fprintf(cords_f_out, "%d\n", 2*n);


	for(int i = 0; i < n; i++){
		double x, y, z; //считываем координаты
		fscanf(cords_f_in, "%lf %lf %lf",&x, &y, &z);
		fprintf(cords_f_out, "%.15lf\t%.15lf\t%.15lf\n", x, y, z);
		fprintf(cords_f_out, "%.15lf\t%.15lf\t%.15lf\n", x, y, z);
	}

	fclose(cords_f_in);
	fclose(cords_f_out);

	//Преобразование файла элементов

	FILE* els_f_in = fopen(file_elements.c_str(), "r"); //Файл исходной сетки
	FILE* els_f_out = fopen((file_elements+"tr").c_str(), "w"); //Файл модифицированной сетки

	fscanf(els_f_in, "%d", &n);
	fprintf(el_f_out, "%d\n", n);

	for(int i = 0; i < n; i++){
		for(int j = 0; j < 8; j++){
			int k; //текущая вершина
			fscanf(els_f_in, "%d", &k);
			fprintf(els_f_out, "%d %d", 2*k, 2*k+1);
		}
		fprintf(els_f_out, "\n");
	}

	fclose(els_f_in);
	fclose(els_f_out);

	//Преобразование файла граней

	FILE* face_f_in = fopen(file_faces.c_str(), "r"); //Файл исходной сетки
	FILE* face_f_out = fopen((file_faces+"tr").c_str(), "w"); //Файл модифицированной сетки

	fscanf(face_f_in, "%d", &n);
	fprintf(face_f_out, "%d\n", n);

	for(int i = 0; i < n; i++){
		for(int j = 0; j < 4; j++){
			int k; //текущая вершина
			fscanf(face_f_in, "%d", &k);
			fprintf(face_f_out, "%d %d", 2*k, 2*k+1);
		}
		fprintf(face_f_out, "\n");
	}

	fclose(face_f_in);
	fclose(face_f_out);
}
