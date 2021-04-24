#pragma once

#include <stdio.h>
#include <omp.h> //���������!
#include <vector>
#include <algorithm>

using namespace std;

#include "random_generator.h"

//�����, ����������� ���������� ���������
class easy_random_search{
 public:
	 void init_rectangle(vect_2d s_x0, double s_hx, double s_hy); //��������� ������ �������������� � ��������� �� ������ �� ����
	 void init_tests(double eps, double P); //��������� ����� ������������ ����� (��������������� �������� � ��������� ����������� ����������, ����� �������� �������������)
	 void init_func(func_2d s_func); //��������� ������� �������


	 vect_2d search(); //����� ��������
 private:

	 vect_2d x0; //������� �����������
	 func_2d min_f; //������������� ������
	 double min_f_val; //������� �������� ��������

	 vect_2d left_bottom, right_top; //������� ��������������

	 long int tests_N; //���������� ������������� �����
	 random_generator rand_gen; //��������� ��������� �����
};