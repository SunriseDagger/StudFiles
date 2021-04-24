#include<stdio.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<string.h>
#include<sys/wait.h>
#include<unistd.h>
#include<errno.h>
#include <stdlib.h>
#include "unit.h"

union semun 
{
 int val;                 
 struct semid_ds *buf;   
 unsigned short *array;
};

struct message                   //��������� ��� ������ � ��������
{ 
 long mtype;
 char mtext[120];                //����� ���������
}msg;

int p(int semid, int num) {	
  sembuf p_buf;
  p_buf.sem_num=num;		
  p_buf.sem_op=-1;
  p_buf.sem_flg=0;
  semop(semid,&p_buf,1);	
  return 0;
}

int v(int semid, int num) {	
  sembuf p_buf;
  p_buf.sem_num=num;		
  p_buf.sem_op=1;
  p_buf.sem_flg=0;
  semop(semid,&p_buf,1);	
  return 0;
}

int main(int argc, char *argv[])
 {
  key_t msgkey=0x200, semkey=0x201;   //�����
  int msgid, semid, i;
  union semun arg;
  int pId1, pId2;

 if(semid=semget(semkey,2,0666|IPC_CREAT|IPC_EXCL)==-1)  //������� ������� ���������
   {
    if(errno==EEXIST) semid=semget(semkey,2,0);             // ���� ��� ��� �������
     else{
          printf("�� ���������� ������� �������!\n");
          return 1;
         }
   }

 for(i=0; i<2; i++)
  {
   arg.val=0;
   semctl(semid,i,SETVAL,arg);
  }


  if(msgid=msgget(msgkey,0666|IPC_CREAT|IPC_EXCL)==-1)  //������� ������� ���������
   {
    if(errno==EEXIST) msgid=msgget(msgkey,0);             // ���� ��� ��� �������
     else{
          printf("�� ���������� ������� �������!\n");
          return 1;
         }
   }

 v(semid, 0);
 if(!(pId1=fork()))	 //��������� ����� �������
   {			
    int k=0;
    for(int i=0; i<4; i++)
    {
     p(semid, 0);
     sleep(rand()%4);
     strcpy(msg.mtext,out[k]);
     msg.mtype=1;
     k+=2;
     if(msgsnd(msgid,&msg,120,IPC_NOWAIT)==-1)  
      {
       printf("������ ��� ���������� � �������! 1\n");
       exit(1);
      }else{
             printf("������� 1 �������� ��������� �������!\n");
           } 
     v(semid, 1);
    }
    exit(0);
   }else if(pId1==-1){		//���� �������� �� ����������
                      printf("�� ���������� ������� ����� ������� 1!\n");
                      return 1;
                     }

 if(!(pId2=fork()))	 //��������� ����� �������
   {			
    int k=1;
    for(int i=0; i<3; i++)
    {
     p(semid, 1);
     sleep(rand()%6);
     strcpy(msg.mtext,out[k]);
     msg.mtype=2;
     k+=2;
     if(msgsnd(msgid,&msg,120,IPC_NOWAIT)==-1)  
      {
       printf("������ ��� ���������� � �������! 2\n");
       exit(1);
      }else{
             printf("������� 2 �������� ��������� �������!\n");
           }  
     v(semid, 0);
    }
    exit(0);
   }else if(pId2==-1){		//���� �������� �� ����������
                      printf("�� ���������� ������� ����� ������� 2!\n");
                      return 1;
                     }
 


int status; wait(&status); wait(&status); //���� ���������� ���� ��������
ph[0]=ph[1]=NULL; //������� ������ ������
int tmp;
stih *tmp_st;

for(int i=0; i<7; i++)
{
 if((msgrcv(msgid,&msg,120,0,IPC_NOWAIT|MSG_NOERROR))>=0)
  {
   tmp=msg.mtype-1;
    if(ph[tmp]==NULL)
     {
      ph[tmp]=new stih; 
      strcpy(ph[tmp]->txt,msg.mtext); //=msg.mtext;
      ph[tmp]->next=NULL;
     }else{
  	    for(tmp_st=ph[tmp]; tmp_st->next!=NULL; tmp_st=tmp_st->next); //����� �� ����� ������
	    tmp_st->next=new stih; tmp_st=tmp_st->next;  //������� ����� ����� � �����
            strcpy(tmp_st->txt,msg.mtext);	 
            tmp_st->next=NULL;
          }
  }
}

for(stih *st0=ph[0], *st1=ph[1]; st0!=NULL || st1!=NULL;)
 {
  if(st0!=NULL)
   {
    printf("%s\n", st0->txt);
    st0=st0->next;
   }
  if(st1!=NULL)
   {
    printf("%s\n", st1->txt);
    st1=st1->next;
   }
 }

  msgctl(msgid,IPC_RMID,0);           //������� ������� ���������
  semctl(semid,IPC_RMID,0);
return 0;
}

