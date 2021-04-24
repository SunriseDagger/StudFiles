#pragma once

#include <math.h>

//узел
struct node{
	double x, y, z;
	int number;
};

//Тетраэдр
struct FE{
	int node_n[4];
	int number;
	int area;
};

//Грань
struct face{
	int node_n[3];
	int number;
	int area;
};


//элемент списка для геренации портрета СЛАУ
struct gen_l_el{
	int value;
	gen_l_el* next;
};

class SLAE_port_list{
 public:

	 SLAE_port_list();
	~SLAE_port_list();
	 void add_el(FE &el_a);
	 void init();
	 void set_num(int s_num);

	 int size();
	 int size_before(int n);
	 int take_and_next();
	 void cash_off();
	 int get_m_size();
	 
 private:
	 void exclude_last_el();
	 void add(int val);
	 gen_l_el *begin, *end, *cash;
	 int l_size;
	 int m_size;
	 int number_of_line;
};


class SLAE_port_gen{
 public:
	 SLAE_port_gen();
	 ~SLAE_port_gen();
	 void init(int nodes_n);

	 void add_el(FE& el_a);
	 void gen(int *gi, int *&gj, int &m);
private:
	int n;
	SLAE_port_list* lists;
};
