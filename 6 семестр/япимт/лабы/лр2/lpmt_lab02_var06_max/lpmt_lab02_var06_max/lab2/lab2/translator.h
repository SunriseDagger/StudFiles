#pragma once

#include "variable_table.h"

//����� �����������. ���� ���� ��� � ���


class translator{

 public:
	translator();
	//������������ �� ���������. ���� ���������� ������.

	bool lex_analysis(string sorce_code_file, string tokens_file, string errors_file);
	//���� ������������ �������
	//sorce_code_file - ��� ����� � �������� �����
	//tokens_file - ��� ����� �������(����� �������������)
	//errors_file - ��� ����� ��� ������
	//���������� true, ���� ���� ������� ����������

	void parse(string tokens_file, string errors_file); 
	//���� ��������������� �������
	//tokens_file - ��� ����� �������
	//errors_file - ��� ����� ��� ������

	//���������, ����������� �����
	class token{
	 public:
		int table_n; //����� �������
		int chain_n; //����� �������, ���� ������� ���������� - -1
		int numb; //����� � ������� / � �������

		token();
		token(int t_n, int ch_n, int n); //����������� �� ���� "����� ��"

		friend ostream& operator << (ostream& os, token& out_t); //����� ������
		friend istream& operator >> (istream& is, token& inp_t); //���� ������
	};

 private:
	 //��������� �������
	 const_table<char> admis_chars; //������� ���������� ��������, ������������ ��� ��� (�� ����������� ����), ����� ������� - 0
	 const_table<char> numbs; //������� ����, ����� ������� - 1
	 const_table<char> opers_chars; //������� �������� ��������, ����� ������� - 7
	 const_table<string> opers; //������� ��������, ����� ������� - 2
	 const_table<string> key_words; //������� �������� ����, ����� ������� - 3
	 const_table<char> separaters; //������� ������������, ����� ������� - 4

	 //���������� �������
	 variable_table identifier; //������� ���������������, ����� ������� - 5
	 variable_table consts; //������� ��������, ����� ������� - 6

	 bool lex_string_process(string inp_str); //����������� ��������� ������. ���������� true, ���� ������ ���, ����� - false

	 int check_symbol(char sym); //�������� �������� �� ������ � ����������:
	 // -1 - �� ��������
	 // ���� ��������, �� ����� ����� �������

	ifstream lex_sorce_s; //����������� ������. ����� ��� ������ � ������ ��������� ����.
	ofstream token_s; //����������� ������. ����� ��� ������ � ������ �������.
	ofstream errors_s; //����������� ������. ����� ��� ������ � ������ ������.



};