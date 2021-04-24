#include "translator.h"

/** ================ Общие функции собственно транслятора ================ */

// Конструктор со вводом постоянных таблиц и таблицы разбора
translator::translator()
{
    letters.read_file("files/table_letters.txt");
    numbers.read_file("files/table_numbers.txt");
    operations.read_file("files/table_operations.txt");
    keywords.read_file("files/table_keywords.txt");
    separators.read_file("files/table_separators.txt");
    ifstream in_table_parse;
    in_table_parse.open("files/table_parse.txt", ios::in);
    string str;
    getline(in_table_parse, str, '\n');
    struct table_parse_elem te;
    te.jump = 1;
    te.accept = false;
    te.stack_ = true;
    te.return_ = false;
    te.error = true;
    table_parse.push_back(te);
    while(!in_table_parse.eof())
    {
        struct table_parse_elem te;
        string str;
        in_table_parse >> str;
        if(in_table_parse.eof())
            break;
        stringstream a;
        str = "";
        while(str.length() == 0 || str.find("\t") != string::npos)
            getline(in_table_parse, str, '\t');
        a.str(str);
        while(a.good())
        {
            a >> str;
            te.terminal.push_back(str);
        }
        in_table_parse >> te.jump >> te.accept >> te.stack_ >> te.return_ >> te.error;
        table_parse.push_back(te);
    }
    table_parse[0].terminal.resize(table_parse[1].terminal.size());
    for(int i = 0; i < (int)table_parse[1].terminal.size(); i++)
        table_parse[0].terminal[i] = table_parse[1].terminal[i];
    in_table_parse.close();
}

// Отладочный вывод таблиц
void translator::debug_print(ostream& stream)
{
    stream << "ID`s:" << endl;
    identifiers.debug_print(stream);
    stream << "CONST`s:" << endl;
    constants.debug_print(stream);
}

