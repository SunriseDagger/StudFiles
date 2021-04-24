#ifndef TRANSLATOR_H_INCLUDED
#define TRANSLATOR_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <string>
#include "table_const.h"
#include "table_var.h"
#include "lexeme.h"
#include "token.h"

using namespace std;

class translator
{
private:
    // Постоянные таблицы
    table_const<char> letters;      // 0
    table_const<char> numbers;      // 1
    table_const<string> operations; // 2
    table_const<string> keywords;   // 3
    table_const<char> separators;   // 4
    // Переменные таблицы
    table_var identifiers;          // 5
    table_var constants;            // 6
    // Файловые потоки
    ifstream in_source;
    ofstream out_token;
    ofstream out_error;
    // Анализ строки
    bool analyze_lexical_string(string str);
    // Удаление комментариев
    bool analyze_lexical_decomment(string& str, bool is_changed);
    // Счетчики для подробных сообщений об ошибке
    int analyze_lexical_strnum, analyze_lexical_strinc;
    // Удаление пробелов
    static inline void ltrim(string& out_)
    {
        int notwhite = out_.find_first_not_of(" \t\n");
        out_.erase(0, notwhite);
    }
    static inline void rtrim(string& out_)
    {
        int notwhite = out_.find_last_not_of(" \t\n");
        out_.erase(notwhite + 1);
    }
    static inline void trim(string& out_)
    {
        ltrim(out_);
        rtrim(out_);
    }
public:
    // Конструктор со вводом постоянных таблиц
    translator();
    // Лексический анализ
    bool analyze_lexical(string file_source, string file_tokens, string file_error);
    // Отладочный вывод таблиц
    void debug_print(ostream& stream);
};

#endif // TRANSLATOR_H_INCLUDED
