#include "lexeme.h"

// Конструктор по умолчанию
lexeme::lexeme() {}

// Конструктор с заданием имени идентификатора или значения константы
lexeme::lexeme(string new_name)
{
    name = new_name;
    type = 0;
    is_init.push_back(false);
    dimension = 1;
}

// Деструктор
lexeme::~lexeme()
{
    is_init.clear();
}
