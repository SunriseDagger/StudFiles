#include "translator.h"

/** ================ Упрощение выражений с константами, РГЗ ================ */

// Частные случаи
bool translator::special_case(postfix_elem oper1p, postfix_elem oper2p, postfix_elem operation, postfix_elem& result)
{
    // Штуки вроде a*0 или 0*a
    if(operation.id == "*" && (oper2p.id == "0.0" || oper2p.id == "0" ||
                               ((oper1p.id == "0.0" || oper1p.id == "0") &&
                                (oper2p.table == 6 || oper2p.table == 5)) ))
    {
        if(oper2p.id == "0.0" || oper2p.id == "0")
            result = oper2p;
        else
            result = oper1p;
        return true;
    }
    // Штуки вроде a*1 или 1*a
    else if(operation.id == "*" && (oper2p.id == "1.0" || oper2p.id == "1" ||
                                    ((oper1p.id == "1.0" || oper1p.id == "1") &&
                                     (oper2p.table == 6 || oper2p.table == 5)) ))
    {
        if(oper2p.id == "1.0" || oper2p.id == "1")
            result = oper1p;
        else
            result = oper2p;
        return true;
    }
    // Штуки вроде a+0 или 0+a
    else if(operation.id == "+" && (oper2p.id == "0.0" || oper2p.id == "0" ||
                                    ((oper1p.id == "0.0" || oper1p.id == "0") &&
                                     (oper2p.table == 6 || oper2p.table == 5)) ))
    {
        if(oper2p.id == "0.0" || oper2p.id == "0")
            result = oper1p;
        else
            result = oper2p;
        return true;
    }
    // Штуки вроде a-0
    else if(operation.id == "-" && (oper2p.id == "0.0" || oper2p.id == "0"))
    {
        result = oper1p;
        return true;
    }
    return false;
}

