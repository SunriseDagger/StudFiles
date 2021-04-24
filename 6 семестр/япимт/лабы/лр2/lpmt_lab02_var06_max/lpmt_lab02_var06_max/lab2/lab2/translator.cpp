#include "translator.h"

translator::translator(){
	admis_chars.generate_from_file("table_chars.txt");
	numbs.generate_from_file("table_numbs.txt");
	opers_chars.generate_from_file("table_opers_chars.txt");
	opers.generate_from_file("table_opers.txt");
	key_words.generate_from_file("table_key_words.txt");
	separaters.generate_from_file("table_seps.txt");
}

translator::token::token(){
}

translator::token::token(int t_n, int ch_n, int n){
	table_n = t_n;
	chain_n = ch_n;
	numb = n;
}

ostream& operator << (ostream& os, translator::token& out_t){
	os << out_t.table_n << " " << out_t.chain_n << " " << out_t.numb << endl;
	return os;
}

istream& operator >> (istream& is, translator::token& inp_t){
	is >> inp_t.table_n >> inp_t.chain_n >> inp_t.numb;
	return is;
}

bool translator::lex_analysis(string sorce_code_file, string tokens_file, string errors_file){

	lex_sorce_s.open(sorce_code_file.c_str(), ios::in); //����� ��� ������ � ������ ��������� ����
	token_s.open(tokens_file.c_str(), ios::out); //����� ��� ������ � ������ �������
	errors_s.open(errors_file.c_str(), ios::out); //����� ��� ������ � ������ ������

	string inp_str; //������� ������
	bool has_error = false;

	while(!lex_sorce_s.eof() && !has_error){
		lex_sorce_s >> inp_str;
		has_error = !lex_string_process(inp_str);

	};


	lex_sorce_s.close();
	token_s.close();
	errors_s.close();

	return !has_error;
}

bool translator::lex_string_process(string inp_str){
	if(inp_str.size() != 0){
		bool has_error = false;
		int first_sym_type = check_symbol(inp_str[0]);
		bool local_error = false; //������ �� � ������ �������
		string continue_str;
		string basic_inp = inp_str;
		switch(first_sym_type){

			case -1:{ //� ������ ������������� �������
				errors_s << "� " << inp_str << " ����������� ������������ �������. ������ ������ ����������." << endl;
				has_error = true;

				cout << "We have a error!" << endl;
					}break;

			case 0:{ //���� � ������� ������� ����� ��������� ��������������
				int i;
				//��������� ���� �� ���� ����� �������������
				for(i = 1; i < inp_str.size() && !local_error; i++){
					local_error = !(admis_chars.is_element_in_table(inp_str[i]) || numbs.is_element_in_table(inp_str[i]));
				}
				int start = i;
				if(local_error)
					start--;
				continue_str = inp_str.substr(start);
				inp_str = inp_str.substr(0, start);

				//���� �� ������, �� �������� �������� ��� ��� ��� ���
				//���� �� �������� ����� � ����� ������, ���� ��� - ������� � �������������� � ����� ������)
				if(key_words.is_element_in_table(inp_str)){
					int n;
					key_words.get_elemen_number(inp_str, n);
					token_s << token(3, -1, n);
				}
				else{
					identifier.add_ind(inp_str);
					int ch_n, n;
					identifier.ind_adrress(inp_str, ch_n, n);
					token_s << token(5, ch_n, n);
				}
				local_error = !lex_string_process(continue_str);

				   }break;

			case 1:{ //���� ������ ������ - ���������
				int i;
				//��������� ��������� �� ���
				for(i = 1; i < inp_str.size() && !local_error; i++){
					local_error = !(numbs.is_element_in_table(inp_str[i]) || inp_str[i] == '.'); //��� ���� �����, ���� �����
				}
				int start = i;
				if(local_error)
					start--;
				local_error = false;
				continue_str = inp_str.substr(start);
				if(continue_str.size() > 0){
					if(admis_chars.is_element_in_table(continue_str[0])){
						local_error = true;
						errors_s << "����� ����� - �����, ��-��!" << endl;
					}
				}
				inp_str = inp_str.substr(0, start);

				int points_n = inp_str.find_last_of('.') - inp_str.find_first_of('.'); //���������� ��������� �����

				//� ��� ����� ���� ������ ���� �����
				if(points_n != 0)
					local_error = true;

				//���� ��� ������, �� ��������� ������� �������� � ������ �����
				if(!local_error){
					consts.add_ind(inp_str);
					int ch_n, n;
					consts.ind_adrress(inp_str, ch_n, n);
					token_s << token(6, ch_n, n);
					local_error = !lex_string_process(continue_str);
				}			

				   }break;

			case 7:{ //���� ������ ������ ��� ������ ��������
				if(inp_str[0] != '/'){
					int i;
					string oper_s = inp_str.substr(0, 2);
					int oper_l;
					//�������� ����� ������ ���� ������������������
					local_error = !opers.is_element_in_table(oper_s);
					//������ - ��������������
					if(local_error){
						oper_s = inp_str.substr(0,1);
						local_error = !opers.is_element_in_table(oper_s);
						oper_l = 1;
					}
					else{
						if(inp_str.size() == 1)
							oper_l = 1;
						else
							oper_l = 2;
					}
					//���� �� ������ - ������ �����
					if(!local_error){
						int n;
						opers.get_elemen_number(oper_s, n);
						token_s << token(2, -1, n);
						continue_str = inp_str.substr(oper_l, inp_str.size()-oper_l);
						local_error = !lex_string_process(continue_str);
					}
				}
				else{ //���� ���� ���������� �� �����������
					if(inp_str.size() > 1){
						switch(inp_str[1]){
						case '*':{
							string garb; //���� ����� ������ ������
							bool comment_end = false;
							while(!comment_end && !lex_sorce_s.eof()){
								lex_sorce_s >> garb;
								int st = garb.find("*/");
								if(st != -1)
									comment_end = true;
							}
							if(!lex_sorce_s.eof()){
								int st1 = garb.find_first_of("*/");
								continue_str = garb.substr(st1+2);
								local_error = !lex_string_process(continue_str);
							}
							else{
								local_error = true;
								errors_s << "���������� ����������" << endl;
							}
								 }break;
						case '/':{
							string garb;
							getline(lex_sorce_s, garb);
								 }break;
						default:{
							errors_s << "������, ������ / �� ���������� �����������" << endl;
							local_error = true;
						  }break;

					};
					}
					else{
						errors_s << "������, ������ / �� ���������� �����������" << endl;
						local_error = true;
					}
				}

				   }break;
			case 4 :{ // ���� �����������

				continue_str = inp_str.substr(1);
				int n;
				separaters.get_elemen_number(inp_str[0],n);
				token_s << token(4, -1, n);
				local_error = !lex_string_process(continue_str);

					}break;
			default: {
				errors_s << "������������ ���������� �������������� ������� �������" << endl;
				has_error = true;

				cout << "����������� ������" << endl;
					 }break;

		};
		if(local_error){
			cout << "We have a error!" << endl;
			errors_s << "������ � " << basic_inp << " , ������������ �������" << endl;
		}
		return !(has_error || local_error);
	}
	else
		return true;
}

int translator::check_symbol(char sym){
	if(admis_chars.is_element_in_table(sym))
		return 0;
	if(numbs.is_element_in_table(sym) || sym == '-')
		return 1;
	if(opers_chars.is_element_in_table(sym))
		return 7;
	if(separaters.is_element_in_table(sym))
		return 4;
	return -1;
}