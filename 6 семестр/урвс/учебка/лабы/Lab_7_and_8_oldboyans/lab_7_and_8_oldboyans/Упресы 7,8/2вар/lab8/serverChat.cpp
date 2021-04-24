#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define STR 128
#define SZ_SOK sizeof(struct sockstruct)
#define SZ_SIN sizeof(struct sockaddr_in)
#define SZ_INT sizeof(int)

struct buffer{
	char message[STR];
	short command;
} buf;

short buf_s = sizeof(struct buffer);

struct socketClient{
	struct sockaddr_in sock;
	char nick[STR];
} Client;

void errorCl(int signo){
	printf("SERVER: Клиент отключен!\n");
	signal(SIGPIPE, errorCl);
}

int main(int argc, char * argv[]){
	int sockfd, ns[10], ch1[2], pid[11], k = 0, pid1;
	buf.command = 0;
	char buff[STR], buff2[STR], buff3[STR];
	struct sockaddr_in server;
	signal(SIGPIPE,&errorCl);

	server.sin_family = AF_INET;
	server.sin_port = 30231;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("SERVER: Не могу создать новый сокет!\n");
		exit(1);
	}
	if(bind(sockfd, (struct sockaddr*)&server, SZ_SIN) == -1){
		printf("SERVER: Ошибка привязки адреса!\n");
		exit(1);
	}
	if(listen(sockfd, 5) == -1){
		printf("SERVER: Ошибка прослушивания!\n");
		exit(1);
	}

	printf("SERVER: Я запущен!\n");
	printf("SERVER: Я жду подключений.\n");
	
	pipe(ch1);

	while(1){
		if((ns[k] = accept(sockfd, NULL, 0)) == -1){
			printf("SERVER: Ошибка приёма подключения\n");
			exit(1);
		}
		if(!recv(ns[k], &Client.nick, STR, 0))
			 printf("SERVER: Ошибка приема сообщения клиента!\n");
		buff3[0] = '\0';
		strcpy(buff3, Client.nick);
		strcat(buff3, " подключился ***\n");
		buff3[STR-1] = '\0';
		printf("%s\n",buff3);
		for(int i = 0; i <= k; i++)
			send(ns[i], &buff3, STR, 0);
		if((pid[k] = fork()) == -1){
			printf("SERVER: Ошибка создания потомка!\n");
			exit(1);
		}
		if(pid[k] == 0){
			while(!buf.command){
				if(recv(ns[k], &buf, buf_s, 0) > 0){
					if(!buf.command){
						strcpy(buff, Client.nick);
						strcat(buff, ": ");
						strcat(buff, buf.message);
						buff[STR-1] = '\0';
						printf("%s\n", buff);
						write(ch1[1], &buff, STR);
					}
					else{
						strcpy(buff, Client.nick);
						strcat(buff, " - отключен!\n");
						printf("%s\n", buff);
						write(ch1[1], &buff, STR);
					}
				}
				else{
					printf(" * обрыв соединения до клиента %s\n", Client.nick);
					break;
				}
			}
			close(ch1[1]);
			ns[k] = -1;
			exit(0);
		}
		if((pid1 = fork()) == -1){
			printf("SERVER: Ошибка создания потомка!\n");
			exit(1);
		}
		if(pid1 == 0){
			while(1){
				if(read(ch1[0], &buff2, STR) < 0) break;
				for(int j = 0; j <= k; j++)
					if(ns[j] > 0) send(ns[j], buff2, STR, 0);
			}
			exit(0);
		}
		k++;
	}
	//kill(pid1,SIGUSR1);
	close(sockfd);
	close(ch1[0]);
	close(ch1[1]);
	exit(0);
}
