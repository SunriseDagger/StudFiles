#include "fork_thread.hpp"

// Create new thread in new process via fork() and run callback in it.
pid_t fork_thread(int(func(void *param)), void *param) {
	int childpid;
	switch (childpid = fork()) {
	case -1:
		fprintf(stderr, "[shell] Could not fork to subcommand '%s'\n", *(char**)param);
		exit(FORK_ERROR);
		break;
	case 0:
		exit(func(param)); // run callback and exit
		break;
	default:
		fprintf(stderr, "[shell] Process %d created for '%s'\n", childpid, *(char**)param);
		return childpid;
		break;
	}
}

// Wait till ALL running subcommands end
int join_threads() {
	pid_t wait_pid;
	wait_t wait_ret;
	int exec_result = 0;
	while ((wait_pid = wait(&wait_ret)) > 0) { 
		printf("[shell] Process %d exited with code %d\n", wait_pid, wait_ret.code);
		exec_result |= wait_ret.code; // OR all codes
	}
	return exec_result;
}