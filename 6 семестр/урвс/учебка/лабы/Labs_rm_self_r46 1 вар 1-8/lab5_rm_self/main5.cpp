#pragma region Task description
/*
1. Исходный процесс создает два программных канала К1 и К2 и порождает
новый процесс Р1, 
 а тот, в свою очередь, еще один процесс Р2, каждый из которых
 готовит данные для обработки их основным процессом. 
Подготавливаемые данные процесс Р1 помещает в канал К1, 
 а процесс Р2 в канал К2, откуда они
процессом Р1 копируются в канал К1 и дополняются новой порцией данных.

Обработка данных основным процессом заключается в чтении информации
из программного канала К1 и печати её. Кроме того, посредством выдачи
сообщений необходимо информировать обо всех этапах работы программы
(создание процесса, завершение посылки данных в канал и т.д.).
*/
#pragma endregion

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#pragma region Global vars
	#define RD 0 // Read end of pipe
	#define WR 1 // Write end of pipe
	#define DATA_LEN 40 // Data length by task
	#define FORK_ERROR 1 // Exit status for failed fork()
	#define DATA_ERROR 2 // Exit status for failed write()
#pragma endregion
typedef unsigned char byte; // this is missing in standard libs for whatever reason.
struct data_t { // struct for data by task
	pid_t pid;
	char data[DATA_LEN];
	data_t() { // default constructor
		pid = 0;
		memset(data, 0, DATA_LEN); // zero memory
	}
	data_t(pid_t _pid, const char *_data) { // constructor with data filling
		pid = _pid;
		strncpy(data, _data, DATA_LEN - 1); // memcpy for DATA_LEN is unsafe; for DATA_LEN - 1 should be ok
		data[DATA_LEN] = 0; // add string-terminator manually
	}
};

void send_message(const int _fd, const char *msg, pid_t pid = 0) {
	if(pid == 0) // if no sender is present
		pid = getpid(); // get current process ID
	data_t *data = new data_t(pid, msg); // how to replace it to C style?
	ssize_t c = write(_fd, data, sizeof(data_t)); // write data to pipe
	if(c == -1) { // write() returns -1 in case of failure
		fprintf(stderr, "[!] Data from %d couldn't be written to %d\n", pid, _fd);
		exit(DATA_ERROR); // finish current process
	}
	free(data); // C style
	return;
}

int main(int argc, char **argv) {
	// here we go, P0 process
	// piping by http://stackoverflow.com/a/26384903/1543625
    int K1[2]; // Child-to-parent pipe K1
	pipe(K1); // create one-way K1 pipe from P0 to P1
	// create K2
	int K2[2];
	pipe(K2); // create one-way K2 pipe from P1 to P2
	// common usage vars
	data_t buf;
	ssize_t c;
	pid_t wait_pid;
	int wait_ret;
	// signals by http://citforum.ru/programming/unix/signals/
	int sig;
	sigset_t sigset;
	sigemptyset(&sigset); // clear set of signals
	sigaddset(&sigset, SIGCONT); // add SIGCONT to set
	sigprocmask(SIG_BLOCK, &sigset, 0); // block all signals in set to all processes
	pid_t pid_p1; // Process ID of child
	switch(pid_p1 = fork()) { // do fork, pid_p1 contains new pid in parent, and 0 in child
		case -1: // error
			fprintf(stderr, "[!] Could not fork to P1\n");
			exit(FORK_ERROR);
		break;
		case 0: { // child P1
			close(K1[RD]); // this is recommended
			fprintf(stderr, "[i] Process P1 created\n");
			send_message(K1[WR], "Hey, what can I do for you?"); // send custom message
			fprintf(stderr, "[i] Data from P1 sent to K1\n");
			pid_t pid_p2; // identifier of child P2 process
			switch(pid_p2 = fork()) { // do fork, pid_p2 contains new pid in parent, and 0 in child
				case -1: // error in P1
					fprintf(stderr, "[!] Could not fork to P2\n");
					exit(FORK_ERROR);
				break;
				case 0:
					close(K2[RD]); // this is recommended
					fprintf(stderr, "[i] Process P2 created\n");
					while(sigwait(&sigset, &sig) < 0 && sig != SIGCONT); // wait for signal
					fprintf(stderr, "[i] SIGCONT received in P2\n");
					send_message(K2[WR], "I'm alive!"); // send custom message
					fprintf(stderr, "[i] Data from P2 sent to K2\n");
					kill(pid_p1, SIGCONT); // send signal to parent process, if I got it right by the task
					fprintf(stderr, "[i] Process P2 exiting\n");
					exit(EXIT_SUCCESS); // exit normally
				break;
			};
			// P1 code
			close(K2[WR]); // this is recommended
			kill(pid_p2, SIGCONT); // send signal to continue to child process
			fprintf(stderr, "[i] SIGCONT sent to P2\n");
			wait_pid = wait(&wait_ret); // wait till P2 die
			if(wait_ret != EXIT_SUCCESS) {// if child process exited with error
				fprintf(stderr, "[x] Leaving P1 with %d code\n", wait_ret);
				exit(wait_ret); // exit self with the same error
			}
			c = read(K2[RD], &buf, sizeof(data_t)); // read 1 message from K2
			c = write(K1[WR], &buf, sizeof(data_t)); // resend it to K1
			fprintf(stderr, "[i] Data from K2 re-sent to K1\n");
			// send custom message
			send_message(K1[WR], "I've done my best, good bye!");
			fprintf(stderr, "[i] Data from P1 sent to K1\n");
			fprintf(stderr, "[i] Process P1 exiting\n");
			exit(EXIT_SUCCESS); // exit normally
		}
		break;
	};
	// parent (P0) code
	close(K1[WR]); // this is recommended
	wait_pid = wait(&wait_ret); // wait till P1 die
	if(wait_ret != EXIT_SUCCESS) {// if child process exited with error
		fprintf(stderr, "[x] Leaving P0 with %d code\n", wait_ret);
		exit(wait_ret); // exit self with the same error
	}
	fprintf(stdout, "Messages queued in K1:\n");
	while((c = read(K1[RD], &buf, sizeof(data_t))) > 0) { // read each queued message
		fprintf(stdout, "Message from PID = %d: %s\n", buf.pid, buf.data);
	}
	return 0;
}