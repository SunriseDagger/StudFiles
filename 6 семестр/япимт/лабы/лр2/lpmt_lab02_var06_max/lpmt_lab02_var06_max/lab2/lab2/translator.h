#pragma once

#include "variable_table.h"

//Класс тарнслятора. Итог всех лаб и РГЗ


class translator{

 public:
	translator();
	//Констуркутор по умолчанию. Ввод постоянных таблиц.

	bool lex_analysis(string sorce_code_file, string tokens_file, string errors_file);
	//Фаза лексического анализа
	//sorce_code_file - имя файла с исходным кодом
	//tokens_file - имя файла токенов(будет генерироватся)
	//errors_file - имя файла для ошибок
	//Возврашает true, если фаза успешно завершенна

	void parse(string tokens_file, string errors_file); 
	//Фаза синтаксического анализа
	//tokens_file - имя файла токенов
	//errors_file - имя файла для ошибок

	//Струкутра, описывающая токен
	class token{
	 public:
		int table_n; //номер таблицы
		int chain_n; //номер цепочки, если таблица постоянная - -1
		int numb; //номер в таблице / в цепочке

		token();
		token(int t_n, int ch_n, int n); //конструктор по типу "сразу всё"

		friend ostream& operator << (ostream& os, token& out_t); //вывод токена
		friend istream& operator >> (istream& is, token& inp_t); //ввод токена
	};

 private:
	 //Постоянны таблицы
	 const_table<char> admis_chars; //таблица допустимых символов, используемых для имён (за исключением цифр), номер таблицы - 0
	 const_table<char> numbs; //таблица цифр, номер таблицы - 1
	 const_table<char> opers_chars; //таблица символов операций, номер таблицы - 7
	 const_table<string> opers; //таблица операций, номер таблицы - 2
	 const_table<string> key_words; //таблица ключевых слов, номер таблицы - 3
	 const_table<char> separaters; //таблица разделителей, номер таблицы - 4

	 //Переменные таблицы
	 variable_table identifier; //таблица индетификаторов, номер таблицы - 5
	 variable_table consts; //таблица констант, номер таблицы - 6

	 bool lex_string_process(string inp_str); //Лексическая обработка строки. Возврашает true, если ошибок нет, иначе - false

	 int check_symbol(char sym); //проверят допустим ли символ и возврашает:
	 // -1 - не допустим
	 // Если допустим, то вернёт номер таблицы

	ifstream lex_sorce_s; //Лексический анализ. Поток для работы с файлом исходного кода.
	ofstream token_s; //Лексический анализ. Поток для работы с файлом токенов.
	ofstream errors_s; //Лексический анализ. Поток для работы с файлом ошибок.



};