#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<sys/types.h>
#include<errno.h>
#include <time.h>

#define ERR ((struct databuf *) -1)
#define FILE_NAME "./a1.txt"
#define FILE_NAME1 "./b1.txt"
#define FILE_NAME2 "./c1.txt"

char bbbuf[6]; 
//--------------------------------------------------------------------------------

struct databuf /* структура с данными и счетчиком чтения */
{
	char d_buf[4][20];
};

struct q_entry 
{
	long mtype;
	int pid;
	char mtext[20];
};

 union semun
   {
     int val;				
     struct semid_ds *buf;		
     unsigned short int *array;		
     struct seminfo *__buf;		
   };


int initsem (key_t semkey, int num)
{
	int status = 0, semid, i;
	union semun arg;
	if ((semid = semget (semkey, num, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
	{
		if (errno == EEXIST) /* если набор семафоров существовал ранее */
		semid = semget (semkey, num, 0); /* то открыть его */
	}
	if (semid == -1)
	return -1;
	return (semid);
}

int initmsg (key_t msgkey) /* функция инициализации очереди */
{
	int msgid;
	if ((msgid = msgget (msgkey, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
	{
		if (errno == EEXIST) /* если очередь существовала ранее */
		msgid = msgget (msgkey, 0); /* то открыть ее */
	}
	if (msgid == -1)
	return -1;
	return (msgid);
}

int getseg (key_t segkey, struct databuf **p)
{
	int segid;
	if ((segid = shmget (segkey, sizeof(struct databuf),0666 | IPC_CREAT |IPC_EXCL)) == -1)
	{
		if (errno == EEXIST) /* если сегмент существовал ранее */
		segid = shmget (segkey, sizeof(struct databuf), 0);/* то подключить его */
	}
	if (segid == -1)
	return -1;
	if ((*p = (struct databuf*) shmat (segid, 0, 0)) == ERR)
	return -1;
	return segid;
}

int p (int semid, int num)
{
	struct sembuf p_buf;
	p_buf.sem_num = num;
	p_buf.sem_op = -1;
	p_buf.sem_flg = 0;
	if (semop (semid, &p_buf, 1) == -1)
	{
		//printf ("error in p %d\n", semop (semid, &p_buf, 1));
		exit (1);
	}
	return 0;
}

int v (int semid, int num)
{
	struct sembuf p_buf;
	p_buf.sem_num = num;
	p_buf.sem_op = 1;
	p_buf.sem_flg = 0;
	if (semop (semid, &p_buf, 1) == -1)
	{
		//printf ("error in v %d\n", semop (semid, &p_buf, 1));
		exit (1);
	}
	return 0;
}
//-----------------------------------------------------------------------------------
void main()
{	int fd,j;
	key_t keysem=ftok(FILE_NAME,1),segkey=ftok(FILE_NAME1,1),keymsg=ftok(FILE_NAME2,3);
	int i, k, num, semid, msgid, segid, ret_code, status,lk;
	int pid[5];
	union semun arg; /* для инициализации семафора */
	struct msqid_ds msq_stat;struct shmid_ds shm_stat;
	struct databuf *buf;struct q_entry in_entry, out_entry;
	if ((semid = initsem (keysem, 4)) < 0)
	{
		printf ("semid= %d errno= %d\n", semid, errno);
	exit (1);
	}
	if ((msgid = initmsg (keymsg)) < 0)
	{
		printf ("msgid= %d\n", msgid);
		exit (1);
	}
	if ((segid = getseg (segkey, &buf)) < 0)
	exit (1);
	strcpy(buf->d_buf[0], "./lab5");		
	strcpy(buf->d_buf[1], "./lab6");
	strcpy(buf->d_buf[2], "./lab2");
	for (i = 0; i < 4; i++)
		{
		arg.val = 1; /* начальное значение семафора */
		status = semctl (semid, i, SETVAL, arg);
		}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
for(i=0;i<4;i++){
pid[i]=fork();
if(pid[i]==0)
	{
	  srand((unsigned)time(0)+i*20.0);
	  for(;;){k=(int)rand()%3;
		p(semid,i);
		if(fork()==0){
		 	out_entry.mtype=(long)1;	
		  	out_entry.pid=getppid();
		 	for(lk=0;lk<20;lk++)
		  	out_entry.mtext[lk]=buf->d_buf[k][lk];
		  	msgsnd(msgid, &out_entry , 20, 0);
		  	if(fork()==0)
				{
		  		execve(buf->d_buf[k],NULL,NULL);
				}
		 	 wait(&status); 
		  	out_entry.mtype=(long)1;
		  	out_entry.pid=getppid();
		  	out_entry.mtext[0]='e';
		  	out_entry.mtext[1]='n';
		  	out_entry.mtext[2]='d';
			out_entry.mtext[3]=' ';
		  	out_entry.mtext[4]=' ';
			out_entry.mtext[5]=' ';
		  	out_entry.mtext[6]='\0';
		  	msgsnd(msgid, &out_entry , 20, 0);
	           	v(semid,i);
			exit(0);
			 }
		}
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
	pid[4]=fork();
	if(pid[4]==0)		//обработка сообщений
	{	fd=open("2.txt",O_WRONLY|O_CREAT|O_TRUNC);
		for(;;){
		if ((status=msgrcv(msgid,&in_entry,20,1,0))==-1)
		 {
		     printf ("error of get message\n");
		     return -1;
	         }
	        else
	 	     {//for(j=0;j<20;j++)in_entry.mtext[j]='\0';
			printf("Quene Process [%d] execute programm %s\n",in_entry.pid,in_entry.mtext);
			sprintf(bbbuf,"\n%d",in_entry.pid);write(fd,bbbuf,6);write(fd," ",1);
			write(fd,in_entry.mtext,6);
			}
			
	        }
	}
	sleep(2);
	for(i=0;i<5;i++)
	kill(pid[i],9);
	printf("kill\n");
	shmdt ((void *) buf);
	status = semctl (semid, 0, IPC_RMID, arg);
	if (status == -1)
		printf ("error of RMD sem\n");
	status = msgctl (msgid, IPC_RMID, &msq_stat);
	if (status == -1)
		printf ("error of RMD mes\n");
	status = shmctl (segid, IPC_RMID, &shm_stat);
	if (status == -1)
		printf ("error of RMD seg1\n");
	exit(0);

}
