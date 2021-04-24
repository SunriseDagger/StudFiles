#include "translator.h"

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
    int type_type = 0;      // Если находимся, то какой тип объявляем
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
        if(curr_token.table == 6)
        {
            // Устанавливаем тип константы
            if(token_str.find(".") != string::npos)
                constants.set_type(curr_token.place, curr_token.chain, 2);
            else
                constants.set_type(curr_token.place, curr_token.chain, 1);
            token_str = "const";
        }

        // Ищем терминалы из списка
        bool find_terminal = false;
        //cout << "Curr Row = " << curr_row << endl;
        //cout << "Token: " << curr_token;
        //cout << "Token String: " << token_str << endl;
        for(int i = 0; i < (int)table_parse[curr_row].terminal.size() && !find_terminal; i++)
        {
            //cout << "Scan " << table_parse[curr_row].terminal[i] << " : ";
            if(table_parse[curr_row].terminal[i] == token_str)
                find_terminal = true;
            //cout << find_terminal << endl;
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
                         get_token_text(next_token) == "+=" ||
                         get_token_text(next_token) == "-=" ||
                         get_token_text(next_token) == "*=" ||
                         (get_token_text(next_token) == "[" && !have_type)))
                    need_postfix = true;

                if((token_str == "var" || token_str == "const") && have_type && get_token_text(next_token) == "[")
                    need_array_resize = true;

                // Обработка того, что для не-массивов нет оператора []
                if(token_str == "var" && !need_array_resize && !have_type && get_token_text(next_token) == "[")
                {
                    lexeme lex_var;
                    identifiers.get_lexeme(curr_token.place, curr_token.chain, lex_var);
                    if(lex_var.dimension == 0)
                    {
                        error_flag = true;
                        out_error << "Syntax Error: Can`t use operator \"[]\" for variable \"" << lex_var.name << "\"" << endl;
                        cerr << "Syntax Error: Can`t use operator \"[]\" for variable \"" << lex_var.name << "\"" << endl;
                    }
                }

                // Обработка того, что для массивов обязан быть оператор []
                if(token_str == "var" && !need_array_resize && !have_type && get_token_text(next_token) != "[")
                {
                    lexeme lex_var;
                    identifiers.get_lexeme(curr_token.place, curr_token.chain, lex_var);
                    if(lex_var.dimension != 0)
                    {
                        error_flag = true;
                        out_error << "Syntax Error: Must use operator \"[]\" for array \"" << lex_var.name << "\"" << endl;
                        cerr << "Syntax Error: Must use operator \"[]\" for array \"" << lex_var.name << "\"" << endl;
                    }
                }

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
                    lexeme lex;
                    if(next_token.table == 5)
                    {
                        identifiers.get_lexeme(next_token.place, next_token.chain, lex);
                    }
                    else
                    {
                        if(get_token_text(next_token).find(".") != string::npos)
                            lex.type = 2;
                        else
                            lex.type = 1;
                    }
                    if(lex.type == 2)
                    {
                        constants.add("-1.0");
                        constants.get_location("-1.0", one_hash, one_chain);
                        constants.set_type(one_hash, one_chain, 2);
                    }
                    else
                    {
                        constants.add("-1");
                        constants.get_location("-1", one_hash, one_chain);
                        constants.set_type(one_hash, one_chain, 1);
                    }
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
                    lexeme lex;
                    if(next_token.table == 5)
                    {
                        identifiers.get_lexeme(next_token.place, next_token.chain, lex);
                    }
                    else
                    {
                        if(get_token_text(next_token).find(".") != string::npos)
                            lex.type = 2;
                        else
                            lex.type = 1;
                    }
                    if(lex.type == 2)
                    {
                        constants.add("-1.0");
                        constants.get_location("-1.0", one_hash, one_chain);
                        constants.set_type(one_hash, one_chain, 2);
                    }
                    else
                    {
                        constants.add("-1");
                        constants.get_location("-1", one_hash, one_chain);
                        constants.set_type(one_hash, one_chain, 1);
                    }
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
                        /*if(!make_postfix(array_resize_expr_infix))
                            error_flag = true;*/
                        // Получим размер массива и занесем его в таблицу
                        vector<postfix_elem> array_resize_vect;
                        if(!make_postfix(array_resize_expr_infix, array_resize_vect))
                        {
                            error_flag = true;
                        }
                        else
                        {
                            if(array_resize_vect.size() == 4)
                            {
                                stringstream a;
                                a.str(array_resize_vect[1].id);
                                int dim;
                                a >> dim;
                                if(dim > 0)
                                    identifiers.set_dimension(array_resize_vect[0].id, dim);
                                else
                                {
                                    error_flag = true;
                                    cerr << "Syntax Error: Array \"" << array_resize_vect[0].id
                                         << "\" size must be > 0!" << endl;
                                    out_error << "Syntax Error: Array \"" << array_resize_vect[0].id
                                              << "\" size must be > 0!" << endl;
                                }
                            }
                            else
                            {
                                error_flag = true;
                                cerr << "Syntax Error: Array \"" << array_resize_vect[0].id
                                     << "\" size must be a constant value!" << endl;
                                out_error << "Syntax Error: Array \"" << array_resize_vect[0].id
                                          << "\" size must be a constant value!" << endl;
                            }
                        }
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
                {
                    lexeme lex;
                    identifiers.get_lexeme(curr_token.place, curr_token.chain, lex);
                    if(lex.type == 0)
                    {
                        identifiers.set_type(get_token_text(curr_token), type_type);
                    }
                    else
                    {
                        error_flag = true;
                        string oldtype = "unknown";
                        if(lex.type == 1) oldtype = "int";
                        else if(lex.type == 2) oldtype = "float";
                        cerr << "Syntax Error: Identifier \"" << get_token_text(curr_token)
                             << "\" was already defined as \"" << oldtype << "\"!" << endl;
                        out_error << "Syntax Error: Identifier \"" << get_token_text(curr_token)
                                  << "\" was already defined as \"" << oldtype << "\"!" << endl;
                    }
                }

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
    vector<postfix_elem> postfix_tmp;
    bool error_flag = make_postfix(t, postfix_tmp);
    for(int i = 0; error_flag && i < (int)postfix_tmp.size(); i++)
        postfix_record.push_back(postfix_tmp[i]);
    return error_flag;
}

