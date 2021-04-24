#pragma once

#include <math.h>
#include <string>

typedef double(*func)(double, double, double, double);
using namespace std;

namespace area_1{
	extern string path; //���� ����� � �������
	extern int lambda_n, betta_n, u_betta_n, tetta_n, fir_n; //���������� ��� ��� ���� ������

	//���� ������
	extern double *lambda_values;
	extern double *betta_values;
	extern func *gamma_values;
	extern func *sigma_values;
	extern func *epsilon_values;
	extern func *u_betta_values;
	extern func *tetta_values;
	extern func *fir_values;

	double sol_func(double x, double y, double z, double t); //�������
	double equation_rp(double x, double y, double z, double t); //������ ����� ���������(������ ������� V(x,y,z,t), f = dV/dx)

	void init_area(); //���������������� ������
	
};
	
namespace area_2{
	extern string path; //���� ����� � �������
	extern int lambda_n, betta_n, u_betta_n, tetta_n, fir_n; //���������� ��� ��� ���� ������

	//���� ������
	extern double *lambda_values;
	extern double *betta_values;
	extern func *gamma_values;
	extern func *sigma_values;
	extern func *epsilon_values;
	extern func *u_betta_values;
	extern func *tetta_values;
	extern func *fir_values;

	double sol_func(double x, double y, double z, double t); //�������
	double equation_rp(double x, double y, double z, double t); //������ ����� ���������(������ ������� V(x,y,z,t), f = dV/dx)

	void init_area(); //���������������� ������
	
};
	
namespace area_3{
	extern string path; //���� ����� � �������
	extern int lambda_n, betta_n, u_betta_n, tetta_n, fir_n; //���������� ��� ��� ���� ������

	//���� ������
	extern double *lambda_values;
	extern double *betta_values;
	extern func *gamma_values;
	extern func *sigma_values;
	extern func *epsilon_values;
	extern func *u_betta_values;
	extern func *tetta_values;
	extern func *fir_values;

	double sol_func(double x, double y, double z, double t); //�������
	double equation_rp(double x, double y, double z, double t); //������ ����� ���������(������ ������� V(x,y,z,t), f = dV/dx)

	void init_area(); //���������������� ������
	
};