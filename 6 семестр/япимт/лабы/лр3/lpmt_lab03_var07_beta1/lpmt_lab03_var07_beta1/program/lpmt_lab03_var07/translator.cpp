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

/** ================ Функции лексического анализатора ================ */

// Лексический анализ
bool translator::analyze_lexical(string file_source, string file_tokens, string file_error)
{
    in_source.open(file_source.c_str(), ios::in);
    out_token.open(file_tokens.c_str(), ios::out);
    out_error.open(file_error.c_str(), ios::out);
    bool flag_error = false;
    string str;
    analyze_lexical_strnum = 1;
    while(!in_source.eof() && !flag_error)
    {
        getline(in_source, str);
        if(!in_source.eof())
        {
            analyze_lexical_strinc = 0;
            string strold = str;
            if(!analyze_lexical_decomment(str, true))
            {
                out_error << "Lexic Error: Error in string " << analyze_lexical_strnum << ": " << strold << endl;
                cerr << "Lexic Error: Error in string " << analyze_lexical_strnum << ": " << strold << endl;
                return false;
            }
            analyze_lexical_strnum += analyze_lexical_strinc;
            flag_error = !analyze_lexical_string(str);
            if(flag_error)
            {
                out_error << "Lexic Error: Error in string " << analyze_lexical_strnum << ": " << str << endl;
                cerr << "Lexic Error: Error in string " << analyze_lexical_strnum << ": " << str << endl;
            }
            analyze_lexical_strnum ++;
        }
    }
    in_source.close();
    out_token.close();
    out_error.close();
    return !flag_error;
}

// Очистка от комментариев
bool translator::analyze_lexical_decomment(string& str, bool is_changed)
{
    if(str.size())
    {
        bool change = false;
        size_t index_c = str.find("//"), index_c1 = str.find("/*"), index_c2;
        if (index_c != string::npos && index_c < index_c1)
        {
            str.erase(index_c);
            change = true;
        }
        index_c1 = str.find("/*");
        index_c2 = str.find("*/");
        if(index_c2 < index_c1)
        {
            out_error << "Lexic Error: Incorrect coment" << endl;
            cerr << "Lexic Error: Incorrect coment" << endl;
            return false;
        }
        while(index_c1 != string::npos && index_c2 != string::npos)
        {
            string tmpstr = str;
            str.erase(index_c1);
            tmpstr.erase(0, index_c2 + 2);
            str += tmpstr;
            index_c1 = str.find("/*");
            index_c2 = str.find("*/");
            change = true;
        }
        index_c1 = str.find("/*");
        index_c2 = str.find("*/");
        if(index_c1 != string::npos && index_c2 == string::npos)
        {
            str.erase(index_c1);
            string tmpstr;
            if(!in_source.eof())
            {
                getline(in_source, tmpstr);
                analyze_lexical_strinc++;
            }
            else
            {
                out_error << "Lexic Error: Incorrect coment" << endl;
                cerr << "Lexic Error: Incorrect coment" << endl;
                return false;
            }
            while(tmpstr.find("*/") == string::npos)
            {
                if(!in_source.eof())
                {
                    getline(in_source, tmpstr);
                    analyze_lexical_strinc++;
                }
                else
                {
                    out_error << "Lexic Error: Incorrect coment" << endl;
                    cerr << "Lexic Error: Incorrect coment" << endl;
                    return false;
                }
            }
            index_c2 = tmpstr.find("*/");
            tmpstr.erase(0, index_c2 + 2);
            str += " " + tmpstr;
            change = true;
        }
        index_c1 = str.find("/*");
        index_c2 = str.find("*/");
        if((index_c1 != string::npos && index_c2 == string::npos) ||
                (index_c1 == string::npos && index_c2 != string::npos))
        {
            out_error << "Lexic Error: Incorrect coment" << endl;
            cerr << "Lexic Error: Incorrect coment" << endl;
            return false;
        }
        if(is_changed)
            return analyze_lexical_decomment(str, change);
    }
    return true;
}

