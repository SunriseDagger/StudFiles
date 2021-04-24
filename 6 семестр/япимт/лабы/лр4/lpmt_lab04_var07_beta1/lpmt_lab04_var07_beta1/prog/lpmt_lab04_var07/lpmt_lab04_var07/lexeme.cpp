#include "lexeme.h"

/** Класс для хранения идентификаторов и констант */

// Конструктор по умолчанию
lexeme::lexeme()
{
    name = "";
    type = 0;
    dimension = 0;
}

// Конструктор с заданием имени идентификатора или значения константы
lexeme::lexeme(string new_name)
{
    name = new_name;
    type = 0;
    is_init.push_back(false);
    dimension = 0;
}

// Конструктор с заданием другого экземпляра класса
lexeme::lexeme(const lexeme& other)
{
    name = other.name;
    type = other.type;
    dimension = other.dimension;
    is_init.resize(other.is_init.size());
    is_init = other.is_init;
}

// Деструктор
lexeme::~lexeme()
{
    is_init.clear();
}

// Оператор присваивания
lexeme& lexeme::operator = (const lexeme &other)
{
    if(this != &other)
    {
        name = other.name;
        type = other.type;
        dimension = other.dimension;
        is_init.resize(other.is_init.size());
        is_init = other.is_init;
    }
    return *this;
}

//
///** Класс для реализации распространения констант (РГЗ) */
//
//lexeme_adv::val_el::val_el()
//{
//    val_float = 0.0;
//    val_int = 0;
//}
//
//lexeme_adv::val_el::val_el(int val)
//{
//    val_float = 0.0;
//    val_int = val;
//}
//
//lexeme_adv::val_el::val_el(float val)
//{
//    val_int = 0;
//    val_float = val;
//}
//
//// Конструктор по умолчанию
//lexeme_adv::lexeme_adv()
//{
//    name = "";
//    type = 0;
//    dimension = 0;
//}
//
//// Конструктор с заданием имени идентификатора или значения константы
//lexeme_adv::lexeme_adv(string new_name)
//{
//    name = new_name;
//    type = 0;
//    dimension = 0;
//    is_init.push_back(false);
//    val.push_back(val_el());
//}
//
//// Конструктор с заданием другого экземпляра класса
//lexeme_adv::lexeme_adv(const lexeme_adv& other)
//{
//    name = other.name;
//    type = other.type;
//    dimension = other.dimension;
//    is_init.resize(other.is_init.size());
//    val.resize(other.val.size());
//    is_init = other.is_init;
//    val = other.val;
//}
//
//// Конструктор с заданием экземпляра класса lexeme
//lexeme_adv::lexeme_adv(const lexeme& other)
//{
//    name = other.name;
//    type = other.type;
//    dimension = other.dimension;
//    is_init.resize(other.is_init.size());
//    val.resize(other.is_init.size());
//    is_init = other.is_init;
//}
//
//// Деструктор
//lexeme_adv::~lexeme_adv()
//{
//    is_init.clear();
//    val.clear();
//}
//
//// Оператор присваивания
//lexeme_adv& lexeme_adv::operator = (const lexeme_adv &other)
//{
//    if(this != &other)
//    {
//        name = other.name;
//        type = other.type;
//        dimension = other.dimension;
//        is_init.resize(other.is_init.size());
//        val.resize(other.is_init.size());
//        is_init = other.is_init;
//        val = other.val;
//    }
//    return *this;
//}
//
//// Оператор присваивания
//lexeme_adv& lexeme_adv::operator = (const lexeme &other)
//{
//    if(this != &other)
//    {
//        name = other.name;
//        type = other.type;
//        dimension = other.dimension;
//        is_init.resize(other.is_init.size());
//        val.resize(other.is_init.size());
//        is_init = other.is_init;
//    }
//    return *this;
//}
//
//// Установка размерности
//void lexeme_adv::set_dimension(int new_dimension)
//{
//    dimension = new_dimension;
//    int real_dimension = (1 > dimension ? 1 : dimension);
//    is_init.resize(real_dimension);
//    val.resize(real_dimension);
//    for(int i = 0; i < real_dimension; i++)
//        is_init[i] = false;
//}
//
