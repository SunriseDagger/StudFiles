#pragma once

#include "grid_gen_cube.h"
#include <set>
#include <algorithm>

//��������� ��������� ������������� �����, � ����������� �� ���

class grid_gen_tet{
 public:
	 //===================��������� ������ ��������� �����===================
	static void transform_cube_to_tetrahedron(string cube_file_elements, string cube_file_faces, string tet_file_elements, string tet_file_faces); //������������� ���������� ����� � �������������
	//cube_file_elements - ���� ��������� �������� �����
	//cube_file_faces - ���� ������� ������ �������� �����
	//tet_file_elements - ���� ��������� ���������� �����
	//tet_file_faces - ���� ������� ������ ���������� �����

	static void ins_grid(string file_elements, string file_faces, string file_nodes, string ins_file_elements, string ins_file_faces, string ins_file_nodes); //���������� ��������� �����
	//���� ������, �� ���� ��������� �������� ���������� ������� ��� ���� ���. ������� ������ �����, ����� - �����.

	
	 static void generate_face_nodes(string file_faces, string file_f_nodes); //�������� ���� �� ������
	 //file_faces - ���� ����
	 //file_f_nodes - �������� ���� � �������� �����

 private:
	 //���������������� ���������:
	 //�� - ���
	 struct FE_cube{
		int node[8];
	 };
	 //�� - �������� � �� ���� �������������
	 struct FE_tet{
		int node[4];
	 };
	 //�� - �����������
	 struct FE_tri{
		int node[3];
	 };

	 static bool find_tri_in_tets(FE_tri& tri, vector<FE_tet>& tets); //��������������� ������ ��������� ���� �� ����� ����������� ����� ����������

	 //��������������� ��������� ��� ���������� ��������� �����
	 struct edge{ //�����
		 pair<int, int> nodes; //�������� ���� �����
		 int new_node; //����� ����
		 
		 bool operator < (const edge& b) const{
			 return nodes < b.nodes;
		 }

		 bool operator == (const edge& b) const{
			 return nodes == b.nodes;
		 }

		 bool operator <= (const edge& b) const{
			 return this->operator <(b) || this->operator ==(b);
		 }

		 edge(){
			 nodes.first = 0;
			 nodes.second = 0;
			 new_node = 0;
		 }

		 edge(int n1, int n2){
			 init(n1, n2);
			 new_node = 0;
		 }

		 void init(int n1, int n2){
			 if(n1 < n2){
				 nodes.first = n1;
				 nodes.second = n2;
			 }
			 else{
				 nodes.first = n2;
				 nodes.second = n1;
			 }
		 }
		
	 };

};