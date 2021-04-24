#include "grid_gen_tet.h"


void grid_gen_tet::transform_cube_to_tetrahedron(string cube_file_elements, string cube_file_faces, string tet_file_elements, string tet_file_faces){
	FILE* inp_f = fopen(cube_file_elements.c_str(), "r");

	int cubes_n;
	vector<FE_cube> cubes; //кубы
	//Считываем элементы-кубы
	fscanf(inp_f, "%d", &cubes_n);

	FE_cube tmp_cube; //временный куб, куда будем считывать

	for(int i = 0; i < cubes_n; i++){
		for(int j = 0; j < 8; j++)
			fscanf(inp_f, "%d", &tmp_cube.node[j]);
		cubes.push_back(tmp_cube);
	}

	fclose(inp_f);

	vector<FE_tet> tets; //тетраэдры

	//Формируем тетраэдры
	for(int i = 0; i < cubes.size(); i++){
		FE_tet tmp_tet[6]; //из одного куба - шесть тетраэдров
		//1
		tmp_tet[0].node[0] = cubes[i].node[0];
		tmp_tet[0].node[1] = cubes[i].node[2];
		tmp_tet[0].node[2] = cubes[i].node[3];
		tmp_tet[0].node[3] = cubes[i].node[6];

		//2
		tmp_tet[1].node[0] = cubes[i].node[3];
		tmp_tet[1].node[1] = cubes[i].node[4];
		tmp_tet[1].node[2] = cubes[i].node[6];
		tmp_tet[1].node[3] = cubes[i].node[7];

		//3
		tmp_tet[2].node[0] = cubes[i].node[0];
		tmp_tet[2].node[1] = cubes[i].node[3];
		tmp_tet[2].node[2] = cubes[i].node[4];
		tmp_tet[2].node[3] = cubes[i].node[6];

		//4
		tmp_tet[3].node[0] = cubes[i].node[0];
		tmp_tet[3].node[1] = cubes[i].node[1];
		tmp_tet[3].node[2] = cubes[i].node[3];
		tmp_tet[3].node[3] = cubes[i].node[4];

		//5
		tmp_tet[4].node[0] = cubes[i].node[1];
		tmp_tet[4].node[1] = cubes[i].node[4];
		tmp_tet[4].node[2] = cubes[i].node[5];
		tmp_tet[4].node[3] = cubes[i].node[7];

		//6
		tmp_tet[5].node[0] = cubes[i].node[1];
		tmp_tet[5].node[1] = cubes[i].node[3];
		tmp_tet[5].node[2] = cubes[i].node[4];
		tmp_tet[5].node[3] = cubes[i].node[7];

		//Сформировали - заносим в вектор
		for(int j = 0; j < 6; j++)
			tets.push_back(tmp_tet[j]);

		if(i%1000 == 0)
			printf("%d from %d cubes done\n", i, cubes.size());
	}

	//Выводим тетраэдры
	FILE* out_f = fopen(tet_file_elements.c_str(), "w");
	fprintf(out_f, "%d\n", tets.size());
	for(int i = 0; i < tets.size(); i++){
		for(int j = 0 ; j < 4; j++)
			fprintf(out_f, "%d ", tets[i].node[j]);
		fprintf(out_f, "\n");
	}

	fclose(out_f);
	cubes.clear();

	//Теперь разбираемся с гранями
	inp_f = fopen(cube_file_faces.c_str(), "r");
	
	int rects_n; //количетво граней - прямоугольников
	vector<FE_tet> rects; //грани-прямоугольники
	FE_tet tmp_rect;

	fscanf(inp_f, "%d", &rects_n);
	for(int i = 0; i < rects_n; i++){
		for(int j = 0; j < 4; j++)
			fscanf(inp_f, "%d", &tmp_rect.node[j]);
		rects.push_back(tmp_rect);
	}
	fclose(inp_f);

/*	vector<FE_tri> triangles; //треугольники
	FE_tri tmp_tri;

	for(int i = 0; i < rects_n; i++){
		//Собираем треугольник
		tmp_tri.node[0] = rects[i].node[0];
		tmp_tri.node[1] = rects[i].node[1];
		tmp_tri.node[2] = rects[i].node[2];
		//Проверяес есть ли такой, если есть - добавляем
		if(find_tri_in_tets(tmp_tri, tets))
			triangles.push_back(tmp_tri);

		//И так для всех 4-х треугольников

		//Собираем треугольник
		tmp_tri.node[0] = rects[i].node[0];
		tmp_tri.node[1] = rects[i].node[1];
		tmp_tri.node[2] = rects[i].node[3];
		//Проверяес есть ли такой, если есть - добавляем
		if(find_tri_in_tets(tmp_tri, tets))
			triangles.push_back(tmp_tri);

		//Собираем треугольник
		tmp_tri.node[0] = rects[i].node[0];
		tmp_tri.node[1] = rects[i].node[2];
		tmp_tri.node[2] = rects[i].node[3];
		//Проверяес есть ли такой, если есть - добавляем
		if(find_tri_in_tets(tmp_tri, tets))
			triangles.push_back(tmp_tri);

		//Собираем треугольник
		tmp_tri.node[0] = rects[i].node[1];
		tmp_tri.node[1] = rects[i].node[2];
		tmp_tri.node[2] = rects[i].node[3];
		//Проверяес есть ли такой, если есть - добавляем
		if(find_tri_in_tets(tmp_tri, tets))
			triangles.push_back(tmp_tri);
	}

	//И вывод
	out_f = fopen(tet_file_faces.c_str(), "w");
	fprintf(out_f, "%d\n", triangles.size());

	for(int i = 0; i < triangles.size(); i++){
		for(int j = 0; j < 3; j++)
			fprintf(out_f, "%d ", triangles[i].node[j]);
		fprintf(out_f, "\n");
	}

	fclose(out_f);
	triangles.clear();
	rects.clear();*/

	tets.clear();
}

