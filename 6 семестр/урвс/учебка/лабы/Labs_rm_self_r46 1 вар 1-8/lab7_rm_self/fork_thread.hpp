#pragma once

#pragma region Definition section - to be copied to main file on reporting
#define FORK_ERROR 1 // Exit status for failed fork()

// this is missing in standard libs for whatever reason.
typedef unsigned char byte;
// union for wait result
typedef struct { byte flags; byte code; } wait_t;
#pragma endregion

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Create new thread in new process via fork() and run callback in it.
pid_t fork_thread(int(func(void *param)), void *param = NULL);

// Wait till ALL running subcommands end
int join_threads();