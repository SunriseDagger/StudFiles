#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<signal.h>
char sig1=1, sig2=1;
int ka[2];
float fun(float x)
{ return 4.0/(1.0+x*x); }
void sign_fun1(int sig)
{ char arg[10]; 
  int pid;
  sig1 = 0;
  sprintf(arg,"%d",ka[1]);
  pid = fork();
  if(pid == 0) execl("./pot","./pot",arg,0);
}
void sign_fun2(int sig){ sig2 = 0; }
void potomok()                           // �������
{ printf("�������� ������� ��� ������\n");
  signal(SIGUSR1, sign_fun1);            // ��������� ������� 1 
  while(sig1)sleep(1);
  printf("�������� ������� ��� ����������\n");
  signal(SIGUSR2, sign_fun2);            // ��������� ������� 2 
  while(sig2)sleep(1);
  exit(0);
}
void roditel(int pid)                    // ��������
{ int status, rea, i, j;
  unsigned int n;
  float po[1000], max;
  sleep(2);                              // ������� 2 ���.
  kill(pid,SIGUSR1);                     // ������� ������� ��� ������
  sleep(10);                             // �������� 10 ���.
  kill(pid,SIGUSR2);                     // ������� ������� ��� ����������
  wait(&status);                         // �������� ���������� �������
  if(!status)                            // ���� �� ���������� �������, ��...
  { rea = read(ka[0],&n,sizeof(unsigned int));// ������ N
    if(rea == sizeof(unsigned int))           // ���� ������ �������, ��...
    { rea = read(ka[0],po,n*sizeof(float));     // ������ ������������������
      if(rea == n*sizeof(float))                // ���� ������ �������, ��...
      { for(i=0; i<n-1; i++)                      // ������������� ������������������
          for(j=i+1; j<n; j++)
            if(po[i] > po[j])
            { max=po[i];
              po[i]=po[j];
              po[j]=max;
            }
        max=0;	                                  // ������� ��������
        for(i=0; i<n-1; i++)
          max+=(fun(po[i+1])+fun(po[i]))*(po[i+1]-po[i])/2.0; 	
        printf("�������� = %f\n",max);            // ����� ����������
      }
      else printf("������� ��������� ������!\n");    
    }
    else printf("������� ��������� N!\n");    
  }
}

void main ()
{ int pid;
  pipe(ka);
  pid=fork();
  if(pid==-1) printf("������� �� ������!\n");
  else
  { if(pid==0) potomok();
    else roditel(pid);
  }
  printf("\n");
}
