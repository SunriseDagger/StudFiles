#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define RD 0
#define WR 1
#define BUFSIZE 512
typedef struct {
	int pid;
	char msg[40];
} msg_t;
const char *msg1 = "Surprise!";
const char *msg2 = " Where's my space?";

int main(int argc, char **argv) {
	msg_t M;
	int k1[2], k2[2];
	printf("Process MAIN has started.\n");
	pipe(k1);
	printf("MAIN:\tpipe K1 was created.\n");
	pipe(k2);
	printf("MAIN:\tpipe K2 was created.\n");
	switch (fork()) {
		case -1: {/*error*/} break;
		case 0: {//child
			printf("P1:\tprocess P1 has started.\n");
			while (read(k1[RD], &M, sizeof(M)) <= 0)
			printf("P1:\tprocess P2 has finished.\n");
			printf("P1:\tmessage was received.\n");
			strncat(M.msg, msg2, sizeof(M.msg));
			M.pid = getpid();
			write(k2[WR], &M, sizeof(M));
			printf("P1:\tmessage was resended.\n");
			exit(EXIT_SUCCESS); 
			} break;
		default: {/*parent*/} break;
	}
	switch(fork())
	{
		case -1: {/*error*/} break;
		case 0: { //child
			printf("P2:\tprocess P2 has started.\n");
			strncpy(M.msg, msg1, sizeof(M.msg));
			M.pid = getpid();
			write(k1[WR], &M, sizeof(M));
			printf("P2:\tmessage was sended.\n");
			exit(EXIT_SUCCESS);
			} break;
		default: {/*parent*/} break;
	}
	while (read(k2[RD], &M, sizeof(M)) <= 0)
	printf("MAIN:\tprocess P1 has finished.\n");
	printf("MAIN:\tReceived message [pid=%d]:\n%s\n", M.pid, M.msg);
	return 0;
}