#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <map>
#include <string.h>
using namespace std;
typedef map<string, string> table;
/**
  *считывание таблицы из файла
  *@param fin поток для чтения таблицы
  */
void read_table( ifstream &fin, table main_table )
{
	string key, temp;
	while( !fin.eof() )
	{
		fin>>key;
		if( fin.eof() ) return;
		fin>>temp;
		if( main_table.count(key) > 0 )
			cout<<"Element s key "<<key<<" is already"<<endl;
		else
			main_table[key] = temp;
	}
}

int main()
{
	int soket, soket1;			// идентификаторы сокетов
	struct sockaddr st_addr;
	char buf[30];// mask[30];
	table main_table;
	//файл с таблицей
	ifstream fin("table.txt");
	if( !fin ){
		cout<<"error not found file table.txt"<<endl;
		exit(1);
	}
	read_table( fin, main_table );
	fin.close();
	//char data[1000];
	remove("INADDR_ANY");
	if ((soket = socket(AF_UNIX,SOCK_STREAM,0)) == -1)	// создание сокета
	cout << "Server: Error in creating socket." <<	endl, exit(1);
	//инициализация структуры sockaddr
    st_addr.sa_family = AF_UNIX;
    sprintf(st_addr.sa_data,"%s", "INADDR_ANY");
    // связываем сетевой адрес компьютера с идентификатором сокета
    if(bind(soket, &st_addr, sizeof(struct sockaddr))==-1)
    {	
		cout << "Server: Error binding." << endl;
		close(soket);
		exit(1);
    }
    // включаем прием сообщений
    cout << "Server: is trying to listen socket with id = " << sockid << endl;
    if(listen(soket, 10)==-1)
    {	
		cout<<"Error listening for server!"<<endl;
		close(sockid);
		exit(1);
    }
    cout << "Server: Listening." << endl;
    cout << "Server: Creating another socket for receiving messages." << endl;
    soket1 = accept(soket, 0, 0);
    cout << "Server: Created another socket with id = " << soket1 << endl;
    cout << "Server: Waiting for a message from client." << endl;
	if(recv(soket1, buf, 30*sizeof(char), MSG_DONTROUTE))
		cout << "Server: Received from client key: " << key << endl;
	int pid = fork();
	//ключ - это полученная строка без последнего символа перевода строки \n
	string key( buf, strlen(buf)-1 ), result;
	if(pid == -1) cout<<"Error could not create process"<<endl,exit(1);
	if (!pid)
	{	
		if( main_table.count(key) > 0 )
		result = main_table[key];
	else
		result = string("Ошибка : не найден элемент с  ключом ") + key;
		exit(0);
	}
	
	if(send(soket1,result.data(),sizeof(result.data()),MSG_WAITALL)!=-1) 
		cout << "Server: Send data." << endl;            
    cout << "Server: Closing socket." << endl;
    close(soket1);
    close(soket);
    cout << "Server: Closed socket." << endl;
    return 0;
}