// Анализ строки
bool translator::analyze_lexical_string(string str)
{
    trim(str);
    bool flag_error = false;
    if(str.size())
    {
        //cout << "analyze_lexical_string started with str = "<< str << endl;
        char sym_1 = str[0], sym_2 = str[1];
        // Проверка первого символа
        string str_1, str_2;
        stringstream str_stream;
        str_stream << sym_1;
        str_1 = str_stream.str();
        str_stream << sym_2;
        str_2 = str_stream.str();
        int first_sym_type = -1;
        if(letters.contains(sym_1))
            first_sym_type = 0;
        if(numbers.contains(sym_1) || sym_1 == '-')
            first_sym_type = 1;
        if(operations.contains(str_1) || operations.contains(str_2))
            first_sym_type = 2;
        if(separators.contains(sym_1))
            first_sym_type = 3;

        switch(first_sym_type)
        {
            /*case -1: // Недопустимый символ
            {
                out_error << "Lexic Error: Unresolved first symbol" << endl;
                cerr << "Lexic Error: Unresolved first symbol" << endl;
                return false;
            }
            break;*/
        case 0: // Идентификатор
        {
            // Получим полное название идентификатора
            string idname = str;
            int i;
            bool finded = false;
            for(i = 1; i < (int)idname.size() && !finded; i++)
                finded = !(letters.contains(str[i]) || numbers.contains(str[i]));
            if(finded)
            {
                idname.erase(i - 1);
                str.erase(0, i - 1);
            }
            else
                str.erase(0);
            trim(idname);
            trim(str);
            if(keywords.contains(idname)) // Если ключевое слово
            {
                if(keywords.get_num(idname, i))
                    out_token << token(3, i, -1);
            }
            else // Иначе в таблицу идентификаторов
            {
                identifiers.add(idname);
                int table, chain;
                identifiers.get_location(idname, table, chain);
                out_token << token(5, table, chain);
            }
            return analyze_lexical_string(str);
        }
        break;
        case 1:  // Константа
        {
            string constval = str;
            int i;
            bool finded = false;
            for(i = 1; i < (int)constval.size() && !finded; i++)
                finded = !(numbers.contains(str[i]) || str[i] == '.' || str[i] == ' ');
            string str_t1, str_t2;
            stringstream str_stream_t;
            str_stream_t << str[i - 1];
            str_t1 = str_stream_t.str();
            str_stream_t << str[i];
            str_t2 = str_stream_t.str();
            if(!operations.contains(str_t1) && !operations.contains(str_t2) && !separators.contains(str[i - 1]))
            {
                out_error << "Lexic Error: Incorrect constant" << endl;
                cerr << "Lexic Error: Incorrect constant" << endl;
                return false;
            }
            if(finded)
            {
                constval.erase(i - 1);
                str.erase(0, i - 1);
            }
            else
                str.erase(0);
            trim(constval);
            trim(str);
            if(constval.find_last_of('.') - constval.find_first_of('.') != 0)
            {
                out_error << "Lexic Error: Incorrect constant" << endl;
                cerr << "Lexic Error: Incorrect constant" << endl;
                return false;
            }
            else
            {
                constants.add(constval);
                int table=-100, chain=-100;
                constants.get_location(constval, table, chain);
                out_token << token(6, table, chain);
            }
            return analyze_lexical_string(str);
        }
        break;
        case 2: // Операция
        {
            int table;
            if(operations.contains(str_2)) // Двухсимвольная
            {
                operations.get_num(str_2, table);
                out_token << token(2, table, -1);
                str.erase(0, 2);
                trim(str);
                return analyze_lexical_string(str);
            }
            if(operations.contains(str_1)) // Односимвольная
            {
                operations.get_num(str_1, table);
                out_token << token(2, table, -1);
                str.erase(0, 1);
                trim(str);
                return analyze_lexical_string(str);
            }
        }
        break;
        case 3: // Разделитель
        {
            int table;
            separators.get_num((const char)str[0], table);
            out_token << token(4, table, -1);
            str.erase(0, 1);
            trim(str);
            return analyze_lexical_string(str);
        }
        break;
        default: // Непонятно что
        {
            out_error << "Lexic Error: Can`t determine symbol \"" << str_1 << "\"" << endl;
            cerr << "Lexic Error: Can`t determine symbol \"" << str_1 << "\"" << endl;
            return false;
        }
        break;
        }
    }
    return !flag_error;
}

/** ================ Функции синтаксического анализатора ================ */

