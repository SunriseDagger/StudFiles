#pragma once

#include "grid_gen_rect.h"
#include <set>

//��������� ��������� ������������������� �����, � ���������� �� ���

class grid_gen_cube{
 public:
	 //===================��������� ������ ��������� �����===================
	 //============ ��������� ������������� ����� ============
	 //left_bottom - ������ ����� ���� �������������
	 //right_top - ������ ������� ���� �������������
	 //h_xmin, h_ymin, h_zmin - ����������� ��� �� x, y � z ��������������
	 //k_x, k_y, k_z - ������������ �������� �� ������ ����������
	 //���������� true, ���� ����� ����� ���� ��������������, false - � ��������� ������
	 //n - ������������ ��������, ����� ����� �����
	 static bool generate_unreg_grid(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, double k_x, double k_y, double k_z, string file_name);  //��������� � ���� � ������ file_name
	 static bool generate_unreg_grid_FEM(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, double k_x, double k_y, double k_z, string file_cords, string file_elements, string file_faces); 
	 //��������� ���-�����
	 //file_cords - ���������� �����, ����� - ����������
	 //file_elements - ���������� ���������, ����� ������ ����� ���������
	 //file_faces - ���������� ������, ����� ������ ����� ������

	  static void generate_face_nodes(string file_faces, string file_f_nodes); //�������� ���� �� ������
	 //file_faces - ���� ����
	 //file_f_nodes - �������� ���� � �������� �����
 private:
	static int generate_unreg_grid_pr(gen_point left_bottom, gen_point right_top, double h_xmin, double h_ymin, double h_zmin, double k_x, double k_y, double k_z, gen_point *&grid_mass); //���������� ��������� �����
	 //grid_mass - ����� ����� �������� �����
};