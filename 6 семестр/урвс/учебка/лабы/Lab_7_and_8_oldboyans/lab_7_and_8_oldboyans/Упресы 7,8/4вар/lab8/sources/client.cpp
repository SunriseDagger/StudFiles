/**
  *программа посылает запрос серверу (строку) и получает список
  *файлов, содержащих данную строку
  *@author Гасенко Валентин
  *@author Михеева Елизавета
  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>

//размер буфера для приема/отправки сообщений
#define buf_size 1024

using namespace std;

int main(int argc, char *argv[])
{
	//поддержка кириллицы
	setlocale(LC_ALL,"Russian");

	//адрес сервера, порт и строка для поиска
	if ( argc!=4 ){
		cerr<<"Ошибка : не указаны адрес сервера, порт и строка для поиска\n"; 
		exit(1); 
	}

	char * server = argv[1];
	int port_number = atol(argv[2]);

	//преобразуем адрес сервера в IP
	hostent * record = gethostbyname(server);
	if( record == NULL ){
            cerr<<"Ошибка : не удалось определить адрес сервера (клиент)\n";
			exit(1); 
	}
	in_addr * addressptr = (in_addr *) record->h_addr;

	//создаем сокет
	int main_socket = socket(AF_INET, SOCK_STREAM, 0);
	if ( main_socket<0 ){
		cerr<<"Ошибка : не удалось создать сокет\n";
		exit(1);
	}

	//создаем сруктуру, описывающую адрес сервера
	sockaddr_in server_info;
	server_info.sin_family = AF_INET;
	server_info.sin_addr = * addressptr;
	server_info.sin_port = htons(port_number);

	//пробуем подключиться
	int r = connect(main_socket, (sockaddr *) &server_info, sizeof(server_info));
	if ( r<0 ){ 
		cerr<<"Ошибка : не удалось подключиться к серверу\n";
		exit(1);
	}

	//Сишными функциями все же удобнее пользоваться, чем системными вызовами
	//open/write в этом случае
	FILE *r_connection = fdopen( main_socket, "r" );
	FILE *w_connection = fdopen( main_socket, "w" );


	//отправляем серверу строку для поска
    fprintf( w_connection, "%s\n", argv[3] );
	fflush( w_connection );

	char buf[buf_size];
	//выводим на экран все полученные ответы
    while( fgets( buf, buf_size, r_connection ) )
		cout<<buf;


	//закрываем все и завершаемся
	fclose(w_connection);
	fclose(r_connection);   
	close(main_socket);
	return 0;
}
