#include "base_classes.h"
#include "grid_gen_tet.h"
#include "timer.h"

#include <sstream>
using namespace std;

void gen_grid(); //Генерируем сетку итд
void gener_starts(); //генерация начальных условий

int main(){

//	gen_grid();
	gener_starts();

	Grid g1;
	string f_m;

	FILE* f;

	f_m = path + "el.txt";
	f = fopen(f_m.c_str(), "r");
	g1.input_elements(f);
	fclose(f);

	f_m = path + "nodes.txt";
	f = fopen(f_m.c_str(), "r");
	g1.input_nodes(f);
	fclose(f);

	f_m = path + "faces_s.txt";
	f = fopen(f_m.c_str(),"r");
	g1.input_faces_s(f);
	fclose(f);

	f_m = path + "faces_s_ar.txt";
	f = fopen(f_m.c_str(),"r");
	g1.input_f_s_areas(f);
	fclose(f);

	f_m = path + "faces_t.txt";
	f = fopen(f_m.c_str(),"r");
	g1.input_faces_t(f);
	fclose(f);

	f_m = path + "faces_t_ar.txt";
	f = fopen(f_m.c_str(),"r");
	g1.input_f_t_areas(f);
	fclose(f);


	f_m = path + "first_bound.txt";
	f = fopen(f_m.c_str(),"r");
	g1.input_fir_nodes(f);
	fclose(f);

	f_m = path + "el_areas.txt";
	f = fopen(f_m.c_str(),"r");
	g1.input_el_areas(f);
	fclose(f);

	//Инифиализация всего, что связанно со временем
	f_m = path + "time data.txt";
	f = fopen(f_m.c_str(), "r");
	g1.input_time_data(f);
	fclose(f);

	FILE *f1, *f2;
	f_m = path + "time0.txt";
	f = fopen(f_m.c_str(), "r");

	f_m = path + "time1.txt";
	f1 = fopen(f_m.c_str(), "r");

	f_m = path + "time2.txt";
	f2 = fopen(f_m.c_str(), "r");
	g1.input_start(f, f1, f2);
	fclose(f);
	fclose(f1);
	fclose(f2);

	timer our_timer; //чтобы мерить время



	printf("Generate matrix portret will began\n");

	our_timer.start();

	g1.gen_SLAE_port();

	double time = our_timer.time();
	printf("Generate portres complite in %.2lf seconds\n", time);

	double total_time = time;

	f_m = path + "diffs.txt";
	f1 = fopen(f_m.c_str(), "w");
	f_m = path + "result.txt";
	f = fopen(f_m.c_str(),"w");
	for(int i = 0; i < 50; i++){

		stringstream a;
		a << i;
		string tmp_str;
		a >> tmp_str;

		//f_m = path + "result" + tmp_str + ".txt";
		//f = fopen(f_m.c_str(),"w");

		our_timer.start();
		g1.gen_SLAE();
		g1.solve_SLAE();

		time = our_timer.time();
		total_time += time;

		g1.output_weight(f);
		g1.output_diff(f1);
		printf("%d iteration is complite in %.2lf seconds\n", i, time);

		//fprintf(f, "\n\n");
	}

	printf("Total time is %.2lf seconds\n", total_time);


	
	fclose(f);
	fclose(f1);


/*
	double po[12][3] = {{0,0.0625,0.125}, {0.0625,0,0.125}, {0.0625, 0.0625, 0.125}, {0, 0, 0.1875}, {0.0625, 0 ,0.1875}, {0, 0.0625, 0.1875}, {0.1875, 0, 0}, {0.1875, 0 ,0.0625}, {0.125, 0 , 0.0625}, {0.1875, 0.0625, 0}, {0.125, 0.0625, 0.0625}, {0.125, 0.0625, 0} };

	f_m = path + "res_inpoint.txt";
	f = fopen(f_m.c_str(),"w");
	for(int i = 0; i < 12; i++)
		fprintf(f,"%.15lf\n",g1.u_in_el(po[i][0],po[i][1],po[i][2]));
	fclose(f);
*/

	return 0;
}

void gen_grid(){

	//Сначало строим кубическую сетку
	double hx = 0.2;
	double hy = 0.2;
	double hz = 0.2;

	double kx = 1.0000000001;
	double ky = 1.0000000001;
	double kz = 1.0000000001;
	grid_gen_cube::generate_unreg_grid_FEM(gen_point(0, 0, 0), gen_point(10, 10, 10), hz, hy, hz, kx, ky, kz, "nodes.txt", "cube_elements.txt", "cube_faces.txt");

	//Далее проебразуем её в тетраэдальную сетку
	grid_gen_tet::transform_cube_to_tetrahedron("cube_elements.txt", "cube_faces.txt", "el.txt", "faces.txt");
	grid_gen_cube::generate_face_nodes("cube_faces.txt", "faces_nodes.txt");


	//grid_gen_tet::ins_grid("to gen/el.txt", "to gen/face1.txt", "to gen/nodes.txt", "el.txt", "1face.txt", "nodes.txt");
	//grid_gen_tet::generate_face_nodes("1face.txt", "faces_nodes.txt");

	FILE* inp_f = fopen("faces_nodes.txt", "r");
	FILE* out_f = fopen("first_bound.txt", "w");

	int fir_b_n;
	fscanf(inp_f, "%d", &fir_b_n);
	fprintf(out_f, "%d\n", fir_b_n);
	int tmp_int;

	for(int i = 0; i < fir_b_n; i++){
		fscanf(inp_f,"%d", &tmp_int);
		fprintf(out_f, "%d %d\n", tmp_int, 0);
	}
	fclose(inp_f);
	fclose(out_f);

	int els_n;
	inp_f = fopen("el.txt", "r");
	fscanf(inp_f, "%d", &els_n);
	fclose(inp_f);

	out_f = fopen("el_areas.txt", "w");
	for(int i = 0; i < els_n; i++)
		fprintf(out_f, "%d %d\n", i, 0);

	fclose(out_f);

}



void gener_starts(){
	string f_m;
	int node_n;
	double x, y, z;

	//Считываемм данные о времени
	f_m = path + "time data.txt";
	FILE* fd = fopen(f_m.c_str(), "r");
	double t0, tau0, k;
	fscanf(fd, "%lf %lf %lf",&t0, &tau0, &k);
	fclose(fd);
	double t1, t2;
	t1 = t0 + tau0;
	t2 = t1 + k*tau0;

	//Открывае файлы для вывода
	FILE *f0, *f1, *f2;
	f_m = path + "time0.txt";
	f0 = fopen(f_m.c_str(), "w");

	f_m = path + "time1.txt";
	f1 = fopen(f_m.c_str(), "w");

	f_m = path + "time2.txt";
	f2 = fopen(f_m.c_str(), "w");

	//Считываем данные об узлах
	f_m = path + "nodes.txt";
	FILE* fn = fopen(f_m.c_str(), "r");
	fscanf(fn, "%d", &node_n);

	//Генерируем
	for(int i = 0; i < node_n; i++){
		fscanf(fn, "%lf %lf %lf", &x, &y, &z);
		fprintf(f0, "%.15lf\n", sol_func(x,y,z,t2));
		fprintf(f1, "%.15lf\n", sol_func(x,y,z,t1));
		fprintf(f2, "%.15lf\n", sol_func(x,y,z,t0));
	}

	fclose(fn);
	fclose(f0);
	fclose(f1);
	fclose(f2);
}