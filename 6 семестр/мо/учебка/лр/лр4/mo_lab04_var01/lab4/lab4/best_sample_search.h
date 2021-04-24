#pragma once

#include <stdio.h>

#include <omp.h>
#include <vector>
#include <algorithm>

#include "random_generator.h"

using namespace std;

//�����, ����������� ����� ��������� ����� � ������������ ��������������, � �������
//x - y <= 1
//0 <= x <= 4
class best_sample_search{
 public:
	 void init_rectangle(vect_2d s_x0, double s_hx, double s_hy); //��������� ����������� ����� �������������� � ��������� ����� �� ������
	 void init_tests(double s_eps, double s_P); //��������� ����� ������������ �����(��� ���������� ������ ��� ����� ��������, ����������� � ������� ��������������)
	 void init_func(func_2d s_func); //��������� ������� �������
	 void init_max_iters(int s_max_iters = 100); //��������� ������������� ����� ��������
	 void init_eps_grow(double s_eps_grow = 1E-1); //���������� �������� �������� �������, ��� ������� ������� ��������� �������


	 vect_2d search(); //����� ��������
 private:

	 vect_2d x0; //������� �����������
	 func_2d min_f; //������������� ������
	 double min_f_val; //������� �������� ��������

	 int max_iters; //������������ ����� ���� ���������������

	 vect_2d left_bottom, right_top; //������� ��������������
	 double hx, hy; //����� ������ ��������������
	 void change_rect(); //������� �������������
	 double eps; //�������� (����������� ����� ������� ��������������)
	 double P; //�����������, ���������� �������� � ������ ���������
	 double eps_grow; //�������� �������� �������, ��� ������� ������� ��������� �������

	 int tests_N; //���������� ������������� �����(� ����� ���������������)
	 random_generator rand_gen; //��������� ��������� �����

	 bool check_bounds(vect_2d ch_p); //��������� ����������� �� ����� ch_p ��������� ������������, ���� ����� � ������� - true, ����� - false

};