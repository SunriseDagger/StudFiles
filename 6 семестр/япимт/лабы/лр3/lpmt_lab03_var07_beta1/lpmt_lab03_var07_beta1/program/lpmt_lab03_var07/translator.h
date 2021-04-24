#ifndef TRANSLATOR_H_INCLUDED
#define TRANSLATOR_H_INCLUDED

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
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
    /** Синтаксический анализ    */
    // Определяем какая строка содержится в токене
    string get_token_text(token get_t);
    // Структура элемент таблицы разбора
    struct table_parse_elem
    {
        vector<string> terminal; // Терминалы
        int jump;                // Переход
        bool accept;             // Принимать или нет
        bool stack_;             // Класть в стек или нет
        bool return_;            // Возвращать или нет
        bool error;              // Может ли быть ошибка

    };
    // Таблица разбора
    vector<table_parse_elem> table_parse;
    // Структура элемент постфиксной записи
    struct postfix_elem
    {
        string id;
        short int type;
        postfix_elem()
        {
            id = "", type = 0;
        }
        postfix_elem(string id_, int type_)
        {
            id = id_, type = type_;
        }
        postfix_elem(string id_)
        {
            id = id_, type = 1;
        }
        friend ostream& operator << (ostream& ostream_, const postfix_elem& pe_)
        {
            ostream_ << pe_.id;
            return ostream_;
        }
    };
    // Сравнение приоритетов операций
    bool priority_le(string what, string with_what);
    // Постфиксная запись
    vector<postfix_elem> postfix_record;
    // Построение постфиксной записи
    bool make_postfix(vector<token> t);
public:
    // Конструктор со вводом постоянных таблиц
    translator();
    // Отладочный вывод таблиц
    void debug_print(ostream& stream);
    // Лексический анализ
    bool analyze_lexical(string file_source, string file_tokens, string file_error);
    // Синтаксический анализ
    bool analyze_syntactical(string file_tokens, string file_error);
    // Печать постфиксной записи в файл и на экран
    void postfix_print(string file_tree);
};

#endif // TRANSLATOR_H_INCLUDED
