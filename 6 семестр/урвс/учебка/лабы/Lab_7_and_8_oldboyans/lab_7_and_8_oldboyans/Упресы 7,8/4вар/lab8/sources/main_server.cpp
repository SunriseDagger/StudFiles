/**
  *программа принимает от клиента строку и рассылает ее
  *всем серверам из списка, получает от них ответы
  *и отправляет обратно клиенту
  *@author Гасенко Валентин
  *@author Михеева Елизавета
  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <vector>
#include <iostream>
#include <fstream>

//размер буфера для приема/отправки сообщений
#define buf_size 1024

using namespace std;

extern int errno;
//список серверов
vector<sockaddr_in*> serverlist;


/**
  *обработчик сигнала SIGCHLD
  *@param sig номер сигнала
  *@return none
  */
void sigchld_handler(int sig){
	//забираем статус завершения потомка
	//дабы не плодить зомбей
        waitpid( -1, &sig, WUNTRACED | WNOHANG );
}

/**
  *считывание списка серверов
  *@param fin открытый поток со списком
  *@return none
  */
void read_serverlist( ifstream &fin ){
	char server[buf_size];
    int port_number;

	fin.unsetf(ios_base::skipws);
    while( !fin.eof() ){
		//адрес сервера
		fin.getline( server, buf_size, ':' );
		//не трогать, так надо!
        if( fin.eof() ) return;
		//номер порта
        fin>>port_number;
		//перевод строки
        fin.get();

        if( port_number<0 ){
			cerr<<"Ошибка : неверный номер порта "<<port_number<<endl;
            continue;
        }

		//получаем IP сервера
        hostent * record = gethostbyname(server);
        if( record==NULL ){
			cerr<<"Ошибка : не удалось определить адрес сервера "<<server<<':'<<port_number<<endl;
            continue;
        }

        in_addr * addressptr = (in_addr *) record->h_addr;
		//выделяем память под структуру с адресом
        sockaddr_in *server_info = (sockaddr_in*)malloc( sizeof(sockaddr_in) );

        if( server_info == NULL ){
			cerr<<"Ошибка : недостаточно памяти\n";
            return;
        }
        server_info->sin_family = AF_INET;
        server_info->sin_addr = * addressptr;
        server_info->sin_port = htons(port_number);

		//добавляем сервер в список
        serverlist.push_back(server_info);
	}
}

/**
  *диалог с 1им из серверов из списка
  *@param w_connection_client дескрипор файла для записи
  *соединения с клиентом
  *@param server_info адрес сервера
  *@param str строка для поиска
  *@param srvnum порядковый номер сервера в списке
  *@return none
  */
void server_dlg( FILE * w_connection_client, const sockaddr_in* server_info, const char *str, int srvnum ){
	//буферы приема и отпраки сообщений
    char buf[buf_size];
	char sndbuf[buf_size];

	//играем в клиента и подключаемся к серверу
	int dlg_socket = socket( AF_INET, SOCK_STREAM, 0 );
    if ( dlg_socket<0 ){
		cerr<<"Ошибка : не удалось создать сокет\n";
        _exit(1);
	}
    int r = connect( dlg_socket, (sockaddr*)server_info, sizeof(sockaddr_in) );
    if ( r<0 ){
		cerr<<"Ошибка : не удалось подключиться к серверу "
			<<inet_ntoa( server_info->sin_addr )<<':'
            <<server_info->sin_port<<endl;
            _exit(1);
    }

    FILE *r_connection = fdopen( dlg_socket, "r" );
    FILE *w_connection = fdopen( dlg_socket, "w" );

	//отправляем серверу строку
    fputs( str, w_connection );
    fflush(w_connection);

	//делаем в буфер отправки приписку о номере сервера
    sprintf( sndbuf, "Server%d:", srvnum );
    int len = strlen(sndbuf);

	//получаем ответы
    while( fgets( buf, buf_size, r_connection ) ){
		//дописываем ответ сервера после его номера
		strcpy( sndbuf+len, buf );
		//и отправляем клиенту
        fputs( sndbuf, w_connection_client );
        fflush( w_connection_client );
    }

    fclose(w_connection);
    fclose(r_connection);
    close(dlg_socket);
    _exit(0);
}