// Получение строки, на которую указывает токен
string translator::get_token_text(token t)
{
    string str = "";
    char sym = '\0';
    lexeme l("");
    switch(t.table)
    {
    case 2:
        operations.get_val(t.place, str);
        return str;
    case 3:
        keywords.get_val(t.place, str);
        return str;
    case 4:
        separators.get_val(t.place, sym);
        str.append(&sym, 1);
        return str;
    case 5:
        identifiers.get_lexeme(t.place, t.chain, l);
        return l.name;
    case 6:
        constants.get_lexeme(t.place, t.chain, l);
        return l.name;
    }
    return str;
}

// Синтаксический анализатор
bool translator::analyze_syntactical(string tokens_file, string errors_file)
{
    ifstream in_token(tokens_file.c_str(), ios::in);
    out_error.open(errors_file.c_str(), ios::out);
    token curr_token, next_token;
    stack<int> parse_stack;
    bool error_flag = false;
    int curr_row = 0;
    bool have_type = false; // Находимся ли мы в строке с объявлением типа
    int type_type;          // Если находимся, то какой тип объявляем
    bool need_postfix = false;      // Нужно ли выполнять построение постфиксной записи для данной строки
    vector<token> code_expr_infix;  // Если да, то сюда помещаем токены в инфиксном (обычном) порядке
    bool need_array_resize = false;         // Объявляем ли мы сейчас размер массива
    vector<token> array_resize_expr_infix;  // Если да, то сюда помещаем токены в инфиксном (обычном) порядке
    bool eof_flag = in_token.eof();    // Флаг конца файла (чтобы считать последний токен)

    in_token >> curr_token >> next_token;
    while(!eof_flag && !error_flag)
    {
        string token_str = get_token_text(curr_token);
        trim(token_str);

        if(curr_token.table == 5) token_str = "var";
        if(curr_token.table == 6) token_str = "const";

        // Ищем терминалы из списка
        bool find_terminal = false;
        cout << "Curr Row = " << curr_row << endl;
        cout << "Token: " << curr_token;
        cout << "Token String: " << token_str << endl;
        for(int i = 0; i < (int)table_parse[curr_row].terminal.size() && !find_terminal; i++)
        {
            cout << "Scan " << table_parse[curr_row].terminal[i] << " : ";
            if(table_parse[curr_row].terminal[i] == token_str)
                find_terminal = true;
            cout << find_terminal << endl;
        }

        // Если нашли
        if(find_terminal)
        {
            if(table_parse[curr_row].stack_)
                parse_stack.push(curr_row + 1);

            if(table_parse[curr_row].accept)
            {
                if((token_str == "var" || token_str == "const") &&
                                (get_token_text(next_token) == "=" ||
                                (get_token_text(next_token) == "[" && !have_type)))
                    need_postfix = true;

                if((token_str == "var" || token_str == "const") && have_type && get_token_text(next_token) == "[")
                    need_array_resize = true;

                // Обработка необъявленного типа
                if(!have_type && token_str == "var")
                {
                    lexeme lex_var;
                    identifiers.get_lexeme(curr_token.place, curr_token.chain, lex_var);
                    if(lex_var.type == 0)
                    {
                        error_flag = true;
                        out_error << "Syntax Error: Undefined identifier \"" << lex_var.name << "\"" << endl;
                        cerr << "Syntax Error: Undefined identifier \"" << lex_var.name << "\"" << endl;
                    }
                }

                // Обработка унарного минуса
                bool flag_unary_minus = false;
                if(curr_row == 54 && need_postfix)
                {
                    int one_hash, one_chain;
                    constants.add("-1");
                    constants.get_location("-1", one_hash, one_chain);
                    code_expr_infix.push_back(token(6, one_hash, one_chain));
                    int mult_pos;
                    operations.get_num("*", mult_pos);
                    code_expr_infix.push_back(token(2, mult_pos, -1));
                    flag_unary_minus = true;
                }

                if(need_postfix && !flag_unary_minus)
                    code_expr_infix.push_back(curr_token);

                // Обработка унарного минуса
                flag_unary_minus = false;
                if(curr_row == 54 && need_array_resize)
                {
                    int one_hash, one_chain;
                    constants.add("-1");
                    constants.get_location("-1", one_hash, one_chain);
                    array_resize_expr_infix.push_back(token(6, one_hash, one_chain));
                    int mult_pos;
                    operations.get_num("*", mult_pos);
                    array_resize_expr_infix.push_back(token(2, mult_pos, -1));
                    flag_unary_minus = true;
                }

                if(need_array_resize && !flag_unary_minus)
                {
                    array_resize_expr_infix.push_back(curr_token);
                    if(token_str == "=" || token_str == "+=" || token_str == "-=" || token_str == "*=")
                    {
                        error_flag = true;
                        out_error << "Syntax Error: Can`t assign to array \"" << get_token_text(array_resize_expr_infix[0]) << "\"" << endl;
                        cerr << "Syntax Error: Can`t assign to array \"" << get_token_text(array_resize_expr_infix[0]) << "\"" << endl;
                    }
                }

                // Если закончили разбор присваивания или части объявления
                if(token_str == ";" || token_str == ",")
                {
                    // Добавим все, что разобрали, в постфиксную запись
                    if(!make_postfix(code_expr_infix))
                        error_flag = true;
                    if(need_array_resize && !error_flag)
                    {
                        if(!make_postfix(array_resize_expr_infix))
                            error_flag = true;
                    }
                    // Сбрасываем все флаги
                    code_expr_infix.clear();
                    array_resize_expr_infix.clear();
                    need_postfix = false;
                    need_array_resize = false;
                }

                // Если закончили разбор объявления, сбросим флаг объявления
                if(token_str == ";")
                    have_type = false;

                // Если попался тип, запоминаем его
                if(token_str == "int" || token_str == "float")
                {
                    have_type = true;
                    if(token_str == "int")
                        type_type = 1;
                    if(token_str == "float")
                        type_type = 2;
                }

                // Заносим тип в таблицу идентификаторов
                if(token_str == "var" && have_type && curr_row == 69)
                    identifiers.set_type(get_token_text(curr_token), type_type);

                eof_flag = in_token.eof();
                curr_token = next_token;
                if(!eof_flag)
                    in_token >> next_token;
            }

            if(table_parse[curr_row].return_)
            {
                if(!parse_stack.empty())
                {
                    curr_row = parse_stack.top();
                    parse_stack.pop();
                }
                else // Если внезапно стек пуст
                {
                    error_flag = true;
                    cerr << "Syntax Error: Parse stack is empty!" << endl;
                    cerr << "Return requested by row " << curr_row << " at token " << curr_token
                         << " (value = \"" << get_token_text(curr_token) << "\")" << endl;
                    out_error << "Syntax Error: Parse stack is empty!" << endl;
                    out_error << "Return requested by row " << curr_row << " at token " << curr_token
                         << " (value = \"" << get_token_text(curr_token) << "\")" << endl;
                }
            }
            else
                curr_row = table_parse[curr_row].jump;
        }
        else
        {
            // Если ошибка безальтернативная
            if(table_parse[curr_row].error)
            {
                error_flag = true;
                out_error << "Syntax Error: Unexpected terminal \"" << get_token_text(curr_token) << "\"" << endl;
                out_error << "Must be: ";
                for(int i = 0; i < (int)table_parse[curr_row].terminal.size(); i++)
                    out_error << "\"" << table_parse[curr_row].terminal[i] << "\" ";
                out_error << endl;
                cerr << "Syntax Error: Unexpected terminal \"" << get_token_text(curr_token) << "\"" << endl;
                cerr << "Must be: ";
                for(int i = 0; i < (int)table_parse[curr_row].terminal.size(); i++)
                    cerr << "\"" << table_parse[curr_row].terminal[i] << "\" ";
                cerr << endl;
            }
            else
            {
                curr_row++;
            }
        }
    };

    // Если внезапно стек не пуст
    if(!error_flag && !parse_stack.empty())
    {
        error_flag = true;
        cerr << "Syntax Error: Parse stack isn`t empty!" << endl;
        cerr << "Size = " << parse_stack.size() << endl;
        cerr << "Contains: ";
        out_error << "Syntax Error: Parse stack isn`t empty!" << endl;
        out_error << "Size = " << parse_stack.size() << endl;
        out_error << "Contains: ";
        while(!parse_stack.empty())
        {
            cerr << "\"" << parse_stack.top() << "\" " << endl;
            out_error << "\"" << parse_stack.top() << "\" " << endl;
            parse_stack.pop();
        }
        cerr << endl;
        out_error << endl;
    }

    in_token.close();
    out_error.close();
    return !error_flag;
}

