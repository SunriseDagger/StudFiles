#pragma once

#include <math.h>
#include <stdio.h>

#include <string>
#include <vector>

using namespace std;

typedef vector<double> dvector;

// ��������� ���������� �����

class grid_gen{
 public:
	 //===================��������� ������ ��������� �����===================
	 //============ ��������� ������������� ����� ============
	 //a - ������ �������
	 //b - ����� �������
	 //h_min - ����������� ���
	 //k - ����������� ��������
	 //���������� true, ���� ����� ����� ���� ��������������, false - � ��������� ������
	 //n - ������������ ��������, ����� ����� �����
	/*static bool generate_unreg_grid(double a, double b, double h_min, double k, string file_name); //��������� � ���� � ������ file_name
	static bool generate_unreg_grid(double a, double b, double h_min, double k, dvector &grid_vector); //��������� � ������ grid_vector
	static bool generate_unreg_grid(double a, double b, double h_min, double k, int &n, double*& grid_mass); //��������� � ������ grid_vector*/

	//============ ��������� ����������� ����� ============
	//�� ����, � �������� ���� �����
	//static bool generate_reg_grid(double a, double b, double h, string file_name); //��������� � ���� � ������ file_name
	//static bool generate_reg_grid(double a, double b, double h, dvector &grid_vector); //��������� � ������ grid_vector
	static bool generate_reg_grid(double a, double b, double h, int &n, double*& grid_mass); //��������� � ������ grid_vector

	//============ ��������� ��������� ����� ============
	//��� �������������
	/*static bool generate_unreg_grid_ins(double a, double b, double h_min, double k, string file_name);
	static bool generate_unreg_grid_ins(double a, double b, double h_min, double k, dvector &grid_vector);
	static bool generate_unreg_grid_ins(double a, double b, double h_min, double k, int &n, double*& grid_mass);*/

	//��� �����������
	/*static bool generate_reg_grid_ins(double a, double b, double h, string file_name);
	static bool generate_reg_grid_ins(double a, double b, double h, dvector &grid_vector);
	static bool generate_reg_grid_ins(double a, double b, double h, int &n, double*& grid_mass);
*/
	 //============ ��������� �������������� ���������� ============
	/*static int nodes_number_gen_unreg(double a, double b, double h_min, double k); //��������� ���������� ����� ��� ��������� �������� ������
	*/static int nodes_number_gen_reg(double a, double b, double h); //��������� ���������� ����� ��� ��������� �������� ������
	/*static bool check_data(double a, double b, double h_min, double k); //��������� �������� �� ��������� ����� � ������ ������� �������, ���� ����� - true,  ���� ������ - false
*/

 private:

	 /*static int generate_unreg_grid_pr(double a, double b, double h_min, double k, double *&grid_mass); // ���������� ��������� �����
	 *///grid_mass - ����� ����� �������� �����

	 static int generate_reg_grid_pr(double a, double b, double h, double *&grid_mass); // ���������� ��������� �����
	 //grid_mass - ����� ����� �������� �����

};


