#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>   

#define MAX_USER 100

int s, ns;
struct client_info
{
    int socket_address;
    int socket_desc;
}clients[MAX_USER];

int main(int argc, char *argv[])
{

 struct sockaddr_in serv_addr, clnt_addr;
 struct in_addr addr;
 size_t length=sizeof(struct sockaddr_in);

 int cur_client_index=-1, i, tmp;

  serv_addr.sin_family=AF_INET;			//настройки параметров сервера
  serv_addr.sin_addr.s_addr= inet_addr("217.71.130.131");
  serv_addr.sin_port=htons(1500);
//  addr.s_addr = INADDR_ANY;
//  serv_addr.sin_addr = addr;

  if((s=socket(AF_INET,SOCK_STREAM,0))==-1)	//открываем сокет
  {
   printf("Ошибка: не могу открыть сокет!\n");
   return 1;
  }


  if(bind(s,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in))==-1) //связываем сокет и адрес машины
  {
   printf("Ошибка: не могу произвести связывание!\n");
   return 1;
  }

  if(listen(s,MAX_USER)==-1) //включение приема TCP-соединения
  {
   printf("Ошибка: не могу включить прием TCP-соединения!\n");
   return 1;
  }

 while ((ns=accept(s,(struct sockaddr *)&clnt_addr,&length))!=-1)
    {
       if(ns==-1) //прием запроса на установку соединения
        {
         printf("Ошибка: не могу получить запрос на установку соединения!\n");
         return 1;
        }
       printf("Приветствуем нового пользователя! %s\n", inet_ntoa(clnt_addr.sin_addr));

	for (i=0;i<=cur_client_index;i++)
	    write(clients[i].socket_desc,&clnt_addr.sin_addr.s_addr,4);
	
	tmp=cur_client_index;
	write(ns,&tmp,4);
	for (i=0;i<=cur_client_index;i++)
	    write(ns,&clients[i].socket_address,4);
	cur_client_index+=1;

	clients[cur_client_index].socket_desc = ns;
	clients[cur_client_index].socket_address = clnt_addr.sin_addr.s_addr;
    }

 close(s);
return 0;
}

 
