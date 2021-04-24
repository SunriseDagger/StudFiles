#include "table_var.h"
// Размер хэш-таблицы по умолчанию
#define default_hashnum 100

// Подсчет хэша
int table_var::get_hash(string name)
{
    int hash = 0;
    for(int i = 0; i < (int)name.size(); i++)
        hash += name[i];
    return hash % hashnum;
}

// Подсчет номера в цепочке
int table_var::get_chain(string name)
{
    for(int i = 0, h = get_hash(name); i < (int)table[h].size(); i++)
        if(name == table[h][i].name) return i;
    return -1;
}

// Конструктор с размером таблицы по умолчанию
table_var::table_var()
{
    hashnum=default_hashnum;
    table = new vector<lexeme> [hashnum];
}

// Конструктор с пользовательским размером таблицы
table_var::table_var(int new_hashnum)
{
    hashnum=new_hashnum;
    table = new vector<lexeme> [hashnum];
}

// Деструктор
table_var::~table_var()
{
    for(int i = 0; i < hashnum; i++)
        table[i].clear();
    delete [] table;
}

// Проверка есть ли элемент в таблице
inline bool table_var::contains(string name)
{
    if(get_chain(name) != -1) return true;
    return false;
}

// Добавление нового имени идентификатора или значения константы
bool table_var::add(string name)
{
    if(contains(name)) return false;
    int h = get_hash(name);
    table[h].push_back(lexeme(name));
    return true;
}

// Задание типа по хэшу и номеру в цепочке
bool table_var::set_type(int hash, int chain, int type)
{
    if(chain == -1) return false;
    table[hash][chain].type = type;
    return true;
}

// Задание типа по имени идентификатора или значению константы
bool table_var::set_type(string name, int type)
{
    int hash = get_hash(name), chain = get_chain(name);
    return set_type(hash, chain, type);
}

// Задание размерности по хэшу и номеру в цепочке
bool table_var::set_dimension(int hash, int chain, int dimension)
{
    if(chain == -1) return false;
    table[hash][chain].dimension = dimension;
    table[hash][chain].is_init.resize(dimension);
    for(int i = 0; i < dimension; i++)
        table[hash][chain].is_init[i] = false;
    return true;
}

// Задание размерности по имени идентификатора или значению константы
bool table_var::set_dimension(string name, int dimension)
{
    int hash = get_hash(name), chain = get_chain(name);
    return set_dimension(hash, chain, dimension);
}

// Задание флага инициализации для массивов по хэшу и номеру в цепочке
bool table_var::set_is_init(int hash, int chain, bool is_init, int init_index)
{
    if(chain == -1) return false;
    table[hash][chain].is_init[init_index] = is_init;
    return true;
}

// Задание флага инициализации для массивов по имени идентификатора или значению константы
bool table_var::set_is_init(string name, bool is_init, int init_index)
{
    int hash = get_hash(name), chain = get_chain(name);
    return set_is_init(hash, chain, is_init, init_index);
}

// Задание флага инициализации по хэшу и номеру в цепочке
bool table_var::set_is_init(int hash, int chain, bool is_init)
{
    return set_is_init(hash, chain, is_init, 0);
}

// Задание флага инициализации по имени идентификатора или значению константы
bool table_var::set_is_init(string name, bool is_init)
{
    return set_is_init(name, is_init, 0);
}

// Определение хэша и номера в цепочке
bool table_var::get_location(string name, int &hash, int &chain)
{
    int h = get_hash(name), c = get_chain(name);
    if(c == -1) return false;
    hash = h;
    chain = c;
    return true;
}

// Получение структуры lexeme по хэшу и номеру в цепочке
bool table_var::get_lexeme(int hash, int chain, lexeme &lexeme)
{
    if(chain == -1) return false;
    lexeme = table[hash][chain];
    return true;
}

// Получение структуры lexeme по имени идентификатора или значению константы
bool table_var::get_lexeme(string name, lexeme &lexeme)
{
    int hash = get_hash(name), chain = get_chain(name);
    return get_lexeme(hash, chain, lexeme);
}

// Отладочная печать
void table_var::debug_print(ostream& stream)
{
    for(int i = 0; i < hashnum; i++)
    {
        if(table[i].size())
        {
            stream << i << ":\t[ ";
            for(int j = 0; j < (int)table[i].size(); j++)
            {
                stream << table[i][j].name;
                switch(table[i][j].type)
                {
                case 0:
                    stream << "\tnotype\tdim=";
                    break;
                case 1:
                    stream << "\tint\tdim=";
                    break;
                case 2:
                    stream << "\tfloat\tdim=";
                    break;
                }
                stream << table[i][j].dimension << "\tinit={";
                for(int k = 0; k < table[i][j].dimension; k++)
                {
                    stream << table[i][j].is_init[k];
                    if(k != table[i][j].dimension - 1) stream << " ";
                }
                if(j != (int)table[i].size() - 1) stream << "},\t";
                else stream << "}";
            }
            stream << " ]" << endl;
        }
    }
}

#undef default_hashnum
