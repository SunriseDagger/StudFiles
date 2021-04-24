#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define RD 0 // Read end of pipe
#define WR 1 // Write end of pipe
#define FORK_ERROR 1 // Exit status for failed fork()
#define DATA_ERROR 2

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
	param_t *param,
	char *command,
	char *arg1,
	char *arg2 = NULL, 
	int out = STDOUT_FILENO,
	int err = STDERR_FILENO,
	int in = STDIN_FILENO) {
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
	switch (childpid = fork()) {
	case -1:
		fprintf(stderr, "[shell]\t Could not fork to subcommand '%s'\n", ((param_t*)param)->command);
		exit(FORK_ERROR);
		break;
	case 0:
		exit(func((param_t*)param)); // run callback and exit
		break;
	default:
		fprintf(stderr, "[shell]\t Process %d created for '%s'\n", childpid, ((param_t*)param)->command);
	}
	return childpid;
}

// Wait till ALL running subcommands end
int join_threads() {
	pid_t wait_pid;
	wait_t wait_ret;
	int exec_result = 0;
	while ((wait_pid = wait(&wait_ret)) > 0) { 
		printf("[shell]\t Process %d exited with code %d\n", wait_pid, wait_ret.code);
		exec_result |= wait_ret.code; // OR all codes
	}
	return exec_result;
}

int main(int argc, char **argv) {
	param_t param;
	int a = open("a.txt", O_RDONLY); // if file doesn't exists, returns -1
	if (a == -1) {
		fprintf(stderr, "[MAIN]\t Could not open file 'a.txt'\n");
		return DATA_ERROR;
	}
	int pipes[2], pipestwo[2];
	pipe(pipes);
	fprintf(stderr, "[MAIN]\t First pipe was opened.\n");
	pipe(pipestwo);
	fprintf(stderr, "[MAIN]\t Second pipe was opened.\n");
	int ret;
	create_startup_param(&param, "tr", "-d", "[0-9]", pipes[WR], STDERR_FILENO, a);
	fork_thread(shell_exec, &param);
	fprintf(stderr, "[MAIN]\t tr -d [0-9] completed.\n");
	if((ret = join_threads()) != 0)
		return ret;
	close(pipes[WR]);
	fprintf(stderr, "[MAIN]\t First pipe was closed for writing.\n");
	create_startup_param(&param, "sort", NULL, NULL, pipestwo[WR], STDERR_FILENO, pipes[RD]);
	fork_thread(shell_exec, &param);
	fprintf(stderr, "[MAIN]\t sort completed.\n");
	join_threads();
	close(pipestwo[WR]);
	fprintf(stderr, "[MAIN]\t Second pipe was closed for writing.\n");
	int b = open("b.txt", O_WRONLY | O_CREAT, 0600);
	if (b == -1) {
		fprintf(stderr, "[MAIN]\t Could not open file 'b.txt'\n");
		return DATA_ERROR;
	}
	create_startup_param(&param, "uniq", NULL, NULL, b, STDERR_FILENO, pipestwo[RD]);
	fork_thread(shell_exec, &param);
	fprintf(stderr, "[MAIN]\t uniq completed, result is in 'b.txt'.\n");
	return join_threads();
}