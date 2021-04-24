#include "grid_gen_cube.h"

int grid_gen_cube::generate_unreg_grid_pr(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, double k_x, double k_y, double k_z, gen_point *&grid_mass){
	double* mass_x; // x-���������� �����
	double* mass_y; // y-���������� �����
	double* mass_z; // z-���������� �����
	int n_x, n_y, n_z; //���������� �����

	//���������� ��� ����� �������� ��������� �� ���������
	double a_x = left_bottom.x;
	double a_y = left_bottom.y;
	double a_z = left_bottom.z;

	//���������� ��� ������ �������� ��������� �� ���������
	double b_x = right_top.x;
	double b_y = right_top.y;
	double b_z = right_top.z;

	//��������� ���������� ����� �� ����������
	grid_gen::generate_unreg_grid(a_x, b_x, h_xmin, k_x, n_x, mass_x);
	grid_gen::generate_unreg_grid(a_y, b_y, h_ymin, k_y, n_y, mass_y);
	grid_gen::generate_unreg_grid(a_z, b_z, h_zmin, k_z, n_z, mass_z);

	int n_xy = n_x * n_y; //���������� ����� � ����� ��������� xy
	int n = n_xy * n_z;  //����� ����� ����� �����

	grid_mass = new gen_point [n];

	for(int i = 0; i < n_x; i++){
		for(int j = 0; j < n_y; j++){
			for(int k = 0; k < n_z; k++){
				grid_mass[k*n_xy + j*n_x + i].x = mass_x[i];
				grid_mass[k*n_xy + j*n_x + i].y = mass_y[j];
				grid_mass[k*n_xy + j*n_x + i].z = mass_z[k];
			}
		}
	}

	delete[] mass_x;
	delete[] mass_y;
	delete[] mass_z;

	return n;
}

bool grid_gen_cube::generate_unreg_grid(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, double k_x, double k_y, double k_z, std::string file_name){
	//�������� ����������� ������, ��������� ������������ ������
	//���������� ��� ����� �������� ��������� �� ���������
	double a_x = left_bottom.x;
	double a_y = left_bottom.y;
	double a_z = left_bottom.z;

	//���������� ��� ������ �������� ��������� �� ���������
	double b_x = right_top.x;
	double b_y = right_top.y;
	double b_z = right_top.z;

	//�������� ����������� ������, �������� �� ����������
	bool check_x = grid_gen::check_data(a_x, b_x, h_xmin, k_x);
	bool check_y = grid_gen::check_data(a_y, b_y, h_ymin, k_y);
	bool check_z = grid_gen::check_data(a_z, b_z, h_zmin, k_z);

	if(!(check_x && check_y && check_z)) return false; //�� ��� � ���� ��������

	gen_point* grid_mass; //���� �����
	int n = generate_unreg_grid_pr(left_bottom, right_top, h_xmin, h_ymin, h_zmin, k_x, k_y, k_z, grid_mass);

	FILE* out_f = fopen(file_name.c_str(), "w");

	fprintf(out_f, "%d\n", n);
	
	for(int i = 0; i < n; i++){
		fprintf(out_f, "%.15lf\t%.15lf\t%.15lf\n", grid_mass[i].x, grid_mass[i].y, grid_mass[i].z);
	}

	fclose(out_f);

	delete[] grid_mass;

	return true;
}

