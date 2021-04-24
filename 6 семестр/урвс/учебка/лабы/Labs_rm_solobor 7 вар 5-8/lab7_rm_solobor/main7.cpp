#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>

#define FORK_ERROR 1 // Exit status for failed fork()
#define DATA_ERROR 2
#define IPC_NOFLAGS 0
#define MSGTYPE_ANY 0
#define CMD_LENGTH 19
#define ITER 3
#define BUFSIZE 128

// Semaphore-specific actions for System V: A(S, N), D(S, N), Z(S)
#define sem_a(sem_id,sb,i,n) sb.sem_num=(i); sb.sem_op=(n); semop(sem_id,&sb,1);
#define sem_d(sem_id,sb,i,n) sb.sem_num=(i); sb.sem_op=-(n); semop(sem_id,&sb,1);
#define sem_z(sem_id,sb,i) sb.sem_num=(i); sb.sem_op=0; semop(sem_id,&sb,1);

const char labs_list[][CMD_LENGTH] = {
	"./lab5_rm_solobor",
	"./lab6_rm_solobor"
};
const int count = sizeof(labs_list) / CMD_LENGTH;

typedef unsigned char byte; // this is missing in standard libs for whatever reason.
typedef struct { byte flags; byte code; } wait_t; // union for wait result
// process creation parameters
typedef struct {
	int shmd, semid, mqid, num;
	//int in, out, err; // descriptors of standard streams
} param_t;
typedef struct { short pid, num; } match_t;
// Struct for data to be sent via message queue.
typedef struct {
	long type; // this is for combatibility with POSIX syscalls
	char text[BUFSIZE]; // useful data
} msg_t;

struct sembuf sb;
msg_t mbuf;

// Fill param_t with process creation data.
void create_startup_param(param_t *param, int shmd, int semid, int mqid, int num) {
	param->shmd = shmd;
	param->semid = semid;
	param->mqid = mqid;
	param->num = num;
}

// Overlays current executing process with COMMAND.
int shell_exec(param_t *param) {
	void *memseg = shmat(param->shmd, NULL, IPC_NOFLAGS);
	// check
	sem_z(param->semid, sb, param->num); // wait till unlock
	sem_a(param->semid, sb, param->num, 1);
	void *cmd = memseg;
	cmd = (char*)cmd + param->num * CMD_LENGTH; // move pointer to next string beginning
	mbuf.type = 1; // this MUST be positive, as said in man
	sprintf(mbuf.text, "Running program '%s' as pid = %d", (char*)cmd, getpid());
	if(msgsnd(param->mqid, &mbuf, BUFSIZE, IPC_NOFLAGS) == -1) // try to send via mq
		return errno; // immediately fail on mq-errors
	int ret = execlp((char*)cmd, (char*)cmd, NULL);
	return ((ret == -1) ? -1 : 0); // return -1 on failure, 0 otherwise
}

// Create new thread in new process via fork() and run callback in it.
pid_t fork_thread(int(func(param_t *param)), void *param = NULL) {
	int childpid;
	switch (childpid = fork()) {
	case -1:
		fprintf(stderr, "[shell]\t Could not fork to subcommand\n");
		exit(FORK_ERROR);
		break;
	case 0:
		exit(func((param_t*)param)); // run callback and exit
		break;
	default:
		fprintf(stderr, "[shell]\t Process %d created for task #%d\n", childpid, ((param_t*)param)->num);
	}
	return childpid;
}

int main(int argc, char **argv) {
	srand(time(NULL));
	match_t match[ITER];
	memset(&match, 0, sizeof(match)); // zero memory
	key_t mk = IPC_PRIVATE; // use private key to simplify task
	if (mk == -1) {
		printf("Could not get IPC key\n");
		return EXIT_FAILURE;
	}
	int shmd = shmget(mk, sizeof(labs_list), IPC_CREAT | 0600);
	if (shmd == -1) {
		printf("Could not get shared memory\n");
		return EXIT_FAILURE;
	}
	int mq = msgget(mk, IPC_CREAT | 0600);
	if (mq == -1) {
		printf("Could not get message queue\n");
		return EXIT_FAILURE;
	}
	int sid = semget(mk, count, IPC_CREAT | 0600); // get two semaphores
	if (sid == -1) {
		printf("Could not get semaphore set\n");
		return EXIT_FAILURE;
	}
	
	void *memseg = shmat(shmd, NULL, IPC_NOFLAGS);
	memcpy(memseg, labs_list, sizeof(labs_list));
	
	sb.sem_flg = IPC_NOFLAGS; // don't use flags
	
	// run there
	for (int i = 0; i < ITER; i++) {
		int num;
		param_t param;
		do {
			num = rand() % count;
		} while (false); // while num is not available
		create_startup_param(&param, shmd, sid, mq, num);
		match[i].num = num;
		match[i].pid = fork_thread(shell_exec, &param);
	}
	
	// after run
	pid_t wait_pid;
	wait_t wait_ret;
	int exec_result = 0;
	while ((wait_pid = wait(&wait_ret)) > 0) {
		for (int i = 0; i < count; i++)
			if (match[i].pid == wait_pid) { // unlock semaphore
				match[i].pid = 0;
				sem_d(sid, sb, match[i].num, 1);
				break;
			}
		fprintf(stderr, "[shell]\t Process %d exited with code %d\n", wait_pid, wait_ret.code);
		exec_result |= wait_ret.code; // OR all codes
	}
	
	printf("Messages from queue:\n");
	ssize_t cnt;
	do {
		cnt = msgrcv(mq, &mbuf, BUFSIZE, MSGTYPE_ANY, IPC_NOWAIT);
	} while (cnt != -1 && printf("%s\n", mbuf.text));
	return exec_result;
}