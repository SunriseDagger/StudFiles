#include "additional_classes.h"

SLAE_port_list::SLAE_port_list(){
	init();
}

void SLAE_port_list::init(){
		begin = end = cash = 0;
		l_size = 0;
}

void SLAE_port_list::set_num(int s_num){
	number_of_line = s_num;

}


SLAE_port_list::~SLAE_port_list(){
	cash = 0;
	while(begin != 0)
		exclude_last_el();
}

void SLAE_port_list::add_el(FE &el_a){
	if(number_of_line > 4000)
		number_of_line = number_of_line;
	for(int i = 0; i < 16; i++)
		add(el_a.node_n[i]);
}

void SLAE_port_list::cash_off(){
	cash = begin;
}

int SLAE_port_list::take_and_next(){
	int cash_v = cash->value;
	cash = cash->next;
	return cash_v;
}

int SLAE_port_list::get_m_size(){
	return m_size;
}

void SLAE_port_list::add(int val){
	if(val <= number_of_line){
		gen_l_el *add_el;
		if(begin == 0){
			begin = new gen_l_el;
			begin->value  = val;
			begin->next = 0;
			end = begin;
			cash = begin;
		}
		else{
			if(val < begin->value){
				add_el = new gen_l_el;
				add_el->value = val;
				add_el->next = begin;
				begin = add_el;
				cash = begin;
			}
			else{
				if(val > end->value){
					add_el = new gen_l_el;
					add_el->value = val;
					add_el->next = 0;
					end->next = add_el;
					end = end->next;
				}
				else{
					cash = begin;
					while(cash->next != 0 && val > cash->next->value) cash = cash->next;
					if(cash->next != 0 && cash->next->value != val && cash->value != val){
						add_el = new gen_l_el;
						add_el->value = val;
						add_el->next = cash->next;
						cash->next = add_el;
					}
				}
			}
		}
		l_size++;
	}
}

void SLAE_port_list::exclude_last_el(){
	if(begin != end){
		cash = begin;
		while(cash->next != end) cash = cash->next;
		delete cash->next;
		cash->next = 0;
		end = cash;
	}
	else{
		delete begin;
		begin = end = cash = 0;
	}
}
int SLAE_port_list::size_before(int n){
	int tmp_s = 0;
	cash = begin;
	if(begin != 0){
		if(begin->value < n) tmp_s++;
		while(cash->next != 0 && cash->next->value < n){
			tmp_s++;
			cash = cash->next;
		}
	}
	m_size = tmp_s;
	return tmp_s;
}

SLAE_port_gen::SLAE_port_gen(){
	lists = new SLAE_port_list [0];
}

SLAE_port_gen::~SLAE_port_gen(){
	delete []lists;
	lists = 0;
}
void SLAE_port_gen::init(int nodes_n){
	n = nodes_n;
	lists = new SLAE_port_list [n];
	for(int i = 0 ; i < n; i++)
		lists[i].set_num(i);
}

void SLAE_port_gen::add_el(FE &el_a){
	for(int i = 0; i < 16; i++)
		lists[el_a.node_n[i]].add_el(el_a);
}

void SLAE_port_gen::gen(int *gi, int *&gj, int &m){
	m = 0;
	gi[0] = 0;
	for(int i = 0; i < n; i++){
		gi[i] = m;
		m += lists[i].size_before(i);
	}
	gi[n] = m;
	gj = new int [m];
	int shift, m1 = 0, iters_m;
	for(int i = 0; i < n; i++){
		shift = m1;
		iters_m = lists[i].get_m_size();
		lists[i].cash_off();
		for(int j = 0; j < iters_m; j++)
			gj[shift+j] = lists[i].take_and_next();
		m1 += iters_m;
	}
}