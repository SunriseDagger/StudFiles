/**
  *Программа моделирует работу примитивной СУБД,
  *хранящей единственную таблицу в оперативной памяти.
  *Выполняя некоторые циклы работ, K порожденных процессов посредством очереди сообщений
  *передают родительскому процессу номер строки, которую нужно удалить из таблицы. 
  *Родительский процесс выполняет указанную операцию и возвращает содержимое удалённой строки.
  *@author Гасенко Валентин
  *@author Михеева Елизавета
  */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <signal.h>
#include "table.h"

//ключи для очереди сообщений
key_t qkey_in=45000,qkey_out=qkey_in+1;
//максимальный размер сообщения
const size_t msgsize=512;
//количество процессов-потомков
int k;

//сообщение
typedef struct {
	long mtype;
	char mtext[msgsize];
} message;

/**
  *считывание файла конфигурации
  *@param fin открытый поток файла конфигурации
  *@param tb ссылка на таблицу
  *@return none
  */
void input(ifstream &fin,table &tb){
	try{
		//размер таблицы
		size_t tbsize;
		string temp;

		//сперва считываем количество потомков и размер таблицы	
		fin>>k>>tbsize;
		if( fin.fail() )
			throw "Ошибка чтения из файла конфигурации";
		if( k<0 )
			throw "Ошибка : неверное количество процессов";

		//потом считываем строки таблицы
		for( int i=0; i<tbsize; ++i ){
			fin>>temp;
			if( fin.fail() )
				throw "Ошибка чтения из файла конфигурации";
			if( temp.size()>msgsize )
				throw "Ошибка : слишком длинная запись в файле конфинурации";

			tb.add(temp);
			#ifdef _DEBUG
			cerr<<temp<<" добавлено в таблицу\n";
			cerr.flush();
			#endif
		}
	}		
        catch(const char *errmsg ){
		cerr<<errmsg<<endl;
		exit(1);
	}
}

/**
  *действия процесса потомка - попытка удалить из 
  *таблицы строку с определенным номером
  *@param row номер удаляемой строки
  *@return none 
  */
void process_func(int row){
	message msg;
	
	//получаем дескрипторы очередей сообщений
	int msgid_in=msgget(qkey_in,0);
	if( msgid_in<0 ){
		cerr<<"Ошибка : Процесс "<<getpid()<<" не смог получить дескриптор очереди сообщений\n";
		_exit(1);
	}
	int msgid_out=msgget(qkey_out,0);
	if( msgid_in<0 ){
		msgctl(msgid_in,IPC_RMID,NULL);
		cerr<<"Ошибка : Процесс "<<getpid()<<" не смог получить дескриптор очереди сообщений\n";
		_exit(1);
	}

	sprintf(msg.mtext,"%d %d",getpid(),row);
	msg.mtype=1;

	//посылаем родительскому процессу сообщение с номеров строки	
    msgsnd(msgid_in,(void*)&msg,strlen(msg.mtext)+1,0);
	#ifdef _DEBUG
	cerr<<"Процесс "<<getpid()<<" отправил "<<msg.mtext<<endl;
	cerr.flush();
	#endif

	//ждем ответа с таким же типом сообщения, какой pid у процесса
	msgrcv(msgid_out,(void*)&msg,msgsize,getpid(),0);
	#ifdef _DEBUG
	cerr<<"Процесс "<<getpid()<<" получил "<<msg.mtext<<endl;
	cerr.flush();
	#endif

	//завершаемся
    _exit(0);
}

/**
  *действия родительского процесса - удаление строк таблицы по 
  *запросу потомков
  *@param tb ссылка на таблицу
  *@param msgid_in дескриптор очереди сообщениий (входящие)
  *@param msgid_out дескриптор очереди сообщениий (исходящие)
  *@return none
  */
void main_pocess_func(table &tb,int msgid_in,int msgid_out){

	message msg;
	string res;

	int row,pid;
	//ждем по 1му сообщению от каждого потомка
	for( int i=0; i<k; ++i ){
		//получаем сообщение
        msgrcv(msgid_in,(void*)&msg,msgsize,0,0);
		#ifdef _DEBUG
        cerr<<"Основной процесс получил "<<msg.mtext<<endl;
		cerr.flush();
		#endif

		//считываем номер строки
        sscanf(msg.mtext,"%d %d",&pid,&row);
		//удаляем
        res=tb.remove(row);
		strcpy(msg.mtext,res.data());
		msg.mtype=pid;
		//отсылаем сообщение с типом, равным pid процесса, 
		//от которого было получено входящее сообщение
		msgsnd(msgid_out,(void*)&msg,strlen(msg.mtext)+1,0);
		#ifdef _DEBUG
        cerr<<"Основной процесс отправил "<<msg.mtext<<endl;
		cerr.flush();
		#endif
	}
}


int main(int argc,char *argv[]){
	int temp,msgid_in,msgid_out;
	//список идентификаторов потомков
	vector<int> pids;

	//включаем поддержку русских букв в консоли
	setlocale(LC_ALL,"Russian");
	if( argc<2 ){
		cerr<<"Ошибка : не найдено имя файла конфигурации\n";
		return 1; 
	}
	if( argc>2 ){
		for( int i=2; i<argc; ++i )
			cerr<<"Опция "<<argv[i]<<" будет проигнорирована\n";
	}

	//единственный параметр - имя файла конфигурации
	ifstream fin(argv[1]);
	if( !fin ){
		cerr<<"Ошибка : не удалось открыть файл "<<argv[1]<<endl;
		return 1;
	}

	//создаем и считываем таблицу
	table tb;
	input(fin,tb);
	fin.close();

	//создаем очереди сообщений
    msgid_in=msgget( qkey_in,0666  | IPC_CREAT | IPC_EXCL );
    if( msgid_in<0 ){
		cerr<<"Ошибка : не удалось создать очередь сообщений\n";
		return 1;
	}
	msgid_out=msgget( qkey_out,0666  | IPC_CREAT | IPC_EXCL );
    if( msgid_in<0 ){
		msgctl(msgid_in,IPC_RMID,NULL);
		cerr<<"Ошибка : не удалось создать очередь сообщений\n";
		return 1;
	}

	//создаем k потомков каждый удаляет строку со своим порядковым номером
	for( int i=0; i<k; ++i )
		switch( (temp=fork()) ){
			case 0: process_func(i); break;
			case -1: 
				cerr<<"Ошибка : не удалось создать новый процесс\n";
				for(vector<int>::const_iterator j=pids.begin(); j<pids.end(); ++j)
					kill(*j,SIGKILL);
			default:
				pids.push_back(temp);
		}

	main_pocess_func(tb,msgid_in,msgid_out);
	//удаляем очереди сообщений
	msgctl(msgid_in,IPC_RMID,NULL);
	msgctl(msgid_out,IPC_RMID,NULL);

	//выводим на экран содержимое таблицы
	for( int i=0; i<tb.size(); ++i )
		cout<<tb[i]<<endl;
	
	return 0;
}
