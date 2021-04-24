#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <errno.h>

void p0(int sem_id);
void v0(int sem_id);
void p1(int sem_id);
void v1(int sem_id);