//� ����� ������ - ������������ � ����� ���������, ����� ��� ��������, ������.
bool grid_gen_cube::generate_unreg_grid_FEM(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, double k_x, double k_y, double k_z, string file_cords, string file_elements, string file_faces){

	if(!generate_unreg_grid(left_bottom, right_top, h_xmin, h_ymin, h_zmin, k_x, k_y, k_z, file_cords)) return false; //����� ����� � �������� �� ������������ �� ����

	//���������� ��� ����� �������� ��������� �� ���������
	double a_x = left_bottom.x;
	double a_y = left_bottom.y;
	double a_z = left_bottom.z;

	//���������� ��� ������ �������� ��������� �� ���������
	double b_x = right_top.x;
	double b_y = right_top.y;
	double b_z = right_top.z;

	//������� ��������

	FILE* out_f = fopen(file_elements.c_str(), "w");

	//������ ����� - ���� ��������, � 7 ������ ���������� � ��� ���������� � ��, ����� ������
	//��� ������ ��� ����������� ���������� ����� �� ������ ����������

	int n_x = grid_gen::nodes_number_gen_unreg(a_x, b_x, h_xmin, k_x);
	int n_y = grid_gen::nodes_number_gen_unreg(a_y, b_y, h_ymin, k_y);
	int n_z = grid_gen::nodes_number_gen_unreg(a_z, b_z, h_zmin, k_z);

	int n_xy = n_x * n_y; // ��� ���� ����

	fprintf(out_f, "%d\n", (n_x-1)*(n_y-1)*(n_z-1));

	for(int i = 0; i < n_x - 1; i++){
		for(int j = 0; j < n_y - 1; j++){
			for(int k = 0; k < n_z - 1; k++){
				int node_i0 = k*n_xy + j*n_y + i; //���������� ����� �������
				//��������� 7 ������
				//��, ������� � ��� �� ������� �� xy
				int node_i1 = k*n_xy + j*n_y + i+1;
				int node_i2 = k*n_xy + (j+1)*n_x + i;
				int node_i3 = k*n_xy + (j+1)*n_x + i + 1;

				//��, ������� ����
				int node_i4 = (k+1)*n_xy + j*n_y + i;
				int node_i5 = (k+1)*n_xy + j*n_y + i+1;
				int node_i6 = (k+1)*n_xy + (j+1)*n_x + i;
				int node_i7 = (k+1)*n_xy + (j+1)*n_x + i + 1;

				fprintf(out_f, "%d %d %d %d %d %d %d %d\n", node_i0, node_i1, node_i2, node_i3, node_i4, node_i5, node_i6, node_i7);
			}
		}
	}

	fclose(out_f);

	//� ����� - �����

	out_f = fopen(file_faces.c_str(), "w");

	fprintf(out_f, "%d\n", 2*(n_x-1)*(n_y-1) + 2*(n_x-1)*(n_z-1) + 2*(n_y-1)*(n_z-1)); //����� ���������� ������

	//��� z = z_min
	for(int i = 0; i < n_x - 1; i++){
		for(int j = 0; j < n_y - 1; j++){
			int node_i0 = j*n_x + i;
			int node_i1 = j*n_x + i + 1;
			int node_i2 = (j+1)*n_x + i;
			int node_i3 = (j+1)*n_x + i + 1;

			fprintf(out_f, "%d %d %d %d\n", node_i0, node_i1, node_i2, node_i3);
		}
	}

	//��� z = z_max

	for(int i = 0; i < n_x - 1; i++){
		for(int j = 0; j < n_y - 1; j++){
			int node_i0 = (n_z-1)*n_xy + j*n_x + i;
			int node_i1 = (n_z-1)*n_xy + j*n_x + i + 1;
			int node_i2 = (n_z-1)*n_xy + (j+1)*n_x + i;
			int node_i3 = (n_z-1)*n_xy + (j+1)*n_x + i + 1;

			fprintf(out_f, "%d %d %d %d\n", node_i0, node_i1, node_i2, node_i3);
		}
	}

	//��� y = y_min
	for(int i = 0; i < n_x - 1; i++){
		for(int k = 0; k < n_z - 1; k++){
			int node_i0 = k*n_xy + i;
			int node_i1 = k*n_xy + i + 1;
			int node_i2 = (k+1)*n_xy + i;
			int node_i3 = (k+1)*n_xy + i + 1;

			fprintf(out_f, "%d %d %d %d\n", node_i0, node_i1, node_i2, node_i3);
		}
	}

	//��� y = y_max
	for(int i = 0; i < n_x - 1; i++){
		for(int k = 0; k < n_z - 1; k++){
			int node_i0 = k*n_xy + (n_y-1)*n_x + i;
			int node_i1 = k*n_xy + (n_y-1)*n_x + i + 1;
			int node_i2 = (k+1)*n_xy + (n_y-1)*n_x + i;
			int node_i3 = (k+1)*n_xy + (n_y-1)*n_x + i + 1;

			fprintf(out_f, "%d %d %d %d\n", node_i0, node_i1, node_i2, node_i3);
		}
	}

	//��� x = x_min
	for(int j = 0; j < n_y - 1; j++){
		for(int k = 0; k < n_z - 1; k++){
			int node_i0 = k*n_xy + j*n_x;
			int node_i1 = k*n_xy + (j+1)*n_x;
			int node_i2 = (k+1)*n_xy + j*n_x;
			int node_i3 = (k+1)*n_xy + (j+1)*n_x;

			fprintf(out_f, "%d %d %d %d\n", node_i0, node_i1, node_i2, node_i3);
		}
	}

	//��� x = x_max
	for(int j = 0; j < n_y - 1; j++){
		for(int k = 0; k < n_z - 1; k++){
			int node_i0 = k*n_xy + j*n_x + n_x-1;
			int node_i1 = k*n_xy + (j+1)*n_x + n_x-1;
			int node_i2 = (k+1)*n_xy + j*n_x + n_x-1;
			int node_i3 = (k+1)*n_xy + (j+1)*n_x + n_x-1;

			fprintf(out_f, "%d %d %d %d\n", node_i0, node_i1, node_i2, node_i3);
		}
	}

	fclose(out_f);

	return true;

}

void grid_gen_cube::generate_face_nodes(string file_faces, string file_f_nodes){
	FILE* inp_f = fopen(file_faces.c_str(), "r");

	int face_n;
	fscanf(inp_f, "%d", &face_n);
	set<int> nodes;

	for(int i = 0; i < face_n; i++){
		int tmp_node;
		if(i % 100 == 0)
			printf("%d from %d complite\n", i, face_n);
		for(int j = 0; j < 4; j++){
			fscanf(inp_f, "%d", &tmp_node);
			nodes.insert(tmp_node);
		}
	}
	fclose(inp_f);

	FILE* out_f = fopen(file_f_nodes.c_str(), "w");
	fprintf(out_f, "%d\n", nodes.size());
	set<int>::iterator iter = nodes.begin();
	for(int i = 0; i < nodes.size(); i++){
		fprintf(out_f, "%d\n", *iter);
		iter++;
	}
	fclose(out_f);

}