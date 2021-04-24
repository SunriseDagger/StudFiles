#pragma once

#include "grid_gen.h"

//��������������� ��������� ��� ������� �����
struct gen_point{
	double x, y, z;

	gen_point(){
		x = y = z = 0;
	}

	gen_point(double set_x, double set_y){
		x = set_x;
		y = set_y;
		z = 0;
	} 

	gen_point(double set_x, double set_y, double set_z){
		x = set_x;
		y = set_y;
		z = set_z;
	} 
};

//��������� ���������� ������������� ������

class grid_gen_rect{
 public:
	 //===================��������� ������ ��������� �����===================
	 //============ ��������� ������������� ����� ============
	 //left_bottom - ������ ����� ���� ��������������
	 //right_top - ������ ������� ���� ��������������
	 //h_xmin, h_ymin - ����������� ��� �� x � �� y ��������������
	 //k_x, k_y - ������������ �������� �� ������ ����������
	 //���������� true, ���� ����� ����� ���� ��������������, false - � ��������� ������
	 //n - ������������ ��������, ����� ����� �����
	static bool generate_unreg_grid(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, string file_name);  //��������� � ���� � ������ file_name
	static bool generate_unreg_grid(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, int &n, double  **&grid_mass);  //��������� � ��������� ������ grid_vector

	//============ ��������� ����������� ����� ===========
	static bool generate_reg_grid(gen_point left_bottom, gen_point right_top, double h_x, double h_y, string file_name);
	static bool generate_reg_grid(gen_point left_bottom, gen_point right_top, double h_x, double h_y, int &n, double  **&grid_mass);

	
	//============ ��������� ��������� ����� ============
	//��� �������������
	static bool generate_unreg_grid_ins(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, string file_name);
	static bool generate_unreg_grid_ins(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, int &n, double  **&grid_mass);

	//��� �����������
	static bool generate_reg_grid_ins(gen_point left_bottom, gen_point right_top, double h_x, double h_y, string file_name);
	static bool generate_reg_grid_ins(gen_point left_bottom, gen_point right_top, double h_x, double h_y, int &n, double  **&grid_mass);
 
 private:
	static int generate_unreg_grid_pr(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double k_x, double k_y, double **&grid_mass); 
   //grid_mass - ����� ����� �������� �����
	static int generate_reg_grid_pr(gen_point left_bottom, gen_point right_top, double h_x, double h_y, double **&grid_mass); 
   //grid_mass - ����� ����� �������� �����

};
