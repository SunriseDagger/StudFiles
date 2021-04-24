#pragma once

#include <math.h>

//�������� ������� LU-���������� ��� ���� � �������������� �������� � ���������� �������

class solver_LU{
 public:
	 void init(int* s_ig, int* s_jg, double* s_gu, double* s_gl, double* s_di, int s_n); //�������������, ������� �� ������������ � ���������� ������
	 void set_rp(double* s_rp); //��������� ������ �����
	 void solve(double *&solution, int &its); //��������� ������� � ���������� ��������
 private:

	 int n; //����������� ����

	 //�������
	 int *ig;
	 double *gl, *gu, *di;
	 double *rp;

	 void dec(); //����������� LU-����������

};