void grid_gen_tet::generate_face_nodes(string file_faces, string file_f_nodes){
	FILE* inp_f = fopen(file_faces.c_str(), "r");

	set<int> nodes; //наши узлы
	int faces_n; //количество граней
	fscanf(inp_f, "%d", &faces_n);
	int tmp_node;

	for(int i = 0; i < faces_n; i++){
		for(int j = 0; j < 3; j++){
			fscanf(inp_f, "%d", &tmp_node);
			nodes.insert(tmp_node);
		}
	}
	fclose(inp_f);

	FILE* out_f = fopen(file_f_nodes.c_str(), "w");
	fprintf(out_f, "%d\n", nodes.size());
	set<int>::iterator iter = nodes.begin();
	for(int i = 0; i < nodes.size(); i++){
		fprintf(out_f, "%d\n", *iter);
		iter++;
	}
	fclose(out_f);
}

bool grid_gen_tet::find_tri_in_tets(FE_tri &tri, vector<FE_tet> &tets){

	bool find = false; //нашли мы или нет
	int size = tets.size(); //чтобы кучу раз не вызывать функции
	//Для упрощения упорядочим вершины треугольника
	vector<int> sort_tri;
	for(int i = 0; i < 3; i++)
		sort_tri.push_back(tri.node[i]);

	sort(sort_tri.begin(), sort_tri.end());

	for(int i = 0; i < size && !find; i++){
		vector<int> tris[4]; //треугольники тетраэдра

		tris[0].push_back(tets[i].node[0]);
		tris[0].push_back(tets[i].node[1]);
		tris[0].push_back(tets[i].node[2]);

		tris[1].push_back(tets[i].node[0]);
		tris[1].push_back(tets[i].node[1]);
		tris[1].push_back(tets[i].node[3]);

		tris[2].push_back(tets[i].node[0]);
		tris[2].push_back(tets[i].node[2]);
		tris[2].push_back(tets[i].node[3]);

		tris[3].push_back(tets[i].node[1]);
		tris[3].push_back(tets[i].node[2]);
		tris[3].push_back(tets[i].node[3]);

		for(int j = 0; j < 4 && !find; j++){
			sort(tris[j].begin(), tris[j].end());
			if(tris[j] == sort_tri)
				find = true;
		}		
	}

	return find;
}

