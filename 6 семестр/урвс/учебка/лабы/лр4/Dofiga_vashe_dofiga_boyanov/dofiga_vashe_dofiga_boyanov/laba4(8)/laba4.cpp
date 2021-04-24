#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>      
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<math.h>

int flag = 1; 

static void sig_start(int signo)
{flag = 0;}   

static void died_potomok(int signo)
{printf("potomok's died\n");}

main(int argc,char *argv[])
{ int pid,pid1,pid2,procNumber,i;
  int pipeFd[2];
  double rez,Expo = 1000,pi=1000,x;
  char buf[sizeof(int)+sizeof(double)];
  char argument[sizeof(int)+1];
  
  signal(SIGUSR1,sig_start);        
  signal(SIGINT,died_potomok);      

  if(argc != 2)  { printf("Use laba4 with 1 parametr - x\n");
		   exit(1);}

  x = atof(argv[1]); 		   
	      
  printf(" make a pipe\n");
  if(pipe(pipeFd) !=0) {printf("Can't build a pipe");
                        exit(1);}
			
  sprintf(argument,"%d",pipeFd[1]);
			
  printf("starting process1\n");
  pid = fork();          
  if(pid == -1) {printf(" Can't start process 1");
                  exit(1);}
  if(pid == 0) {
                 while(flag) pause; 
                 execlp("./pi","./pi",argument,NULL);
                 printf(" Can't execute my file");
                 exit(1);}
                 
   else pid1 = pid;  
                             
  printf(" starting process2\n");
  pid = fork();      
  if(pid == -1) {printf(" Can't start process 2");
                  exit(1);}
  if(pid == 0) {
                 while(flag) pause;
                 execlp("./Exponent","./Exponent",argv[1],argument,NULL);
                 printf(" Can't execute my file");
                 exit(1);}
   else pid2 = pid; 
   
  kill(pid1,SIGUSR1);        
  kill(pid2,SIGUSR1); 

  for(i=0;i<2;i++)
   {
    read(pipeFd[0],&buf,sizeof(int)+sizeof(double));
       
    procNumber = *((int *)buf);   
    rez = *((double *)(buf +sizeof(int)));
    if(procNumber == 1) pi  = rez;        
    else if(procNumber == 2) Expo = rez;
   }
  kill(pid1,SIGINT); 
  kill(pid2,SIGINT); 

  rez = (Expo) / (sqrt(pi*2.)); 
			
  printf(" f(%s) = %.10f\n",argv[1],rez); 
}
