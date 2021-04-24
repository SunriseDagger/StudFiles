#pragma once

#include "grid_gen_cube.h"
#include <set>
#include <algorithm>

//Генератор трёхметрых тетраэдальных сеток, с ориентацией на МКЭ

class grid_gen_tet{
 public:
	 //===================Интерфейс класса генерации сетки===================
	static void transform_cube_to_tetrahedron(string cube_file_elements, string cube_file_faces, string tet_file_elements, string tet_file_faces); //Пребразование кубической сетки в тетраэдальную
	//cube_file_elements - файл элементов исходной сетки
	//cube_file_faces - файл боковых граней исходной сетки
	//tet_file_elements - файл элементов полученной сетки
	//tet_file_faces - файл боковых граней полученной сетки

	static void ins_grid(string file_elements, string file_faces, string file_nodes, string ins_file_elements, string ins_file_faces, string ins_file_nodes); //построение вложенной сетки
	//Лень писать, но если прочитать названия переменных понятно что есть что. Сначало старая сетка, затем - новая.

	
	 static void generate_face_nodes(string file_faces, string file_f_nodes); //выделяет узлы на гранях
	 //file_faces - файл ребёр
	 //file_f_nodes - итоговый файл с номерами узлов

 private:
	 //Вспомогалтельные структуры:
	 //КЭ - куб
	 struct FE_cube{
		int node[8];
	 };
	 //КЭ - тетраэдр и за одно прямоугольник
	 struct FE_tet{
		int node[4];
	 };
	 //КЭ - треугольник
	 struct FE_tri{
		int node[3];
	 };

	 static bool find_tri_in_tets(FE_tri& tri, vector<FE_tet>& tets); //Вспомогательная функци проверяет есть ли такой треугольник среди тетраэдров

	 //Вспомогательные структуры для построение вложенной сетки
	 struct edge{ //ребро
		 pair<int, int> nodes; //исходные узлы ребра
		 int new_node; //новый узел
		 
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