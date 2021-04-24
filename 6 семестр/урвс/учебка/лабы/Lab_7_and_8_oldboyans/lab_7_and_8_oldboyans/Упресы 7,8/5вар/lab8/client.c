#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>   
int s;
void kill_socket()
 {
  printf("Сервер разорвал соединение\n");
  close(s);
 }

int main(int argc, char *argv)
{
 struct sockaddr_in serv_addr;
 struct in_addr addr;

 char ch;
 int tmp, tmp_1, i;
 signal(SIGPIPE, kill_socket);
  addr.s_addr = INADDR_ANY;
  serv_addr.sin_family=AF_INET;			//настройки параметров сервера
  serv_addr.sin_addr.s_addr=inet_addr("217.71.130.131");
  serv_addr.sin_port=htons(1500);

  if((s=socket(AF_INET,SOCK_STREAM,0))==-1)	//открываем сокет
  {
   printf("Ошибка: не могу открыть сокет!\n");
   return 1;
  }

  if(connect(s,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in))==-1)
  {
   printf("Ошибка: не могу подключиться к серверу!\n");
   return 1;
  }
   printf("Соединение с сервером установлено!\n");

   if(read(s,&tmp,4)>0)
   {
    printf("\tДобро пожаловать на наш сервер!\n");
    printf("Сейчас подключено пользователей: %d\n", tmp+1);
   }
    for (i=0;i<=tmp;i++)
    {
	read(s,&tmp_1,4);
	printf("%d. %s\n",i+1, inet_ntoa(tmp_1));
    }
    while (read(s,&tmp,4))
    {
	printf("Клиент с %s подключился к серверу! Поприветствуем!\n",inet_ntoa(tmp));
    }
    
    close(s);
  return 0;
}

 
