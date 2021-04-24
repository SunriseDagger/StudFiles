#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;
sembuf sem_buf;			// for sem
/* structure to work with the queue */
struct Msg
{
	long mtype;			// type of message (priority)
	char mtext[10];		// text of message
};
/*
 * Function of using resourse
 * @num - number of semaphores
 * @semid - id of semaphore
 */
void p(int num,int semid)
{	sem_buf.sem_num=num;			
	sem_buf.sem_op=-1;
	semop(semid,&sem_buf,1);
}
/*
 * Function of unusing resourse
 * @num - number of semaphores
 * @semid - id of semaphore
 */
void v(int num,int semid)
{	sem_buf.sem_num=num;			
	sem_buf.sem_op=1;
	semop(semid,&sem_buf,1);
}

int main()
{
	key_t msgkey;			// key of quene
	key_t semkey;			// key of sem
	int msgqid;				// quene id
	int semid;				// sem id
	Msg mm;					// mm - message
	int pid[6];				// id of processes
	int *stat_loc, id;		// for wait()
	int n = 0;
	char files[6][10] = {"lab_1","lab_2","lab_3","lab_4","lab_5","lab_6"};
	msgkey = 10;
	semkey = 11;
	msgqid = msgget(msgkey,0666|IPC_CREAT|IPC_EXCL);	// create new quene
	semid = semget(semkey,1,0666|IPC_CREAT|IPC_EXCL);	// 	create new sem
	if (msgqid == -1) cout << "Error in creating new quene." << endl, exit(1);
	if (semid == -1) cout << "Error in creating new semaphore." << endl, exit(1);
	semctl(semid,0,SETVAL,3);
	/* first process */
	switch(pid[0]=fork())
	{
		case -1: 
		cout << "Error in creating first process" << endl, exit(1);
		case  0: 
		strncpy(mm.mtext,files[0],10);					// set name of file
        	mm.mtype=5l;								// set priority
		if (msgsnd(msgqid,&mm,10,IPC_NOWAIT)==-1)		// send the queue name of the file
		cout << "Error in sending 1 msg." <<endl, exit(1);
		exit(0); break;
	}
	/* second process */
	switch(pid[1]=fork())
	{
		case -1: 
		cout << "Error in creating second process" << endl, exit(1);
		case  0: 
		strncpy(mm.mtext,files[1],10);					// set name of file
        mm.mtype=4l;									// set priority
		if (msgsnd(msgqid,&mm,10,IPC_NOWAIT)==-1)		// send the queue name of the file
		cout << "Error in sending 2 msg." <<endl, exit(1);
		exit(0); break;
	}
	/* third process */
	switch(pid[2]=fork())
	{
		case -1: 
		cout << "Error in creating third process" << endl, exit(1);
		case  0: 
		strncpy(mm.mtext,files[2],10);					// set name of file
        mm.mtype=6l;									// set priority
		if (msgsnd(msgqid,&mm,10,IPC_NOWAIT)==-1)		// send the queue name of the file
		cout << "Error in sending 3 msg." <<endl, exit(1);
		exit(0); break;
	}
	/* fourth process */
	switch(pid[3]=fork())
	{
		case -1: 
		cout << "Error in creating fourth process" << endl, exit(1);
		case  0: 
		strncpy(mm.mtext,files[3],10);					// set name of file
        mm.mtype=1l;									// set priority
		if (msgsnd(msgqid,&mm,10,IPC_NOWAIT)==-1)		// send the queue name of the file
		cout << "Error in sending 4 msg." <<endl, exit(1);
		exit(0); break;
	}
	/* fifth process */
	switch(pid[4]=fork())
	{
		case -1: 
		cout << "Error in creating fifth process" << endl, exit(1);
		case  0: 
		strncpy(mm.mtext,files[4],10);					// set name of file
        mm.mtype=2l;									// set priority
		if (msgsnd(msgqid,&mm,10,IPC_NOWAIT)==-1)		// send the queue name of the file
		cout << "Error in sending 5 msg." <<endl, exit(1);
		exit(0); break;
	}
	/* sixth process */
	switch(pid[5]=fork())
	{
		case -1: 
		cout << "Error in creating sixth process" << endl, exit(1);
		case  0: 
		strncpy(mm.mtext,files[5],10);					// set name of file
        mm.mtype=3l;									// set priority
		if (msgsnd(msgqid,&mm,10,IPC_NOWAIT)==-1)		// send the queue name of the file
		cout << "Error in sending 6 msg." <<endl, exit(1);
		exit(0); break;
	}
	/* waiting of the exiting of all processes  */
	for (int i = 0; i < 6; i++)
		wait(stat_loc);
	cout << "Processes is completed." << endl;
	/* Reading messages on priority and starting labs*/
	for (long i = 6; i > 0; i--)
	{
		msgrcv(msgqid,&mm,10,i,IPC_NOWAIT|MSG_NOERROR);			// read from quene
		cout << "Running '" << mm.mtext << "' with priority " << mm.mtype << ".\n";
		switch(pid[i-1] = fork())
		{
			case -1: 
			cout << "Crash!" << endl, exit(1);
			case  0:
			p(0,semid);			//sem_val - 1
			cout << "Working " << mm.mtext << "...\n" ;
			sleep(1);			// do lab
			exit(0); break;		
		}
	}
	/* waiting of the exiting of all processes  */
	for (int i = 0; i < 6; i++)
	{
		wait(stat_loc);
		cout << "Lab is completed." << endl;
		v(0,semid);			//sem_val + 1
	}
	msgctl(msgqid,IPC_RMID,NULL);		// delete quene and all data in quene
	semctl(semid,1,IPC_RMID,NULL);		// delete sem 
}
