#pragma region Task description
/*
Два дочерних процесса выполняют некоторые циклы работ, передавая
после окончания очередного цикла через очередь сообщений родительскому
процессу очередные четыре строки некоторого стихотворения, при этом первый
процесс передает нечетные четырехстишья, второй - четные. Циклы работ
процессов не сбалансированы по времени. Родительский процесс компонует из
передаваемых фрагментов законченное стихотворение и выводит его по
завершении работы обоих процессов. Решить задачу с использованием аппарата
семафоров.
*/
#pragma endregion

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> // for errno
#include <string.h> // for strncpy()
#include "fork_thread.hpp"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>

#pragma region Global vars
#define BUFSIZE 64 // I think that's enough for my input
#define MSGTYPE_ANY 0
#define IPC_NOFLAGS 0
#pragma endregion

// Struct for child process execution data.
typedef struct {
	char *command; // this is for compability with param_start_t
	bool odd; // is process odd or not
	int semid, mqid; // semaphore and message queue IDs
} param_t;
// Struct for data to be sent via message queue.
typedef struct {
	long type; // this is for combatibility with POSIX syscalls
	char text[BUFSIZE]; // useful data
} msg;
const char *filename = "poetry.txt"; // file with text
msg mbuf; // temporary store of message queue record

// Semaphore-specific actions for System V: A(S, N), D(S, N), Z(S)
#define sem_a(sem_id,sb,i,n) sb.sem_num=(i); sb.sem_op=(n); semop(sem_id,&sb,1);
#define sem_d(sem_id,sb,i,n) sb.sem_num=(i); sb.sem_op=-(n); semop(sem_id,&sb,1);
#define sem_z(sem_id,sb,i) sb.sem_num=(i); sb.sem_op=0; semop(sem_id,&sb,1);

// Fill param_t with child process execution data.
void create_param(param_t *param, int sem_id, int mq_id, bool odd = true) {
	param->command = "process_poetry"; // this is safe to set -Wno-write-strings
	param->semid = sem_id;
	param->mqid = mq_id;
	param->odd = odd;
}

// Callback function for thread to process input text file.
int process_poetry(void *_param) {
	param_t *param = (param_t*)_param; // C style type conversion
	FILE *f = fopen(filename, "r"); // try to open poetry file
	if (f == NULL)
		return EXIT_FAILURE; // immediately fail on non-existent file
	char buf[BUFSIZE];
	int num = 0; // current line number
	// semaphore control
	struct sembuf sb;
	sb.sem_flg = IPC_NOFLAGS; // flags are usually unset
	mbuf.type = 1; // this MUST be positive, as said in man
	
	char *ptr = NULL;
	do {
		do {
			num++; // increase current line number
			ptr = fgets(buf, BUFSIZE - 1, f); // read line from file
			if (ptr == NULL) // stop on eof
				break;
			if ((num % 2) != param->odd) // skip not owned lines
				continue;
			/* используем два семафора. стартовое состояние: 0 1
			   первый процесс нечётный, второй чётный. 0 - доступно, 1 - заблокировано */
			sem_z(param->semid, sb, (short)param->odd); // wait till current semaphore is unblocked
			// send message
			strncpy(mbuf.text, buf, BUFSIZE - 1);
			mbuf.text[strlen(buf) - 1] = '\0'; // remove line ending
			if (msgsnd(param->mqid, &mbuf, BUFSIZE, IPC_NOFLAGS) == -1) // try to send via mq
				return errno; // immediately fail on mq-errors
			sem_a(param->semid, sb, (short)param->odd, 1); // block itself
			sem_d(param->semid, sb, (short)!param->odd, 1); // unblock partner (another thread)
		} while ((num % 4) != 0); // break every 4 lines
		fgets(buf, BUFSIZE - 1, f); // read separator line
	} while (ptr != NULL); // stop on eof
	return EXIT_SUCCESS;
}

// Main entry point of application.
int main(int argc, char **argv) {
	//key_t mk = ftok("lab7_proc_queue", 0);
	key_t mk = IPC_PRIVATE; // use private key to simplify task
	if (mk == -1) {
		printf("Could not get IPC key\n");
		return EXIT_FAILURE;
	}
	int mq = msgget(mk, IPC_CREAT | 0600);
	if (mq == -1) {
		printf("Could not get message queue\n");
		return EXIT_FAILURE;
	}
	int sid = semget(mk, 2, IPC_CREAT | 0600); // get two semaphores
	if (sid == -1) {
		printf("Could not get semaphore set\n");
		return EXIT_FAILURE;
	}
	struct sembuf sb;
	sb.sem_flg = IPC_NOFLAGS; // don't use flags
	sem_a(sid, sb, 0, 1); // increase semaphore 0 value to 1 (init state: 0 1)
	param_t *param = (param_t*)malloc(sizeof(param_t)); // C style
	create_param(param, sid, mq, true); // init odd thread
	fork_thread(process_poetry, (void*)param); // run odd thread
	create_param(param, sid, mq, false); // init non-odd thread
	fork_thread(process_poetry, (void*)param); // run non-odd thread
	join_threads(); // wait for children to do all the hard work
	
	ssize_t cnt;
	do {
		cnt = msgrcv(mq, &mbuf, BUFSIZE, MSGTYPE_ANY, IPC_NOWAIT);
	} while (cnt != -1 && printf("%s\n", mbuf.text));
	
	return EXIT_SUCCESS;
}