// Свертка констант
void translator::constant_folding(vector<postfix_elem>& postfix_tmp)
{
    cout << "Postfix notation before optimize:" << endl;
    for(int i = 0; i < (int)postfix_tmp.size(); i++)
    {
        cout << postfix_tmp[i] << " ";
    }
    cout << endl;
    deque<postfix_elem> optimize_deque;
    bool is_changed = true;
    while(is_changed)
    {
        is_changed = false;
        for(int i = 0; i < (int)postfix_tmp.size(); i++)
        {
            if(postfix_tmp[i].table == 5 || postfix_tmp[i].table == 6)
            {
                optimize_deque.push_back(postfix_tmp[i]);
            }
            else
            {
                int oper1i = 0, oper2i = 0;
                float oper1f = 0.0, oper2f = 0.0;
                int type1 = 0, type2 = 0, typer = 0;
                //bool is_local_change = false;
                postfix_elem oper1p, oper2p;
                postfix_elem result_special;
                deque<postfix_elem>::iterator it = optimize_deque.end() - 1;
                oper2p = *it--;
                oper1p = *it;
                //oper2p = optimize_deque.back();
                /// Костыль!
                /*optimize_deque.pop_back();
                oper1p = optimize_deque.back();
                optimize_deque.push_back(oper2p);*/
                /// End Костыль!
                // Первая фаза оптимизации
                if(oper2p.table == 6)
                {
                    optimize_deque.pop_back();
                    //oper1p = optimize_deque.back();
                    if(oper1p.table == 6)
                    {
                        //is_local_change = true;
                        optimize_deque.pop_back();
                        stringstream a;
                        lexeme lex;
                        constants.get_lexeme(oper1p.id, lex);
                        type1 = lex.type;
                        a.str("");
                        a.clear();
                        a.str(oper1p.id);
                        if(type1 == 2)
                            a >> oper1f;
                        else
                            a >> oper1i;
                        constants.get_lexeme(oper2p.id, lex);
                        type2 = lex.type;
                        a.str("");
                        a.clear();
                        a.str(oper2p.id);
                        if(type2 == 2)
                            a >> oper2f;
                        else
                            a >> oper2i;
                        // Приведение типов
                        if(type1 == 2)
                        {
                            if(type2 != 2)
                            {
                                oper2f = (float)oper2i;
                                type2 = 2;
                            }
                            typer = 2;
                        }
                        else
                        {
                            if(type2 == 2)
                            {
                                oper1f = (float)oper1i;
                                type1 = 2;
                                typer = 2;
                            }
                            else
                            {
                                typer = 1;
                            }
                        }
                        // Собственно выполнение операции
                        a.str("");
                        a.clear();
                        if(postfix_tmp[i].id == "+") // +
                        {
                            if(typer == 2)
                                a << oper1f + oper2f;
                            else
                                a << oper1i + oper2i;
                            string result = a.str();
                            constants.add(result);
                            constants.set_type(result, typer);
                            optimize_deque.push_back(postfix_elem(result, 1, 6));
                            is_changed = true;
                        }
                        else if(postfix_tmp[i].id == "-") // -
                        {
                            if(typer == 2)
                                a << oper1f - oper2f;
                            else
                                a << oper1i - oper2i;
                            string result = a.str();
                            constants.add(result);
                            constants.set_type(result, typer);
                            optimize_deque.push_back(postfix_elem(result, 1, 6));
                            is_changed = true;
                        }
                        else if(postfix_tmp[i].id == "*") // *
                        {
                            if(typer == 2)
                                a << oper1f * oper2f;
                            else
                                a << oper1i * oper2i;
                            string result = a.str();
                            constants.add(result);
                            constants.set_type(result, typer);
                            optimize_deque.push_back(postfix_elem(result, 1, 6));
                            is_changed = true;
                        }
                        else if(postfix_tmp[i].id == "==") // ==
                        {
                            if(typer == 2)
                                a << (oper1f == oper2f);
                            else
                                a << (oper1i == oper2i);
                            string result = a.str();
                            constants.add(result);
                            constants.set_type(result, 1);
                            optimize_deque.push_back(postfix_elem(result, 1, 6));
                            is_changed = true;
                        }
                        else if(postfix_tmp[i].id == "!=") // !=
                        {
                            if(typer == 2)
                                a << (oper1f != oper2f);
                            else
                                a << (oper1i != oper2i);
                            string result = a.str();
                            constants.add(result);
                            constants.set_type(result, 1);
                            optimize_deque.push_back(postfix_elem(result, 1, 6));
                            is_changed = true;
                        }
                        else if(postfix_tmp[i].id == ">") // >
                        {
                            if(typer == 2)
                                a << (oper1f > oper2f);
                            else
                                a << (oper1i > oper2i);
                            string result = a.str();
                            constants.add(result);
                            constants.set_type(result, 1);
                            optimize_deque.push_back(postfix_elem(result, 1, 6));
                            is_changed = true;
                        }
                        else if(postfix_tmp[i].id == "<") // <
                        {
                            if(typer == 2)
                                a << (oper1f < oper2f);
                            else
                                a << (oper1i < oper2i);
                            string result = a.str();
                            constants.add(result);
                            constants.set_type(result, 1);
                            optimize_deque.push_back(postfix_elem(result, 1, 6));
                            is_changed = true;
                        }
                        else // Неразрешенная операция, откат
                        {
                            //is_local_change = false;
                            optimize_deque.push_back(oper1p);
                            optimize_deque.push_back(oper2p);
                            optimize_deque.push_back(postfix_tmp[i]);
                        }
                    }
                    else if(special_case(oper1p, oper2p, postfix_tmp[i], result_special))
                    {
                        optimize_deque.pop_back();
                        optimize_deque.push_back(result_special);
                        is_changed = true;
                    }
                    else
                    {
                        optimize_deque.push_back(oper2p);
                        optimize_deque.push_back(postfix_tmp[i]);
                    }
                }
                else if(special_case(oper1p, oper2p, postfix_tmp[i], result_special))
                {
                    optimize_deque.pop_back();
                    optimize_deque.pop_back();
                    optimize_deque.push_back(result_special);
                    is_changed = true;
                }
                else
                {
                    optimize_deque.push_back(postfix_tmp[i]);
                }
                // Вторая фаза оптимизации
                /*if(!is_local_change)
                {
                    deque<postfix_elem>::iterator it = optimize_deque.end() - 2;
                    oper2p = *it--;
                    oper1p = *it;
                    // Штуки вроде a*0 или 0*a
                    if(postfix_tmp[i].id == "*" && (oper2p.id == "0.0" || oper2p.id == "0" || oper1p.id == "0.0" || oper1p.id == "0"))
                    {
                        optimize_deque.erase(it, it + 3);
                        if(oper2p.id == "0.0" || oper1p.id == "0.0")
                            optimize_deque.push_back(postfix_elem("0.0", 1, 6));
                        else
                            optimize_deque.push_back(postfix_elem("0", 1, 6));
                        is_changed = true;
                    }
                    // Штуки вроде a*1 или 1*a
                    else if(postfix_tmp[i].id == "*" && (oper2p.id == "1.0" || oper2p.id == "1" || oper1p.id == "1.0" || oper1p.id == "1"))
                    {
                        optimize_deque.erase(it, it + 3);
                        if(oper2p.id == "1.0" || oper2p.id == "1")
                            optimize_deque.push_back(oper1p);
                        else
                            optimize_deque.push_back(oper2p);
                        is_changed = true;
                    }
                    // Штуки вроде a+0 или 0+a
                    else if(postfix_tmp[i].id == "+" && (oper2p.id == "0.0" || oper2p.id == "0" || oper1p.id == "0.0" || oper1p.id == "0"))
                    {
                        optimize_deque.erase(it, it + 3);
                        if(oper2p.id == "0.0" || oper2p.id == "0")
                            optimize_deque.push_back(oper1p);
                        else
                            optimize_deque.push_back(oper2p);
                        is_changed = true;
                    }
                    // Штуки вроде a-0
                    else if(postfix_tmp[i].id == "-" && (oper2p.id == "0.0" || oper2p.id == "0"))
                    {
                        optimize_deque.erase(it + 1, it + 3);
                        is_changed = true;
                    }
                }*/
            }
        }

        postfix_tmp.clear();
        for(deque<postfix_elem>::iterator it = optimize_deque.begin(); it != optimize_deque.end(); it++)
            postfix_tmp.push_back(*it);
        optimize_deque.clear();
    }

    /*cout << "Postfix notation after optimize:" << endl;
    for(int i = 0; i < (int)postfix_tmp.size(); i++)
        cout << postfix_tmp[i] << " ";
    cout << endl;*/
}

