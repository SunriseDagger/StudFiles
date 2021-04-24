#pragma once

#include <stdio.h>

#include "functor.h"

//����� ������, ��� ���������� ������� ���� ���������� � �������������, � �������� ����������� ������ ������������ ����� ���������

class Gauss{
 public:
	Gauss(){} //����������� ��-���������
	void set_func(functor set_min_f); //��������� �������������� �������
	void set_start(vect_2d set_x0); //��������� ���������� �����������
	void set_eps(double set_eps_G); //��������� ��������� ��������
	vect_2d find_min(); //����� ��������
 private:
	 functor min_f; //�������������� �������
	 vect_2d x_k; //�����������
	 double eps_G; //��������
	 int func_calc; //���������� ���������� �������

	 //x_i - ��� ����� ����������, ������� ������ ��������������
	 double Fib(double a, double b, int x_i); // ������������ ���������� ����� ������ - ����� ���������
	 void find_area(double x0, double& a, double& b, int x_i); // ������������ ����� ��� ����������� ������� ����������� ��������
	 double one_min_f(double x0, int x_i); // ������� ��� ���������� �����������
	

};