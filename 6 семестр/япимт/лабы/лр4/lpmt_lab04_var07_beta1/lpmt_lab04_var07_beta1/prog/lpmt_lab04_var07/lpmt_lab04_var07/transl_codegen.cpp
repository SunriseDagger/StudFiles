#include "translator.h"

/** ================ Генерация кода на Ассемблере ================ */

// Получения хэша для служебных переменных и констант
string translator::calc_salt(int length)
{
    string postfix_str = "";
    for(int i = 0; i < (int)postfix_record.size(); i++)
        postfix_str.append(postfix_record[i].id);
    locale loc;
    const collate<char>& coll = use_facet<collate<char> >(loc);
    unsigned long salt_long = coll.hash(postfix_str.data(), postfix_str.data() + postfix_str.length());
    unsigned long salt_long_orig = salt_long;
    unsigned long salt_long_new = salt_long >> 5;
    stringstream a;
    while((int)a.str().length() < length)
    {
        unsigned char c = (salt_long - (salt_long_new << 5));
        if(c < 10) c += '0';
        else if(c < 36) c += 'A' - 10;
        else c = '_';
        a << c;
        salt_long = salt_long_new;
        salt_long_new = salt_long_new >> 5;
        if(salt_long_new == 0)
        {
            salt_long_orig = salt_long_orig >> 1;
            salt_long = salt_long_orig;
            salt_long_new = salt_long >> 5;
        }
    }
    return a.str();
}