// Распространение констант
//void translator::constant_propagation()
//{
//    struct identifier
//    {
//        string name;
//        int index;
//        union
//        {
//            int val_int;
//            float val_float;
//        };
//        identifier()
//        {
//            name = "";
//            index = 0;
//            val_float = 0.0;
//            val_int = 0;
//        }
//        identifier(string name_)
//        {
//            name = name_;
//            index = 0;
//            val_float = 0.0;
//            val_int = 0;
//        }
//        identifier(string name_, int index_)
//        {
//            name = name_;
//            index = index_;
//            val_float = 0.0;
//            val_int = 0;
//        }
//    };
//
//
//
//    //vector<lexeme_adv> adv_vect;
//    vector<postfix_elem> postfix_record_new;
//    deque<postfix_elem> optimize_deque;
//    //stack<int> indexes_stack;
//    //stack<lexeme> lexemes_stack;
//    int index = 0;
//    bool local_error = false;
//
//    while(!local_error && index < (int)postfix_record.size())
//    {
//        int i;
//        for(i = index; !local_error && i < (int)postfix_record.size() && postfix_record[i].id != "="; i++)
//        {
//            if(postfix_record[i].table == 5 || postfix_record[i].table == 6)
//            {
//                optimize_deque.push_back(postfix_record[i]);
//                //lexemes_stack
//                /*if(postfix_record[i].table == 5 && i == index)
//                {
//                    lexeme lex;
//                    identifiers.get_lexeme(postfix_record[i].id, lex);
//                    adv_vect
//                }*/
//            }
//            else
//            {
//                int oper1i = 0, oper2i = 0;
//                float oper1f = 0.0, oper2f = 0.0;
//                int type1 = 0, type2 = 0, typer = 0;
//                bool is_local_change = false;
//                postfix_elem oper1p, oper2p;
//                oper2p = optimize_deque.back();
//                optimize_deque.pop_back();
//                oper1p = optimize_deque.back();
//                optimize_deque.pop_back();
//
//                optimize_deque.push_back(oper1p);
//                optimize_deque.push_back(oper2p);
//                optimize_deque.push_back(postfix_record[i]);
//                /*// Первая фаза оптимизации
//                if(oper2p.table == 6)
//                {
//
//
//                    if(oper1p.table == 6)
//                    {
//                        is_local_change = true;
//                        optimize_deque.pop_back();
//                        stringstream a;
//                        lexeme lex;
//                        constants.get_lexeme(oper1p.id, lex);
//                        type1 = lex.type;
//                        a.str("");
//                        a.clear();
//                        a.str(oper1p.id);
//                        if(type1 == 2)
//                            a >> oper1f;
//                        else
//                            a >> oper1i;
//                        constants.get_lexeme(oper2p.id, lex);
//                        type2 = lex.type;
//                        a.str("");
//                        a.clear();
//                        a.str(oper2p.id);
//                        if(type2 == 2)
//                            a >> oper2f;
//                        else
//                            a >> oper2i;
//                        // Приведение типов
//                        if(type1 == 2)
//                        {
//                            if(type2 != 2)
//                            {
//                                oper2f = (float)oper2i;
//                                type2 = 2;
//                            }
//                            typer = 2;
//                        }
//                        else
//                        {
//                            if(type2 == 2)
//                            {
//                                oper1f = (float)oper1i;
//                                type1 = 2;
//                                typer = 2;
//                            }
//                            else
//                            {
//                                typer = 1;
//                            }
//                        }
//                        // Собственно выполнение операции
//                        a.str("");
//                        a.clear();
//                        if(postfix_tmp[i].id == "+") // +
//                        {
//                            if(typer == 2)
//                                a << oper1f + oper2f;
//                            else
//                                a << oper1i + oper2i;
//                            string result = a.str();
//                            constants.add(result);
//                            constants.set_type(result, typer);
//                            optimize_deque.push_back(postfix_elem(result, 1, 6));
//                            is_changed = true;
//                        }
//                        else if(postfix_tmp[i].id == "-") // -
//                        {
//                            if(typer == 2)
//                                a << oper1f - oper2f;
//                            else
//                                a << oper1i - oper2i;
//                            string result = a.str();
//                            constants.add(result);
//                            constants.set_type(result, typer);
//                            optimize_deque.push_back(postfix_elem(result, 1, 6));
//                            is_changed = true;
//                        }
//                        else if(postfix_tmp[i].id == "*") // *
//                        {
//                            if(typer == 2)
//                                a << oper1f * oper2f;
//                            else
//                                a << oper1i * oper2i;
//                            string result = a.str();
//                            constants.add(result);
//                            constants.set_type(result, typer);
//                            optimize_deque.push_back(postfix_elem(result, 1, 6));
//                            is_changed = true;
//                        }
//                        else if(postfix_tmp[i].id == "==") // ==
//                        {
//                            if(typer == 2)
//                                a << (oper1f == oper2f);
//                            else
//                                a << (oper1i == oper2i);
//                            string result = a.str();
//                            constants.add(result);
//                            constants.set_type(result, 1);
//                            optimize_deque.push_back(postfix_elem(result, 1, 6));
//                            is_changed = true;
//                        }
//                        else if(postfix_tmp[i].id == "!=") // !=
//                        {
//                            if(typer == 2)
//                                a << (oper1f != oper2f);
//                            else
//                                a << (oper1i != oper2i);
//                            string result = a.str();
//                            constants.add(result);
//                            constants.set_type(result, 1);
//                            optimize_deque.push_back(postfix_elem(result, 1, 6));
//                            is_changed = true;
//                        }
//                        else if(postfix_tmp[i].id == ">") // >
//                        {
//                            if(typer == 2)
//                                a << (oper1f > oper2f);
//                            else
//                                a << (oper1i > oper2i);
//                            string result = a.str();
//                            constants.add(result);
//                            constants.set_type(result, 1);
//                            optimize_deque.push_back(postfix_elem(result, 1, 6));
//                            is_changed = true;
//                        }
//                        else if(postfix_tmp[i].id == "<") // <
//                        {
//                            if(typer == 2)
//                                a << (oper1f < oper2f);
//                            else
//                                a << (oper1i < oper2i);
//                            string result = a.str();
//                            constants.add(result);
//                            constants.set_type(result, 1);
//                            optimize_deque.push_back(postfix_elem(result, 1, 6));
//                            is_changed = true;
//                        }
//                        else // Неразрешенная операция, откат
//                        {
//                            is_local_change = false;
//                            optimize_deque.push_back(oper1p);
//                            optimize_deque.push_back(oper2p);
//                            optimize_deque.push_back(postfix_tmp[i]);
//                        }
//                    }
//                    else
//                    {
//                        optimize_deque.push_back(oper2p);
//                        optimize_deque.push_back(postfix_tmp[i]);
//                    }
//                }
//                else
//                {
//                    optimize_deque.push_back(postfix_tmp[i]);
//                }*/
//            }
//        }
//        index = i + 1;
//    }
//
//}
//