// Построение постфиксной записи
bool translator::make_postfix(vector<token> t)
{
    stack<string> stack_temp;
    bool error_flag = false;
    int index = 0;
    while(index < (int)t.size() && !error_flag)
    {
        int i;
        for(i = index; i < (int)t.size() && !error_flag && get_token_text(t[i]) != ";" && get_token_text(t[i]) != ","; i++)
        {
            string token_text = get_token_text(t[i]);
            if(t[i].table == 5 || t[i].table == 6)
            {
                postfix_record.push_back(postfix_elem(token_text));
            }
            else if(token_text == "(" || token_text == "[")
            {
                stack_temp.push(token_text);
            }
            else if(token_text == ")")
            {
                while(!stack_temp.empty() && stack_temp.top() != "(")
                {
                    string tmpstr = stack_temp.top();
                    postfix_record.push_back(postfix_elem(tmpstr));
                    stack_temp.pop();
                }
                if(stack_temp.empty())
                {
                    cerr << "Syntax Error: Unexpected \")\" !" << endl;
                    out_error << "Syntax Error: Unexpected \")\" !" << endl;
                    error_flag = true;
                }
                else
                {
                    stack_temp.pop();
                }
            }
            else if(token_text == "]")
            {
                while(!stack_temp.empty() && stack_temp.top() != "[")
                {
                    string tmpstr = stack_temp.top();
                    postfix_record.push_back(postfix_elem(tmpstr));
                    stack_temp.pop();
                }
                if(stack_temp.empty())
                {
                    cerr << "Syntax Error: Unexpected \"]\" !" << endl;
                    out_error << "Syntax Error: Unexpected \"]\" !" << endl;
                    error_flag = true;
                }
                else
                {
                    postfix_record.push_back(postfix_elem("[]", 3));
                    stack_temp.pop();
                }
            }
            else if(t[i].table == 2)
            {
                while(!stack_temp.empty() && priority_le(token_text, stack_temp.top()))
                {
                    string tmpstr = stack_temp.top();
                    postfix_record.push_back(postfix_elem(tmpstr));
                    stack_temp.pop();
                }
                stack_temp.push(token_text);
            }
        }
        if(error_flag)
        {
            postfix_record.clear();
            return false;
        }
        else
        {
            while(!stack_temp.empty() &&
                    stack_temp.top() != "(" && stack_temp.top() != ")" &&
                    stack_temp.top() != "[" && stack_temp.top() != "]")
            {
                string tmpstr = stack_temp.top();
                postfix_record.push_back(postfix_elem(tmpstr, 1));
                stack_temp.pop();
            }
            if(!stack_temp.empty())
            {
                cerr << "Syntax Error: Brackets balance error!" << endl;
                out_error << "Syntax Error: Brackets balance error!" << endl;
                error_flag = true;
            }
        }
        if(error_flag)
        {
            postfix_record.clear();
            return false;
        }
        if(postfix_record[postfix_record.size() - 1].id == "[]")
        {
            postfix_record[postfix_record.size() - 1] = postfix_elem("[*]", 2);
        }
        index = i + 1;
        postfix_record.push_back(postfix_elem(";", 4));
    }
    return true;
}

// Печать постфиксной записи в файл и на экран
void translator::postfix_print(string file_tree)
{
    ofstream out(file_tree.c_str());
    cout << "Postfix notation:" << endl;
    for(int i = 0; i < (int)postfix_record.size(); i++)
    {
        cout << postfix_record[i] << " ";
        out << postfix_record[i] << " ";
    }
    cout << endl;
    out.close();
}

// Сравнение приоритетов операций
bool translator::priority_le(string what, string with_what)
{
    int pw = 0, pww = 0;
    if(what == "=" || what == "+=" || what == "-=" || what == "*=") pw = 10;
    else if(what == "!=" || what == ">" || what == "<" || what == "==") pw = 20;
    else if(what == "+" || what == "-") pw = 30;
    else pw = 40;
    if(with_what == "=" || with_what == "+=" || with_what == "-=" || with_what == "*=") pww = 10;
    else if(with_what == "!=" || with_what == ">" || with_what == "<" || with_what == "==") pww = 20;
    else if(with_what == "+" || with_what == "-") pww = 30;
    else if(with_what == "*") pww = 40;
    if(pw <= pww) return true;
    return false;
};
