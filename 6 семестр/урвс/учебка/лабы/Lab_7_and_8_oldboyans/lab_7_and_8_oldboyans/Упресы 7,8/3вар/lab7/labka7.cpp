#include <iostream>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
using namespace std;

void proverka(int p, int key)
{
    if(p < 0)
    switch(key)
    {
	case 1:{ perror("oshibka SEMOP\n"); exit(1);}
	case 2:{ perror("oshibka FORK\n"); exit(1);}
	case 3:{ perror("oshibka SHMGET\n"); exit(1);}
	case 4:{ perror("oshibka SHMDT\n"); exit(1);}
	case 5:{ perror("oshibka SEMGET\n"); exit(1);}
	case 6:{ perror("oshibka SEMGET\n"); exit(1);}
	case 7:{ perror("oshibka SEMCTL\n"); exit(1);}
    }
}

void proverka_1(void * p)
{
    if((char *)p == (char *)-1) { perror("oshibka SHMAT\n"); exit(1);}
}
union semun 
{
	int val;
	struct semid_ds *mbuf;
	unsigned short *array;
};

void v(int semid, int sem, int shmid)
{
	struct sembuf v_buf;
	v_buf.sem_num = sem;
	v_buf.sem_op = 1;
	v_buf.sem_flg = 0;
	proverka (semop (semid,&v_buf,1), 1);
}

void p(int semid, int sem, int shmid)
{
	struct sembuf p_buf;
	p_buf.sem_num = sem;
	p_buf.sem_op = -1;
	p_buf.sem_flg = 0;
	proverka (semop (semid,&p_buf,1), 1);
}

int initsem()
{
	int status = 0, semid;
	union semun arg;
	arg.val = 0;
	proverka((semid = semget(IPC_PRIVATE,6,0600 | IPC_CREAT | IPC_EXCL)), 5);
	for (int i = 0; i < 6; i++)
	    proverka(semctl(semid,i,SETVAL,arg), 7);
	v(semid,0,0);
	v(semid,4,0);
	return semid;
}


void write_(int semid, char **str, int shmid, int number)
{
	char *adrr;
	int j = number + 1;
	if(number == 3) j = 0;
	proverka_1 ((adrr = (char *)shmat(shmid,0,0)));
	for (int i = 0; i < 3; i++)
	{
	    p(semid, number,0);
	    p(semid, 4,0);
	    strcpy (adrr, str[i]);
	    v(semid, j,0);
	    v(semid, 5,0);
	}
	proverka(shmdt(adrr), 4);
	exit(0);
}

int fork_(int shmid, char **str, int num, int semid)
{
    int f;
    proverka((f = fork()), 2);
    if (f == 0)
    {    
	write_(semid, str, shmid, num);
    }
    return f;
}

void read_(int semid, int shmid, char *addr)
{
    char  str1[90];
    p(semid, 5,shmid);
    strcpy (str1,addr);
    cout << str1;
    memset(addr,0,sizeof(addr));
    v(semid, 4,shmid);
}

int main ()
{
    int shmid, semid,pr1,pr2,pr3,pr4;
    char *addr, str1[90];
    char *str[4][3];
    str[0][0] = "Мы все учились понемногу\n";
    str[1][0] = "Чему-нибудь и как-нибудь,\n";
    str[2][0] = "Так воспитаньем, слава богу,\n";
    str[3][0] = "У нас немудрено блеснуть.\n";
    str[0][1] = "Онегин был по мненью многих\n";
    str[1][1] = "(Судей решительных и строгих)\n";
    str[2][1] = "Ученый малый, но педант:\n";
    str[3][1] = "Имел он счастливый талант\n";
    str[0][2] = "Без принужденья в разговоре\n";
    str[1][2] = "Коснуться до всего слегка,\n";
    str[2][2] = "С ученым видом знатока\n";
    str[3][2] = "Хранить молчанье в важном споре.\n";
    semid=initsem();
    proverka((shmid = shmget(IPC_PRIVATE,1024,IPC_CREAT|S_IRWXU|S_IRWXG|S_IRWXO)), 3);
    proverka_1((addr=(char *) shmat(shmid,0,0)));
    pr1 = fork_(shmid,str[0],0,semid);
    pr2 = fork_(shmid,str[1],1,semid);
    pr3 = fork_(shmid,str[2],2,semid);
    pr4 = fork_(shmid,str[3],3,semid);
    for (int i = 0; i < 12; i++)
    {
	read_(semid, shmid, addr);
    }
    proverka(shmdt(addr), 4);
    waitpid(pr1, NULL, 0);
    waitpid(pr1, NULL, 0);
    waitpid(pr1, NULL, 0);
    waitpid(pr1, NULL, 0);
    semctl(semid,0,IPC_RMID,NULL);
    return (0);
}