/**
  *диалог с подключившимся клиентом
  *@param session_socket сокет подключения
  *@return none
  */
void deal_with( int session_socket){
	char buf[buf_size];

	//регистрируем обработчик для SIGCHLD
    signal( SIGCHLD, sigchld_handler );

	FILE * r_connection=fdopen( session_socket, "r" );
	FILE * w_connection=fdopen( session_socket, "w" );

    if( r_connection==NULL || w_connection==NULL ){
		cerr<<"Ошибка : не удалось установить соединение\n";
		_exit(1);
	}

	//получаем от клиента строку
    if( fgets( buf, buf_size, r_connection ) == NULL ){
		cerr<<"Клиент неожиданно закрыл соединение\n";
		close(session_socket);
		_exit(1);
	}

	//запускаем по потомку для диалоги с каждым сервером из списка
    vector<sockaddr_in*>::const_iterator i = serverlist.begin();
    for( int temp; i<serverlist.end(); ++i )
		if( (temp=fork()) == 0 )
			server_dlg( w_connection, *i, buf, (int)(i-serverlist.begin()) );
        else if( temp == -1 ){
			cerr<<"Ошибка : не удалось создать новый процесс\n";
            _exit(1);
		}

	fclose(w_connection);
	fclose(r_connection);   
	close(session_socket);
    _exit(0);
}

int main(int argc, char *argv[])
{
	//регистрируем обработчик SIGCHLD
    signal( SIGCHLD, sigchld_handler );
	
	//должен быть задан номер порта 
	if ( argc!=2 ){
                cerr<<"Ошибка : не указан номер порта\n";
		exit(1);
	}
	int port_number = atol(argv[1]);

	//список серверов
	ifstream fin("serverlist.txt");  

	if( !fin ){
		cerr<<"Ошибка : не найден список серверов\n";
		exit(1);
	}
    //читаем списко серверов
    read_serverlist(fin);
	//закрываем файл
	fin.close();

	//создаем сокет
    int main_socket = socket( AF_INET, SOCK_STREAM, 0 );
	if( main_socket<0 ){
		cerr<<"Ошибка : не удалось создать сокет\n";
		exit(1);
	}

	//создаем структуру, описывающую сервер
	sockaddr_in server_info;
	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = htonl(INADDR_ANY);
	server_info.sin_port = htons(port_number);

	//связываем сокет и информацию о сервере
	int r1 = bind( main_socket, (sockaddr *) &server_info, sizeof(server_info) );
	if( r1<0 ){
		cerr<<"Ошибка : порт уже используется\n";
		close(main_socket);
		exit(1);
	}

	//начинаем прослушивать порт
	int r2 = listen( main_socket, 3 );
	if( r2<0 ){
		cerr<<"Ошибка : нет доступа к порту\n";
		close(main_socket);
		exit(1);
	} 

	//цикл обработки сообщений
	while(1){ 
		//информация о клиенте
		sockaddr_in client_info;
		unsigned int client_info_size = sizeof(client_info);
    
		//принимаем соединение
		int session_socket = accept( main_socket, (sockaddr *)&client_info, &client_info_size );
		if ( session_socket<0 ){
			usleep(100000);
			//а вдруг со следующего раза получится?
			continue;
		}

		//создаем потомка для общения с клиентом, а сами ждем новых подключений

        int temp=fork();
        switch( temp ){
        case 0:     deal_with( session_socket );
        case -1:    cerr<<"Ошибка : не удалось создать новый процесс\n"; exit(1);
        default:
                //deal_with( session_socket );
					close(session_socket);
                }
        }
	return 0;
}
