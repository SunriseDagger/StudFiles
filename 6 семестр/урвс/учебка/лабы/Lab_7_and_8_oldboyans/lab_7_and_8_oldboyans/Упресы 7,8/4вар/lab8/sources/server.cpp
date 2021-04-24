/**
  *программа принимает от клиента строку, осуществляет посик
  *файлов, содержащих данную строку
  *и отправляет обратно клиенту имена файлов
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
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>

//размер буфера дял отправки/приема сообщений
#define buf_size 1024

using namespace std;

extern int errno;

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
  *поиск файлов, содержащих определенную строку
  *@param fdw дескриптор временного файла
  *@param str строка для поиска
  *@return none
  */
void search( int fdw, const char* str ){
	//подменяем стандартный вывод файлом
    close(1);
    fcntl( fdw, F_DUPFD, 1 );
    close(fdw);
	//запускаем скрипт для поска
    if( execlp( "sh", "sh", "search.sh", str, (char*)0 ) <0 )
		_exit(1);
    _exit(0);
}

/**
  *диалог с подключившимся клиентом
  *@param session_socket сокет подключения
  *@param session_number номер подключения
  *@return none
  */
void deal_with( int session_socket, int session_number ){
	char buf[buf_size];
	//имя временного файла
    char temp_file_name[buf_size];

	//создаем имя по формату tempfile%номер подключения%
	sprintf( temp_file_name, "%s%d", "tempfile", session_number );

	FILE * r_connection=fdopen( session_socket, "r" );
	FILE * w_connection=fdopen( session_socket, "w" );

	if( r_connection==NULL || w_connection==NULL ){
		cerr<<"Ошибка : не удалось установить соединение\n";
		_exit(1);
	}

	if( fgets( buf, buf_size, r_connection ) == NULL ){
		cerr<<"Клиент неожиданно закрыл соединение\n";
		close(session_socket);
		_exit(1);
	}

	//создаем временный файл
	int tempfdw=open( temp_file_name, O_CREAT | O_TRUNC | O_WRONLY, 0644 );
    if( tempfdw<0 ){
		cerr<<"Ошибка : не удалось создать временный файл\n";
		close(session_socket);
        _exit(1);
     }

	//создаем новый процесс для поиска
	int temp=fork();
    switch ( temp ){
    case 0:
		close(session_socket);
        search(tempfdw,buf);
    case -1:
        cerr<<"Ошибка : не удалось создать новый процесс (sh)\n";
        close(session_socket);
        _exit(1);
    default:
        close(tempfdw);
		//ждем завершения поиска
        waitpid( temp, &temp, WUNTRACED );
    }

	//открываем временный файл для чтения
    FILE *tempfdr=fopen( temp_file_name, "r" );
    if( tempfdr == NULL ){
		cerr<<"Ошибка : не удалось открыть временный файл\n";
        close(session_socket);
        _exit(1);
    }

	//отправляем все его содержимое клиенту
    while( fgets( buf, buf_size, tempfdr ) ){
		fputs( buf, w_connection );
        fflush( w_connection );
	}

	fclose(w_connection);
    fclose(r_connection);
    fclose(tempfdr);
	close(session_socket);
	//удаляем временный файл
    unlink(temp_file_name);
    _exit(0);
}

int main(int argc, char *argv[])
{
	//регистрируем обработчик SIGCHLD
    signal( SIGCHLD, sigchld_handler );

	//должен быть указан номер порта
	if ( argc!=2 ){
        cerr<<"Ошибка : не указан номер порта\n";
		exit(1);
	}
	int port_number = atol(argv[1]);

    if( port_number<0 ){
		cerr<<"Ошибка : неверное значение порта\n";
        exit(1);
    }

	//создаем сокет
    int main_socket = socket( AF_INET, SOCK_STREAM, 0 );
	if( main_socket<0 ){
		cerr<<"Ошибка : не удалось создать сокет\n";
		exit(1);
	}

	//создаем структуру для описания сервера
	sockaddr_in server_info;
	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = htonl(INADDR_ANY);
	server_info.sin_port = htons(port_number);

	//связываем сокет с сервером
	int r1 = bind( main_socket, (sockaddr *) &server_info, sizeof(server_info) );
	if( r1<0 ){
		cerr<<"Ошибка : порт уже используется\n";
		close(main_socket);
		exit(1);
	}

	//начинаем прослушивать порт
	int r2 = listen(main_socket, 3);
	if( r2<0 ){
		cerr<<"Ошибка : нет доступа к порту\n";
		close(main_socket);
		exit(1);
	} 

	//номер подключения
    int session_number=0;
	//цикл обработки сообщений
	while(1){ 
		//описание клиента
		sockaddr_in client_info;
		unsigned int client_info_size = sizeof(client_info);
    
		//принимаем подключение
		int session_socket = accept( main_socket, (sockaddr *)&client_info, &client_info_size );
		if ( session_socket<0 ){
			usleep(100000);
			//ждем новых подключений
			continue;
		}

		//создаем потомка для общения с клиентом, а смаи ждем новых подключений
		int temp=fork();
		switch( temp ){
        case 0:     deal_with( session_socket, ++session_number );
        case -1:    cerr<<"Ошибка : не удалось создать новый процесс\n"; exit(1);
        default:    close(session_socket);
        }
	}
	return 0;
}
