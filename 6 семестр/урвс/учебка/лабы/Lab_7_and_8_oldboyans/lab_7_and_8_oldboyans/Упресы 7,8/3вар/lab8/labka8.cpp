#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dirent.h>
#include <errno.h>
using namespace std;

void proverka(int p, int key, struct sockaddr serv_1,  struct sockaddr serv_2)
{
    if(p < 0)
    switch(key)
    {
	case 1:{ perror("ошибка SOKET\n"); unlink (serv_1.sa_data); 
		 unlink (serv_2.sa_data); exit(1);}
	case 2:{ perror("ошибка FORK\n"); unlink (serv_1.sa_data); 
		 unlink (serv_2.sa_data); exit(1);}
	case 3:{ perror("ошибка CONNECT\n"); unlink (serv_1.sa_data); 
		 unlink (serv_2.sa_data);exit(1);}
	case 4:{ perror("ошибка SEND\n");  unlink (serv_1.sa_data); 
		 unlink (serv_2.sa_data); exit(1);}
	case 5:{ perror("ошибка RECV\n"); unlink (serv_1.sa_data); 
		 unlink (serv_2.sa_data); exit(1);}
	case 6:{ perror("ошибка BIND\n"); unlink (serv_1.sa_data); 
		 unlink (serv_2.sa_data); exit(1);}
	case 7:{ perror("ошибка LISTEN\n"); unlink (serv_1.sa_data);
		 unlink (serv_2.sa_data); exit(1);}
	case 8:{ perror("ошибка READ\n"); unlink (serv_1.sa_data);
		 unlink (serv_2.sa_data); exit(1);}
	case 9:{ perror("ошибка OPEN\n"); unlink (serv_1.sa_data);
		 unlink (serv_2.sa_data); exit(1);}
    }
}

int poisk(int serv, int sock_work, char *file, struct sockaddr &serv_1, struct sockaddr &serv_2)
{
    DIR *d;
    char str[10]=" ";
    char str1[10]=" ";
    int fd, flag = 0;
    struct stat ss;
    struct dirent *dir;
    if (serv == 1) strcpy(str, "server_11");
    if (serv == 2) strcpy(str, "server_22");
    d = opendir(str);
    dir = readdir(d);
    for(; dir!=NULL; dir=readdir(d))
    {
	 stat(dir->d_name, &ss);
	 if ((strcmp(dir -> d_name, file)) == 0)
	 {
	    flag = 1;
	    chdir(str);
	    proverka((fd = open(file, O_RDONLY)), 9, serv_1, serv_2);
	    proverka(read(fd, &str1, 10*sizeof(char)), 8, serv_1, serv_2);
	    proverka(send(sock_work, str1 , sizeof(str1), 0), 4, serv_1, serv_2);
	 }
    }
    strcpy(str, "~/xxx");
    chdir(str);
    return flag;
}

void client_(int number, char *file, struct sockaddr &server, struct sockaddr &server_)
{
    int sock,i,fd;
    char str[40] = " ";
    proverka((sock = socket (AF_UNIX, SOCK_STREAM, 0)), 1, server, server_);
    while (connect (sock, &server, sizeof(struct sockaddr)) == -1)
	cout <<"Ошибка подключения к серверу\n";
    proverka(send (sock, file, 40*sizeof(char), 0), 4, server, server_);
    while ((recv(sock, str, sizeof(str), 0))<0)
	cout<<"Сервер не отвечает. Ждём ответ\n";
    cout<<"Получен ответ от сервера\n";
    cout << str << "\n";
    close (sock);
    exit (0);
}
void server_(int number, struct sockaddr &serv_1, struct sockaddr &serv_2)
{
    int sock_serv, sock_work, pr, fd, n, st;
    char file[9]=" ";
    char er[43];
    strcpy(er, "Такого файла нет!!");
    proverka((sock_serv = socket(AF_UNIX,SOCK_STREAM,0)), 1, serv_1, serv_2);
    proverka((bind (sock_serv, &serv_1, sizeof(struct sockaddr))), 6, serv_1, serv_2);
    proverka(listen (sock_serv, 5), 7, serv_1, serv_2);
    for(;;)															
    {
        if ((sock_work = accept (sock_serv, NULL,NULL)) == -1)
        {
            cout << "Ошибка соединения\n";
            continue;
        }
	proverka((pr = fork()), 2, serv_1, serv_2);
	if (pr == 0)
	{
	    proverka (recv(sock_work, file, sizeof(file), 0), 5, serv_1, serv_2);
	    n = poisk(number, sock_work, file, serv_1, serv_2);
	    if (n) 
	    {
	        cout << "Файл на сервере №  " << number << "\n";
	        exit(0);
	    }
	    else
	    {
	        if (number == 1) client_(2, file, serv_2, serv_1);
	        else proverka(send (sock_work, er, 43, 0), 4, serv_1, serv_2);
	    }
	}	    
	break;
    }
    close(sock_work);
    wait(&st);
    close(sock_serv);
    exit(0);
}

int main (int argc, char **argv)
{
    int pr1, pr2, pr3, st;
    struct sockaddr server_1 = {AF_UNIX,"serve_1"};
    struct sockaddr server_2 = {AF_UNIX,"serve_2"};
    if (argc != 2)
    {
        perror ("Введите входные данные!!\n");
        unlink (server_1.sa_data);
        unlink (server_2.sa_data);
        exit(1);
    }
    proverka(pr1 = fork(), 2, server_1, server_2);
    if (pr1 == 0) server_(1, server_1, server_2);
    proverka(pr2 = fork(), 2, server_1, server_2);
    if (pr2 == 0) server_(2, server_2, server_1);
    proverka(pr3 = fork(), 2, server_1, server_2);
    if (pr3 == 0) client_(1, argv[1], server_1, server_2);
    wait(&st);
    wait(&st);
    kill (pr2, SIGKILL);
    wait(&st);
    unlink(server_1.sa_data);
    unlink(server_2.sa_data);
    return 0;
}
