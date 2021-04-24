#include "grid_gen_cube.h"
#include <sstream>

int main(){
	int n;
	double *gr_m;
	vector<double> d;

	grid_gen_cube::generate_unreg_grid_FEM(gen_point(0,0,0), gen_point(1,1,1), 0.9, 0.9, 0.9, 1.1, 1.1, 1.1, "FEM_cord.txt", "FEM_elems.txt", "FEM_faces.txt");

	return 0;
}


//Специальный мейн для генерации неравномерной сетки из вне
/*int main(int argc, char **argv){
	if(argc!=6) printf("Something wrong with arguments =(\n");
	else{
		string f_name(argv[5], sizeof(argv[5]));
		double a, b, h_min, k;

		istringstream arg0(argv[1]);
		arg0 >> a;

		istringstream arg1(argv[2]);
		arg1 >> b;

		istringstream arg2(argv[3]);
		arg2 >> h_min;

		istringstream arg3(argv[4]);
		arg3 >> k;
		
		bool bild_we = grid_gen::generate_unreg_grid(a, b, h_min, k, f_name);

		if(bild_we) printf("It's ok! Grid had been generated!\n");
		else
			printf("Something gone wrong =(\n");

	}
	return 0;
}*/