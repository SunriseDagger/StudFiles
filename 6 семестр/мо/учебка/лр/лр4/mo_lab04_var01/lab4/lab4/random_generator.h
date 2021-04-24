#pragma once

#include <stdlib.h>

#include "extra.h"

typedef double(*func_2d)(vect_2d); //������� ���� ����������

//�����, ����������� ��������� ��������� ��������� ����� � ��������������
class random_generator{
 public:

	 //������������ �� ��������� - ����������� ���� 0
	 random_generator(){
		 srand(0);
	 }

	 //����������� � ��������� ������ �������
	 random_generator(unsigned int core){
		 srand(core);
	 }

	 //�������, ������������ ��������� ����� � ��������� ��������������
	 //left_bottom - ������ ����� ������� ��������������
	 //right_top - ������ ������� ������� ��������������
	 vect_2d generate(vect_2d left_bottom, vect_2d right_top){
		 //��� ��������� ����� � ������� [0,1]
		 double c_x = (rand()*1.0 + 1.0)/ RAND_MAX;
		 double c_y = (rand()*1.0 + 1.0)/ RAND_MAX;

		 //��������� ����� � ��� �������������
		 double h_x = fabs(right_top.x - left_bottom.x); //����� �������������� �� ��� x
		 double h_y = fabs(right_top.y - left_bottom.y); //����� �������������� �� ��� y

		 double x = c_x * h_x + left_bottom.x; //���������� x ������������ �����
		 double y = c_y * h_y + left_bottom.y; //���������� y ������������ �����
		 return vect_2d(x, y);
	 }

};