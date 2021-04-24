#ifndef LEXEME_H_INCLUDED
#define LEXEME_H_INCLUDED

#include <string>
#include <vector>

using namespace std;

// Класс для хранения идентификаторов и констант
class lexeme
{
public:
    // Имя идентификатора или значение константы
    string name;
    // Тип, 0 - не определен, 1 - int, 2 - float
    int type;
    // Массив флагов "илициализировано ли" размерности max(dimension,1)
    vector<bool> is_init;
    // Размерность массива, для переменных и констант - 0.
    int dimension;
    // Конструктор по умолчанию
    lexeme();
    // Конструктор с заданием имени идентификатора или значения константы
    lexeme(string new_name);
    // Конструктор с заданием другого экземпляра класса
    lexeme(const lexeme& other);
    // Деструктор
    ~lexeme();
    // Оператор присваивания
    lexeme& operator = (const lexeme &other);
};

//// Класс для реализации распространения констант (РГЗ)
//class lexeme_adv: public lexeme
//{
//public:
//    // Структура, в которой вот так хранится значение
//    struct val_el
//    {
//        union
//        {
//            int val_int;
//            float val_float;
//        };
//        val_el();
//        val_el(int val);
//        val_el(float val);
//    };
//    // Массив значений размерности max(dimension,1)
//    vector<struct val_el> val;
//    // Конструктор по умолчанию
//    lexeme_adv();
//    // Конструктор с заданием имени идентификатора или значения константы
//    lexeme_adv(string new_name);
//    // Конструктор с заданием другого экземпляра класса
//    lexeme_adv(const lexeme_adv& other);
//    // Конструктор с заданием экземпляра класса lexeme
//    lexeme_adv(const lexeme& other);
//    // Деструктор
//    ~lexeme_adv();
//    // Оператор присваивания
//    lexeme_adv& operator = (const lexeme_adv &other);
//    // Оператор присваивания
//    lexeme_adv &operator = (const lexeme &other);
//    // Установка размерности
//    void set_dimension(int new_dimension);
//};

#endif // LEXEME_H_INCLUDED
