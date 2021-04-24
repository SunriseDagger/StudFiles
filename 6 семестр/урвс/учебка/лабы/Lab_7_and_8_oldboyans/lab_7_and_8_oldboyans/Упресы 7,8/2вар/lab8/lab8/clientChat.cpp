#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

#define SIZE sizeof(struct sockaddr_in)
#define STR 128

struct buffer{
	char message[STR];
	short command;
} buf;
short buf_s = sizeof(struct buffer);

int main(){
	int flag, sockfd, m, pid[2], vybor;
	char buff[STR], buff2[STR];

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = 30231;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("CLIENT: Не могу создать сокет!\n");
		exit(1);
	}
	if(connect(sockfd,(struct sockaddr*)&server,SIZE) == -1){
		printf("CLIENT: Не могу подключиться к серверу!\n");
		exit(1);
	}

	printf("CLIENT: Я запущен!\n");
	buf.command = 0;
	printf("Ваше имя: ");
	scanf("%s", &buff[0]);
	printf("Вас зовут \"%s\"\n", buff);
	write(sockfd, &buff, STR);			// Передача имени клиента
	// Это слушатель - процесс, печатающий сообщения сервера.
	if((pid[0] = fork()) == -1){
		printf("CLIENT: Ошибка создания слушателя\n");
		exit(1);
	}
	else if(pid[0] == 0){
		while(1){
			if((m = read(sockfd, &buff, STR)) < 0){
				printf("CLIENT: Ошибка чтения сообщения из сокета!\n");
				exit(1);
			}
			if(m == 0){
				printf(" * Сервер отключен!\n");
				exit(0);
			}
			else printf("%s\n", buff);
		}
	}
	// А это говорун - процесс, передающий сообщения на сервер.
	if((pid[1] = fork()) == -1){
		printf("CLIENT: Ошибка создания говоруна\n");
		exit(1);
	}
	else if(pid[1] == 0){
		printf("Можете начинать беседу! (Введите bye для выхода)\n");
		while(1){
			scanf("%s", buf.message);
			if(strcmp(buf.message, "bye") == 0){
				buf.command = 1;
				write(sockfd, &buf, buf_s);
				break;
			}
			write(sockfd, &buf, buf_s);
		}
		kill(pid[0], SIGTERM);			// Остановить прослушивание сервера
		close(sockfd);				// Закрыть сокет
		exit(0);
	}
	// Завершение всех потомков (слушатель - pid[0] и говорун - pid[1])
	int 	procID,					/// Текущий завершившийся процесс
		procID2,				/// Оставшийся для завершения процесс
		status;					/// Статус окончания процесса
	// Первого:
	do{
		procID = wait(&status);
	}while(procID != pid[0] && procID != pid[1]);
	if(procID == pid[0]){
		printf("CLIENT: Завершен слушатель!\n");
		procID2 = pid[1];			// Осталось завершить говоруна
	}
	else{
		printf("CLIENT: Завершен говорун!\n");
		procID2 = pid[0];			// Осталось завершить слушателя
	}
	// И второго:
	do{
		procID = wait(&status);
	}while(procID != procID2);
	printf("CLIENT: Полностью завершен!\n");
	exit(0);
}
