#include "table.h"

/**
  *добавление элемента в таблицу
  *@param newelem новый элемент
  *@return положение нового элемента в таблице
  */
int table::add(const string &newelem){
	int res;
	//ищем первое свободное место
	vector<string>::iterator i=arr.begin();
	for( ; i<arr.end() && !(i->empty()); ++i );

	//если свободных мест нет, то добавляем в конец
	if( i==arr.end() ){
		res=arr.size();
		arr.push_back(newelem);
	}
	else{
		res=i-arr.begin();
		*i=newelem;
	}
	return res;
}

/**
  *удаление элемента из таблицы
  *@param elem номер элемента
  *@return удаленный элемент
  */
string table::remove(int elem){
	string res;
	//если такой элемент есть, то очищаем его содержимое
	if( elem>=0 && elem<arr.size() ){
		res=arr[elem];
		arr[elem].clear();
	}
	return res;
}
