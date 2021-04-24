#pragma once

#include <stdio.h>
#include <sstream>
#include <string>

#include "additional_classes.h"
#include "areas.h"
#include "CGM.h"
#include "Gauss.h"

using namespace area_3;
using namespace std;

struct first_boundary{
	int node_numb;
	int area;
};

//����� ��� ������� ���������������� ���������
//������������� �� �������: ������� 4-� ������� �����
//�������� �������: �������� �� ����������

class Grid{
 public:

	 Grid();

	//����
	 void input_nodes(FILE* nodes_f);
	 void input_elements(FILE* el_f);
	 void input_faces_s(FILE* face_f_sec);
	 void input_faces_t(FILE* faces_f_thi);
	 void input_fir_nodes(FILE *fir_f);

	 void input_time_data(FILE* time_d); //���� ���������� � �������: ��������� ��� � ���������� ��������
	 void input_start(FILE* st0, FILE* st1, FILE* st2);  //���� ��������� ������� �� i-2, i-1 � i-�� ���������

	 void input_el_areas(FILE *el_a_f);
	 void input_f_s_areas(FILE *fs_a_f);
	 void input_f_t_areas(FILE *ft_a_f);

	 double u_in_el(double x, double y, double z); //��������� �������� ������� � �������� ��������� ���� � �����


	 void gen_SLAE_port(); //������ ��������� �������� ����
	 void gen_SLAE(); //������ ��������� ����
	 void solve_SLAE(); //������ ������� ����

	 void init_areas(FILE* elem_areas, FILE* fir_areas, FILE* sec_areas, FILE* thi_areas);

	 void output_weight(FILE* outp_f); //����� �������� �������
	 void output_diff(FILE* outp_f);  //����� ������� �����������

 private:
	 node* nodes; //������ �����
	 FE* elements; //������ ���������
	 face *faces_s, *faces_t; //������� ��� ������ � ������� �������
	 first_boundary* fir_b; //������ ��� ������ �������

	 double *weight_0; //���� � i-�� ��������
	 double *weight_1; //���� � (i-1)-�� ��������
	 double *weight_2; //���� � (i-2)-�� ��������

	 //�������������� ����� �������
	 double t0;
	 double t1;
	 double t2;
	 double t_new; //����� �� i+1 ��������
	 double k_time; //���������� �������� �� �������

	 int nodes_number, elements_number, faces_number_sec, faces_number_thi, first_number; //���������� �����, ��������� � ������� �� ������� �������

	 //������� ��� �������
	 int *gi, *gj;
	 double *gg, *di, *right_part;
	 int SLAE_el_numbers;
	 
	 SLAE_port_gen SLAE_gen; //����� ��� ��������� �������� ����

	//��������� ��������� ������ ���������� �������� ��� �����
	 void gen_local_el(double **A_loc, double *b_loc, int el_n);
	 void gen_local_thi(double **A_loc, double *b_loc, int f_n);
	 void gen_local_sec(double *b_loc, int f_n);

	//��������� �������� ����������� �� ��������
	 double get_lambda_value(int el_n);
	 double get_gamma_value(int el_n);
	 double get_sigma_value(int el_n, double t);
	 double get_epsilon_value(int el_n, double t);
	 double get_rp_value(int node_n);

	 //��������� �������� ������������ �� �����
	 double get_betta_value(int f_n);
	 double get_u_betta_value(int f_n, int node_n);
	 double get_tetta_value(int f_n, int node_n);

	 //��������� �������� ������� �������� � ����
	 double get_func_value_fir(int node_n);

	//��������� ������� �����
	 double get_face_mes(int boun, int f_n);

	//���������� ������ �������� ���� � �������� �������������	
	 int find_el_pos(int i, int j);

	 //��������������� ������� ��� ������������ ���������
	 double **D, **alpha;

	 //��� ��� �������
	 int sourse; //����� ���� � ������� ��������

};