// Генерация кода
bool translator::generation_code(string file_asm, string file_error, bool need_printf, bool need_salt)
{
    ofstream out_code(file_asm.c_str(), ios::trunc);
    out_error.open(file_error.c_str(), ios::trunc);
    string salt;
    if(need_salt) salt = calc_salt(10);
    else salt = "";
    bool need_adv_int = false;

    stack<postfix_elem> parse_stack;
    vector<postfix_elem> variables;
    vector<string> values;
    stringstream outcode;

    int index = 0;
    bool local_error = false;

    while(!local_error && index < (int)postfix_record.size())
    {
        stack<postfix_elem> array_stack;
        bool array_assign_here;
        lexeme lex_array_assign;
        identifiers.get_lexeme(postfix_record[index].id, lex_array_assign);
        if(lex_array_assign.dimension > 0)
        {
            array_assign_here = true;
        }
        else
        {
            array_assign_here = false;
        }
        bool array_assign_is_accepted = false;
        //stringstream array_assign_address;
        bool maybe_uninit_flag = false;
        string maybe_uninit_name = "";

        if(!(postfix_record[index+2].id == "=" && postfix_record[index+1].table == 6))
            outcode << "\tfinit\n";

        int i;
        for(i = index; !local_error && i < (int)postfix_record.size() && postfix_record[i].id != ";"; i++)
        {
            if(postfix_record[i].table == 5 || postfix_record[i].table == 6)
            {
                parse_stack.push(postfix_record[i]);
                bool added = false;
                for(int j = 0; !added && j < (int)variables.size(); j++)
                {
                    if(variables[j] == postfix_record[i])
                        added = true;
                }
                if(!added)
                {
                    variables.push_back(postfix_record[i]);
                    values.push_back("");
                }
                lexeme lex_array_check;
                identifiers.get_lexeme(postfix_record[i].id, lex_array_check);
                if(lex_array_check.dimension > 0)
                {
                    array_stack.push(postfix_record[i]);
                }

                if(postfix_record[i].table == 5 && lex_array_check.dimension == 0 && !lex_array_check.is_init[0])
                {
                    if(i != index)
                    {
                        cerr << "Code error: Variable \"" << postfix_record[i].id << "\" is not initialized!" << endl;
                        out_error << "Code error: Variable \"" << postfix_record[i].id << "\" is not initialized!" << endl;
                        out_code.close();
                        out_error.close();
                        return false;
                    }
                    else
                    {
                        maybe_uninit_flag = true;
                        maybe_uninit_name = postfix_record[i].id;
                    }
                }
            }
            else
            {
                postfix_elem oper1p, oper2p;
                int type1 = 0, type2 = 0;
                lexeme lex;

                oper2p = parse_stack.top();
                parse_stack.pop();
                oper1p = parse_stack.top();
                parse_stack.pop();

                if(oper1p.table == 5)
                {
                    identifiers.get_lexeme(oper1p.id, lex);
                    type1 = lex.type;
                    if(postfix_record[i].id != "=" && postfix_record[i].id != "[]"
                            /*&& postfix_record[i].id != "+=" && postfix_record[i].id != "-=" && postfix_record[i].id != "*="*/)
                    {
                        if(type1 == 2)
                            outcode << "\tfld\t\tqword [" << oper1p.id << "]\n";
                        else
                            outcode << "\tfild\tdword [" << oper1p.id << "]\n";
                        //cout << oper1p.id << "loaded\n";
                    }

                }
                else if(oper1p.table == 6)
                {
                    constants.get_lexeme(oper1p.id, lex);
                    type1 = lex.type;
                    int hash, chain;
                    constants.get_location(oper1p.id, hash, chain);
                    if(postfix_record[i].id != "=")
                    {
                        if(type1 == 2)
                            outcode << "\tfld\t\tqword [const_" << hash << "_" << chain << "_" << salt << "]\n";
                        else
                            outcode << "\tfild\tdword [const_" << hash << "_" << chain << "_" << salt << "]\n";
                    }
                }

                if(oper2p.table == 5)
                {
                    identifiers.get_lexeme(oper2p.id, lex);
                    type2 = lex.type;
                    if(type2 == 2)
                        outcode << "\tfld\t\tqword [" << oper2p.id << "]\n";
                    else
                        outcode << "\tfild\tdword [" << oper2p.id << "]\n";
                    //cout << oper2p.id << " loaded\n";
                }
                else if(oper2p.table == 6)
                {
                    constants.get_lexeme(oper2p.id, lex);
                    type2 = lex.type;
                    int hash, chain;
                    constants.get_location(oper2p.id, hash, chain);
                    if(type2 == 2)
                        outcode << "\tfld\t\tqword [const_" << hash << "_" << chain << "_" << salt << "]\n";
                    else
                        outcode << "\tfild\tdword [const_" << hash << "_" << chain << "_" << salt << "]\n";
                }

                if(postfix_record[i].id == "+")
                {
                    outcode << "\tfadd\n";
                }
                else if(postfix_record[i].id == "-")
                {
                    if(oper2p.id == "last" && oper1p.id != "last")
                        outcode << "\tfsubr\n";
                    else
                        outcode << "\tfsub\n";
                }
                else if(postfix_record[i].id == "*")
                {
                    outcode << "\tfmul\n";
                }
                else if(postfix_record[i].id == "==")
                {
                    outcode << "\tfcomp\n";
                    outcode << "\tfistp\tdword [tmp_var_int_" << salt << "]\n";
                    outcode << "\tfstsw\tax\n\tsahf\n";
                    outcode << "\tje lbl_eq_" << i << "_" << salt << "\n";
                    outcode << "\tfldz\n\tjmp lbl_ex_" << i << "_" << salt << "\n";
                    outcode << "lbl_eq_" << i << "_" << salt << ":\n\tfld1\n";
                    outcode << "lbl_ex_" << i << "_" << salt << ":\n";
                    need_adv_int = true;
                }
                else if(postfix_record[i].id == "!=")
                {
                    outcode << "\tfcomp\n";
                    outcode << "\tfistp\tdword [tmp_var_int_" << salt << "]\n";
                    outcode << "\tfstsw\tax\n\tsahf\n";
                    outcode << "\tjne lbl_ne_" << i << "_" << salt << "\n";
                    outcode << "\tfldz\n\tjmp lbl_ex_" << i << "_" << salt << "\n";
                    outcode << "lbl_ne_" << i << "_" << salt << ":\n\tfld1\n";
                    outcode << "lbl_ex_" << i << "_" << salt << ":\n";
                    need_adv_int = true;
                }
                else if(postfix_record[i].id == ">")
                {
                    outcode << "\tfcomp\n";
                    outcode << "\tfistp\tdword [tmp_var_int_" << salt << "]\n";
                    outcode << "\tfstsw\tax\n\tsahf\n";
                    if(oper2p.id == "last" && oper1p.id != "last")
                        outcode << "\tja lbl_gt_" << i << "_" << salt << "\n";
                    else
                        outcode << "\tjb lbl_gt_" << i << "_" << salt << "\n";
                    outcode << "\tfldz\n\tjmp lbl_ex_" << i << "_" << salt << "\n";
                    outcode << "lbl_gt_" << i << "_" << salt << ":\n\tfld1\n";
                    outcode << "lbl_ex_" << i << "_" << salt << ":\n";
                    need_adv_int = true;
                }
                else if(postfix_record[i].id == "<")
                {
                    outcode << "\tfcomp\n";
                    outcode << "\tfistp\tdword [tmp_var_int_" << salt << "]\n";
                    outcode << "\tfstsw\tax\n\tsahf\n";
                    if(oper2p.id == "last" && oper1p.id != "last")
                        outcode << "\tjb lbl_lt_" << i << "_" << salt << "\n";
                    else
                        outcode << "\tja lbl_lt_" << i << "_" << salt << "\n";
                    outcode << "\tfldz\n\tjmp lbl_ex_" << i << "_" << salt << "\n";
                    outcode << "lbl_lt_" << i << "_" << salt << ":\n\tfld1\n";
                    outcode << "lbl_ex_" << i << "_" << salt << ":\n";
                    need_adv_int = true;
                }
                else if(postfix_record[i].id == "=")
                {
                    if(!array_assign_is_accepted)
                    {
                        if(type1 == 2)
                            outcode << "\tfstp\tqword [" << oper1p.id << "]\n";
                        else
                            outcode << "\tfistp\tdword [" << oper1p.id << "]\n";
                        identifiers.set_is_init(postfix_record[index].id, true);
                    }
                    else
                    {
                        outcode << "\tpop\t\tedx\n";
                        //cout << type1 << endl;
                        if(lex_array_assign.type == 2)
                        {
                            outcode << "\tfstp\tqword [" << lex_array_assign.name << "+edx*8]\n";
                        }
                        else
                        {
                            outcode << "\tfistp\tdword [" << lex_array_assign.name << "+edx*4]\n";
                        }
                    }

                }
                else if(postfix_record[i].id == "+=")
                {
                    if(maybe_uninit_flag)
                    {
                        cerr << "Code error: Variable \"" << maybe_uninit_name << "\" is not initialized!" << endl;
                        out_error << "Code error: Variable \"" << maybe_uninit_name << "\" is not initialized!" << endl;
                        out_code.close();
                        out_error.close();
                        return false;
                    }
                    if(!array_assign_is_accepted)
                    {
                        outcode << "\tfadd\n";
                        if(type1 == 2)
                        {
                            outcode << "\tfstp\tqword [" << oper1p.id << "]\n";
                        }
                        else
                        {
                            outcode << "\tfistp\tdword [" << oper1p.id << "]\n";
                        }
                    }
                    else
                    {
                        outcode << "\tpop\t\tedx\n";
                        if(lex_array_assign.type == 2)
                        {
                            outcode << "\tfld\t\tqword [" << lex_array_assign.name << "+edx*8]\n";
                            outcode << "\tfadd\n";
                            outcode << "\tfstp\tqword [" << lex_array_assign.name << "+edx*8]\n";
                        }
                        else
                        {
                            outcode << "\tfild\tdword [" << lex_array_assign.name << "+edx*4]\n";
                            outcode << "\tfadd\n";
                            outcode << "\tfistp\tdword [" << lex_array_assign.name << "+edx*4]\n";
                        }
                    }
                }
                else if(postfix_record[i].id == "*=")
                {
                    if(maybe_uninit_flag)
                    {
                        cerr << "Code error: Variable \"" << maybe_uninit_name << "\" is not initialized!" << endl;
                        out_error << "Code error: Variable \"" << maybe_uninit_name << "\" is not initialized!" << endl;
                        out_code.close();
                        out_error.close();
                        return false;
                    }
                    if(!array_assign_is_accepted)
                    {
                        outcode << "\tfmul\n";
                        if(type1 == 2)
                        {
                            outcode << "\tfstp\tqword [" << oper1p.id << "]\n";
                        }
                        else
                        {
                            outcode << "\tfistp\tdword [" << oper1p.id << "]\n";
                        }
                    }
                    else
                    {
                        outcode << "\tpop\t\tedx\n";
                        if(lex_array_assign.type == 2)
                        {
                            outcode << "\tfld\t\tqword [" << lex_array_assign.name << "+edx*8]\n";
                            outcode << "\tfmul\n";
                            outcode << "\tfstp\tqword [" << lex_array_assign.name << "+edx*8]\n";
                        }
                        else
                        {
                            outcode << "\tfild\tdword [" << lex_array_assign.name << "+edx*4]\n";
                            outcode << "\tfmul\n";
                            outcode << "\tfistp\tdword [" << lex_array_assign.name << "+edx*4]\n";
                        }
                    }
                }
                else if(postfix_record[i].id == "-=")
                {
                    if(maybe_uninit_flag)
                    {
                        cerr << "Code error: Variable \"" << maybe_uninit_name << "\" is not initialized!" << endl;
                        out_error << "Code error: Variable \"" << maybe_uninit_name << "\" is not initialized!" << endl;
                        out_code.close();
                        out_error.close();
                        return false;
                    }
                    if(!array_assign_is_accepted)
                    {
                        //if(oper2p.id == "last")
                        if(oper2p.id == "last" && oper1p.id != "last")
                            outcode << "\tfsubr\n";
                        else
                            outcode << "\tfsub\n";
                        if(type1 == 2)
                        {
                            outcode << "\tfstp\tqword [" << oper1p.id << "]\n";
                        }
                        else
                        {
                            outcode << "\tfistp\tdword [" << oper1p.id << "]\n";
                        }
                    }
                    else
                    {
                        outcode << "\tpop\t\tedx\n";
                        if(lex_array_assign.type == 2)
                        {
                            outcode << "\tfld\t\tqword [" << lex_array_assign.name << "+edx*8]\n";
                            outcode << "\tfsubr\n";
                            outcode << "\tfstp\tqword [" << lex_array_assign.name << "+edx*8]\n";
                        }
                        else
                        {
                            outcode << "\tfild\tdword [" << lex_array_assign.name << "+edx*4]\n";
                            outcode << "\tfsubr\n";
                            outcode << "\tfistp\tdword [" << lex_array_assign.name << "+edx*4]\n";
                        }
                    }
                    //cout << "Minus stack size " << parse_stack.size() << endl;
                    //cout << "Elems " << oper1p.id << " " << oper2p.id << endl;
                }
                else if(postfix_record[i].id == "[]")
                {
                    //cout << "Array!" << endl;
                    //outcode << "\tarray here\n";
                    //cout << oper1p.id << " " << array_stack.size() << endl;
                    array_stack.pop();
                    bool is_assign = false;
                    if(array_assign_here && !array_assign_is_accepted && array_stack.size() == 0)
                    {
                        //cout << "Heero =)\n";
                        array_assign_is_accepted = true;
                        is_assign = true;
                    }

                    need_adv_int = true;
                    outcode << "\tfistp\tdword [tmp_var_int_" << salt << "]\n";
                    outcode << "\tmov\t\tedx, [tmp_var_int_" << salt << "]\n";
                    if(!is_assign)
                    {
                        if(type1 == 2)
                        {
                            outcode << "\tfld\t\tqword [" << oper1p.id << "+edx*8]\n";
                        }
                        else
                        {
                            outcode << "\tfild\tdword [" << oper1p.id << "+edx*4]\n";
                        }
                    }
                    else
                    {
                        outcode << "\tpush\tedx\n";
                    }

                }
                parse_stack.push(postfix_elem("last"));
            }

            if(i == index && postfix_record[i+2].id == "=" && postfix_record[i+1].table == 6)
            {
                lexeme lex_init;
                if(identifiers.get_lexeme(postfix_record[i].id, lex_init) && lex_init.dimension == 0 && !lex_init.is_init[0])
                {
                    bool found = false;
                    int j;
                    for(j = 0; !found && j < (int)variables.size(); j++)
                    {
                        if(variables[j].id == postfix_record[i].id)
                            found = true;
                    }
                    if(found)
                    {
                        identifiers.set_is_init(postfix_record[i].id, true);
                        values[j-1] = postfix_record[i+1].id;
                        i += 2;
                    }
                }
            }
        }

        while(parse_stack.size() > 0)
            parse_stack.pop();
        index = i + 1;
    }

    stringstream printf_out;
    if(need_printf)
        out_code << "extern printf\n";

    stringstream bss_out;
    bool need_bss = false;

    out_code << "section .data\n";
    for(int i = 0; i < (int)variables.size(); i++)
    {
        lexeme lex;
        if(variables[i].table == 5)
        {
            identifiers.get_lexeme(variables[i].id, lex);
            if(lex.type == 2)
            {
                if(lex.dimension == 0)
                {
                    out_code << "\t" << variables[i].id << ":\tdq\t";
                    if(values[i] == "")
                        out_code << "0.0\n";
                    else
                    {
                        if(values[i].find(".") == string::npos)
                        {
                            stringstream a_type_force;
                            a_type_force << values[i];
                            int f_type_force;
                            a_type_force >> f_type_force;
                            out_code << (float)f_type_force << "\n";
                        }
                        else
                            out_code << values[i] << "\n";
                    }
                    if(need_printf)
                    {
                        out_code << "\tmsg_" << variables[i].id << "_" << salt << ":\tdb\t\"" << variables[i].id << "\",0\n";
                        printf_out << "\tpush\tdword msg_" << variables[i].id << "_" << salt << "\n";
                        printf_out << "\tpush\tdword printf_str_" << salt << "\n";
                        printf_out << "\tcall\tprintf\n\tadd\t\tesp, 8\n";
                        printf_out << "\tpush\tdword [" << variables[i].id << "+4]\n";
                        printf_out << "\tpush\tdword [" << variables[i].id << "]\n";
                        printf_out << "\tpush\tdword printf_flt_" << salt << "\n";
                        printf_out << "\tcall\tprintf\n\tadd\t\tesp, 12\n";
                    }
                }
                else
                {
                    need_bss = true;
                    bss_out << "\t" << variables[i].id << ":\tresq\t" << lex.dimension << "\n";
                    if(need_printf)
                    {
                        out_code << "\tmsg_" << variables[i].id << "_" << salt << ":\tdb\t\"" << variables[i].id << "\",0\n";
                        for(int j = 0; j < lex.dimension; j++)
                        {
                            printf_out << "\tmov\t\tedx, " << j << "\n";
                            printf_out << "\tpush\tedx" << "\n";
                            printf_out << "\tpush\tdword msg_" << variables[i].id << "_" << salt << "\n";
                            printf_out << "\tpush\tdword printf_arr_" << salt << "\n";
                            printf_out << "\tcall\tprintf\n\tadd\t\tesp, 12\n";
                            printf_out << "\tpush\tdword [" << variables[i].id << "+" << j << "*8+4]\n";
                            printf_out << "\tpush\tdword [" << variables[i].id << "+" << j << "*8]\n";
                            printf_out << "\tpush\tdword printf_flt_" << salt << "\n";
                            printf_out << "\tcall\tprintf\n\tadd\t\tesp, 12\n";
                        }
                    }
                }
            }
            else
            {
                if(lex.dimension == 0)
                {
                    out_code << "\t" << variables[i].id << ":\tdd\t";
                    if(values[i] == "")
                        out_code << "0\n";
                    else
                    {
                        if(values[i].find(".") != string::npos)
                        {
                            stringstream a_type_force;
                            a_type_force << values[i];
                            float i_type_force;
                            a_type_force >> i_type_force;
                            out_code << (int)round(i_type_force) << "\n";
                        }
                        else
                            out_code << values[i] << "\n";
                    }
                    if(need_printf)
                    {
                        out_code << "\tmsg_" << variables[i].id << "_" << salt << ":\tdb\t\"" << variables[i].id << "\",0\n";
                        printf_out << "\tpush\tdword msg_" << variables[i].id << "_" << salt << "\n";
                        printf_out << "\tpush\tdword printf_str_" << salt << "\n";
                        printf_out << "\tcall\tprintf\n\tadd\t\tesp, 8\n";
                        printf_out << "\tpush\tdword [" << variables[i].id << "]\n";
                        printf_out << "\tpush\tdword printf_int_" << salt << "\n";
                        printf_out << "\tcall\tprintf\n\tadd\t\tesp, 8\n";
                    }
                }
                else
                {
                    need_bss = true;
                    bss_out << "\t" << variables[i].id << ":\tresd\t" << lex.dimension << "\n";
                    if(need_printf)
                    {
                        out_code << "\tmsg_" << variables[i].id << "_" << salt << ":\tdb\t\"" << variables[i].id << "\",0\n";
                        for(int j = 0; j < lex.dimension; j++)
                        {
                            printf_out << "\tmov\t\tedx, " << j << "\n";
                            printf_out << "\tpush\tedx" << "\n";
                            printf_out << "\tpush\tdword msg_" << variables[i].id << "_" << salt << "\n";
                            printf_out << "\tpush\tdword printf_arr_" << salt << "\n";
                            printf_out << "\tcall\tprintf\n\tadd\t\tesp, 12\n";
                            printf_out << "\tpush\tdword [" << variables[i].id << "+" << j << "*4]\n";
                            printf_out << "\tpush\tdword printf_int_" << salt << "\n";
                            printf_out << "\tcall\tprintf\n\tadd\t\tesp, 8\n";
                        }
                    }
                }
            }
        }
        else
        {
            int hash, chain;
            constants.get_location(variables[i].id, hash, chain);
            constants.get_lexeme(hash, chain, lex);
            if(lex.type == 2)
                out_code << "\tconst_" << hash << "_" << chain << "_" << salt << ":\tdq\t" << variables[i].id << "\n";
            else
                out_code << "\tconst_" << hash << "_" << chain << "_" << salt << ":\tdd\t" << variables[i].id << "\n";
        }
    }

    if(need_adv_int)
    {
        out_code << "\ttmp_var_int_" << salt << ":\tdd\t0\n";
    }
    if(need_printf)
    {
        out_code << "\tprintf_flt_" << salt << ":\tdb\t\"%f\",10,0\n";
        out_code << "\tprintf_int_" << salt << ":\tdb\t\"%i\",10,0\n";
        out_code << "\tprintf_str_" << salt << ":\tdb\t\"%s = \",0\n";
    }
    if(need_bss)
    {
        if(need_printf) out_code << "\tprintf_arr_" << salt << ":\tdb\t\"%s[%i] = \",0\n";
        out_code << "section .bss\n" << bss_out.str();
    }

    out_code << "section .text\n\tglobal main\nmain:\n";
    //out_code << "\tfinit\n";
    out_code << outcode.str();
    out_code << printf_out.str();
    out_code << "\tmov\t\teax, 0\n\tret\n";

    out_code.close();
    out_error.close();
    return true;
}
