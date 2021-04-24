#pragma once

#include "extra.h"
#include <vector>

using namespace std;

typedef double(*func_2d)(vect_2d); //������� ���� ����������
typedef double(*penalty_func)(func_2d, vect_2d); //������� ������

//����� ��������, �������������� ��� ������������ ������� � �������������

class functor{
 public:
	  //����������� ��-���������
	 functor(){
	 }
	 //������������ � �����������
	 functor(func_2d set_mf, double set_r0, vector<func_2d> set_bounds, vector<penalty_func> set_pen, vector<double> set_pen_coef){
		 init(set_mf, set_r0, set_bounds, set_pen, set_pen_coef);
	 }


	 //�������������
	 void init(func_2d set_mf, double set_r0, vector<func_2d> set_bounds, vector<penalty_func> set_pen, vector<double> set_pen_coef){
		 main_f = set_mf;
		 penal_r0 = set_r0;
		 bounds = set_bounds;
		 penaltys = set_pen;
		 penaltys_coef = set_pen_coef;
	 }

	 //���������� ����������
	 double operator()(vect_2d arg){
		 double rez; //��������� ����������
		 double penalty = 0; //�����
		 for(int i = 0; i < penaltys.size(); i++){
			 penalty += penaltys_coef[i] * penaltys[i](bounds[i], arg); //����� �� ����������� ����������
		 }

		 rez = main_f(arg) + penal_r0 * penalty;

		 return rez;
	 }

	 //���������� �������� �������� �������
	 double main_val(vect_2d arg){
		 return main_f(arg);
	 }

	 //��������� �����
	 void panalty_grow(){
		 for(int i = 0; i < penaltys_coef.size(); i++){
			 penaltys_coef[i] *= 10.0;
		 }
	 }

	//��������� �����
	void panalty_low(){
		 for(int i = 0; i < penaltys_coef.size(); i++){
			 penaltys_coef[i] /= 10.0;
		 }
	 }

	bool check_penalty(vect_2d arg){
		for(int i = 0; i <  bounds.size(); i++){
			if(bounds[i](arg) > 0)
				return false;
		}
		return true;
	}

 private:
	 func_2d main_f; //�������� �������
	 double penal_r0; //�������� ����������� �������

	 vector<func_2d> bounds; //�����������
	 vector<penalty_func> penaltys; //�������� �������
	 vector<double> penaltys_coef; //����������� ������

};