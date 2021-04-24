#ifndef _TABLE_H
#define _TABLE_H

#include <string>
#include <vector>

using namespace std;

/**
  *@class простейщая таблица, хранящая строки
  */
class table{
	//вектор строк
	vector<string> arr;
public:
	//конструктор по умолчанию
	table():arr()			{};
	//конструктор копирования
	table(const table &x):arr(x.arr){};
	//добавление элемента
	int add(const string &newelem);
	//удаление элемента
    string remove(int elem);
	//размер таблицы
	const int size() const			{return arr.size();};
	//доступ к элементам
    const string& operator[](int i) {return arr[i];};
	//деструктор
	virtual ~table()		{};
};

#endif //_TABLE_H