void grid_gen_tet::ins_grid(string file_elements, string file_faces, string file_nodes, string ins_file_elements, string ins_file_faces, string ins_file_nodes){
	FILE* inp_f = fopen(file_elements.c_str(), "r");

	int els_n; //кол-во элементов
	vector<FE_tet> tets; //исходная сетка
	FE_tet tmp_tet;

	fscanf(inp_f, "%d", &els_n);

	for(int i = 0; i < els_n; i++){
		for(int j = 0; j < 4; j++)
			fscanf(inp_f, "%d", &tmp_tet.node[j]);
		tets.push_back(tmp_tet);
	}
	fclose(inp_f);
	//Далее составляем множество всех ребёр
	set<edge> edges; //собственно, это множество ребёр
	edge tmp_edge;

	for(int i = 0; i < els_n; i++){
		//Берём составляем ребро и добавляем в сэт
		for(int j = 0; j < 3; j++){
			for(int k = j+1; k < 4; k++){
				tmp_edge = edge(tets[i].node[j], tets[i].node[k]);
				edges.insert(tmp_edge);
			}
		}
	}


	int nodes_n; //кол-во вершин

	FILE* nodes_f = fopen(file_nodes.c_str(), "r");
	fscanf(nodes_f, "%d", &nodes_n);

	int ins_nodes_n = nodes_n; //число узлов новой сетки

	//Устанавливаем новые узлы
	set<edge>::iterator iter = edges.begin();
	for(int i = 0; i < edges.size(); i++){
		(*iter).new_node = ins_nodes_n;
		ins_nodes_n++;
		iter++;
	}

	//Формируем новые элементы
	vector<FE_tet> ins_tets;

	for(int i = 0; i < tets.size(); i++){
		int new_nodes[6]; //новые вершины
		//Находим новые вершины
		for(int j = 0, s = 0; j < 3; j++){
			for(int k = j+1; k < 4; k++, s++){
				iter = edges.find(edge(tets[i].node[j], tets[i].node[k]));
				new_nodes[s] = (*iter).new_node;
			}
		}

		//Формируем новые тетраэдры и записываем их
		//1
		tmp_tet.node[0] = tets[i].node[0];
		tmp_tet.node[1] = new_nodes[1];
		tmp_tet.node[2] = new_nodes[2];
		tmp_tet.node[3] = new_nodes[0];
		ins_tets.push_back(tmp_tet);

		//2
		tmp_tet.node[0] = tets[i].node[2];
		tmp_tet.node[1] = new_nodes[1];
		tmp_tet.node[2] = new_nodes[5];
		tmp_tet.node[3] = new_nodes[3];
		ins_tets.push_back(tmp_tet);

		//3
		tmp_tet.node[0] = tets[i].node[1];
		tmp_tet.node[1] = new_nodes[3];
		tmp_tet.node[2] = new_nodes[4];
		tmp_tet.node[3] = new_nodes[0];
		ins_tets.push_back(tmp_tet);

		//4
		tmp_tet.node[0] = tets[i].node[3];
		tmp_tet.node[1] = new_nodes[2];
		tmp_tet.node[2] = new_nodes[4];
		tmp_tet.node[3] = new_nodes[5];
		ins_tets.push_back(tmp_tet);

		//5
		tmp_tet.node[0] = new_nodes[1];
		tmp_tet.node[1] = new_nodes[0];
		tmp_tet.node[2] = new_nodes[2];
		tmp_tet.node[3] = new_nodes[5];
		ins_tets.push_back(tmp_tet);

		//6
		tmp_tet.node[0] = new_nodes[1];
		tmp_tet.node[1] = new_nodes[0];
		tmp_tet.node[2] = new_nodes[3];
		tmp_tet.node[3] = new_nodes[5];
		ins_tets.push_back(tmp_tet);

		//7
		tmp_tet.node[0] = new_nodes[4];
		tmp_tet.node[1] = new_nodes[0];
		tmp_tet.node[2] = new_nodes[2];
		tmp_tet.node[3] = new_nodes[5];
		ins_tets.push_back(tmp_tet);

		//8
		tmp_tet.node[0] = new_nodes[4];
		tmp_tet.node[1] = new_nodes[0];
		tmp_tet.node[2] = new_nodes[3];
		tmp_tet.node[3] = new_nodes[5];
		ins_tets.push_back(tmp_tet);

	}

	FILE* out_f = fopen(ins_file_elements.c_str(), "w");
	fprintf(out_f, "%d\n", ins_tets.size());

	for(int i = 0; i < ins_tets.size(); i++){
		for(int j = 0; j < 4; j++)
			fprintf(out_f, "%d ", ins_tets[i].node[j]);
		fprintf(out_f, "\n");
	}

	fclose(out_f);

	ins_tets.clear();
	tets.clear();

	vector<FE_tri> triangles; //исходные рёбра
	FE_tri tmp_tri;
	inp_f = fopen(file_faces.c_str(), "r");
	
	int faces_n;
	fscanf(inp_f, "%d", &faces_n);
	for(int i = 0; i < faces_n; i++){
		for(int j = 0; j < 3; j++)
			fscanf(inp_f, "%d", &tmp_tri.node[j]);
		triangles.push_back(tmp_tri);
	}

	fclose(inp_f);

	vector<FE_tri> ins_triangles;

	//Делаем аналогично для треугольников
	for(int i = 0; i < faces_n; i++){
		int new_nodes[3];

		for(int j = 0, s = 0; j < 2; j++){
			for(int k = j+1; k < 3; k++, s++){
				iter = edges.find(edge(triangles[i].node[j], triangles[i].node[k]));
				new_nodes[s] = (*iter).new_node;
			}
		}
		
		//1
		tmp_tri.node[0] = triangles[i].node[0];
		tmp_tri.node[1] = new_nodes[0];
		tmp_tri.node[1] = new_nodes[1];
		ins_triangles.push_back(tmp_tri);

		//2
		tmp_tri.node[0] = triangles[i].node[1];
		tmp_tri.node[1] = new_nodes[0];
		tmp_tri.node[1] = new_nodes[2];
		ins_triangles.push_back(tmp_tri);

		//3
		tmp_tri.node[0] = triangles[i].node[2];
		tmp_tri.node[1] = new_nodes[1];
		tmp_tri.node[1] = new_nodes[2];
		ins_triangles.push_back(tmp_tri);

		//4
		tmp_tri.node[0] = new_nodes[0];
		tmp_tri.node[1] = new_nodes[1];
		tmp_tri.node[1] = new_nodes[2];
		ins_triangles.push_back(tmp_tri);

	}

	//И выводим наши новые грани
	out_f = fopen(ins_file_faces.c_str(), "w");
	fprintf(out_f, "%d\n", ins_triangles.size());

	for(int i = 0; i < ins_triangles.size(); i++){
		for(int j = 0; j < 3; j++)
			fprintf(out_f, "%d ", ins_triangles[i].node[j]);
		fprintf(out_f, "\n");
	}
	fclose(out_f);
	triangles.clear();
	ins_triangles.clear();

	//И наконец - пребразовываем узлы

	vector<gen_point> ins_nodes;
	gen_point tmp_nodes;

	for(int i = 0; i < nodes_n; i++){
		fscanf(nodes_f, "%lf %lf %lf", &tmp_nodes.x, &tmp_nodes.y, &tmp_nodes.z);
		ins_nodes.push_back(tmp_nodes);
	}

	fclose(nodes_f);

	//теперь добавим новые точки
	for(iter = edges.begin(); iter != edges.end(); iter++){

		int node1, node2;
		node1 = (*iter).nodes.first;
		node2 = (*iter).nodes.second;
		double x = (ins_nodes[node1].x + ins_nodes[node2].x) / 2.0;
		double y = (ins_nodes[node1].y + ins_nodes[node2].y) / 2.0;
		double z = (ins_nodes[node1].z + ins_nodes[node2].z) / 2.0;


		gen_point new_point(x, y, z);

		ins_nodes.push_back(new_point);
	}

	//И выводим

	out_f = fopen(ins_file_nodes.c_str(), "w");
	fprintf(out_f, "%d\n", ins_nodes_n);

	for(int i = 0; i < ins_nodes_n; i++)
		fprintf(out_f, "%.15lf\t%.15lf\t%.15lf\n", ins_nodes[i].x, ins_nodes[i].y, ins_nodes[i].z);
	
	fclose(out_f);

	//Урааа!

		

}