// Построение постфиксной записи (локально)
bool translator::make_postfix(vector<token> t, vector<postfix_elem>& postfix_tmp)
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
                postfix_tmp.push_back(postfix_elem(token_text, 1, t[i].table));
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
                    postfix_tmp.push_back(postfix_elem(tmpstr));
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
                    postfix_tmp.push_back(postfix_elem(tmpstr));
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
                    postfix_tmp.push_back(postfix_elem("[]", 3));
                    stack_temp.pop();
                }
            }
            else if(t[i].table == 2)
            {
                while(!stack_temp.empty() && priority_le(token_text, stack_temp.top()))
                {
                    string tmpstr = stack_temp.top();
                    postfix_tmp.push_back(postfix_elem(tmpstr));
                    stack_temp.pop();
                }
                stack_temp.push(token_text);
            }
        }
        if(error_flag)
        {
            postfix_tmp.clear();
            return false;
        }
        else
        {
            while(!stack_temp.empty() &&
                    stack_temp.top() != "(" && stack_temp.top() != ")" &&
                    stack_temp.top() != "[" && stack_temp.top() != "]")
            {
                string tmpstr = stack_temp.top();
                postfix_tmp.push_back(postfix_elem(tmpstr, 1));
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
            postfix_tmp.clear();
            return false;
        }
        if(postfix_tmp[postfix_tmp.size() - 1].id == "[]")
        {
            postfix_tmp[postfix_tmp.size() - 1] = postfix_elem("[*]", 2);
        }
        index = i + 1;
        // Свертка констант, РГЗ
        constant_folding(postfix_tmp);
        // Конец свертки констант
        postfix_tmp.push_back(postfix_elem(";", 4));
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

