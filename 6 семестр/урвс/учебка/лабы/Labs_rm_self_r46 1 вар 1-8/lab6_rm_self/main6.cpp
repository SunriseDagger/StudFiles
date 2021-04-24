#pragma region Task description
/*
Составить программу, моделирующую работу Shell-интерпретатора при
обработке командной строки, указанной в варианте. При реализации программы
путем выдачи сообщений информировать обо всех этапах ее работы (создан
процесс, выполнение команды закончено и т.д.).
Вариант 1: 
	(cc pr1.c & cc pr2.c) && cat pr1.c pr2.c > prall.c
*/
#pragma endregion

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#pragma region Global vars
#define RD 0 // Read end of pipe
#define WR 1 // Write end of pipe
#define FORK_ERROR 1 // Exit status for failed fork()
#define DATA_ERROR 2
#pragma endregion

typedef unsigned char byte; // this is missing in standard libs for whatever reason.
typedef struct { byte flags; byte code; } wait_t; // union for wait result
// process creation parameters
typedef struct {
	char *command; // executable file to be found in PATH
	char *argv0; // argv[0] string, required for exec-() functions
	char *arg1, *arg2; // first arg, second arg. no more due to complexity
	int in, out, err; // descriptors of standard streams
} param_t;

// Fill param_t with process creation data.
void create_startup_param(
	param_t *param, char *command, char *arg1, char *arg2 = NULL, 
	int out = STDOUT_FILENO, int err = STDERR_FILENO, int in = STDIN_FILENO
	) {
	param->command = command;
	param->argv0 = command;
	param->arg1 = arg1;
	param->arg2 = arg2;
	
	param->in = in;
	param->out = out;
	param->err = err;
}

// Overlays current executing process with COMMAND.
int shell_exec(param_t *param) {
	dup2(param->out, STDOUT_FILENO); // this is identical to fcntl(param->out, F_DUPFD, 1)
	dup2(param->err, STDERR_FILENO);
	dup2(param->in, STDIN_FILENO);
	int ret = execlp(param->command, param->argv0, param->arg1, param->arg2, NULL);
	return ((ret == -1) ? -1 : 0); // return -1 on failure, 0 otherwise
}

// Create new thread in new process via fork() and run callback in it.
pid_t fork_thread(int(func(param_t *param)), void *param = NULL) {
	int childpid;
	switch(childpid = fork()) {
		case -1:
			fprintf(stderr, "[shell] Could not fork to subcommand '%s'\n", ((param_t*)param)->command);
			exit(FORK_ERROR);
		break;
		case 0:
			exit(func((param_t*)param)); // run callback and exit
		break;
		default:
			fprintf(stderr, "[shell] Process %d created for '%s'\n", childpid, ((param_t*)param)->command);
			return childpid;
		break;
	}
}

// Wait till ALL running subcommands end
int join_threads() {
	pid_t wait_pid;
	wait_t wait_ret;
	int exec_result = 0;
	while((wait_pid = wait(&wait_ret)) > 0) { 
		printf("[shell] Process %d exited with code %d\n", wait_pid, wait_ret.code);
		exec_result |= wait_ret.code; // OR all codes
	}
	return exec_result;
}

// Main entry point of application.
int main(int argc, char **argv) {
	// We can initialize this struct only once due to fork() behaviour,
	// which creates a COPY of process, so memory isn't shared.
	// This is why I've set -Wno-write-strings option to gcc.
	param_t *param = (param_t*)malloc(sizeof(param_t));
	int exec_result;

	// Part 1. `cc pr1.c & cc pr2.c` (async run)
	create_startup_param(param, "cc", "pr1.c"); // cc pr1.c
	fork_thread(shell_exec, param); // run command
	create_startup_param(param, "cc", "pr2.c"); // cc pr2.c
	fork_thread(shell_exec, param); // run command
	if((exec_result = join_threads()) != 0) // if any of subcommands failed
		return exec_result; // && cat ...

	// Part 2. `cat pr1.c pr2.c > prall.c`
	int out = open("prall.c", O_WRONLY | O_CREAT, 0700); // open/create output file
	if(out == -1) { // if attempt to open file has failed
		fprintf(stderr, "[shell] Could not open file 'prall.c'\n");
		exit(DATA_ERROR);
	}
	create_startup_param(param, "cat", "pr1.c", "pr2.c", out); // cat pr1.c pr2.c > prall.c
	fork_thread(shell_exec, param); // run command

	return join_threads(); // exit with return code of last task
}