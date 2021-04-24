#include<stdio.h>
#include<signal.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
static void func(int signo)
{
 signal(SIGUSR1,func);
 printf("Have signal USR1\n");
 };
 
int main()
{ int r,n[10],i,s,s1;
  int fd[2]; 
  pid_t pid[10];
  char *buf1;
  
  signal(SIGUSR1,func); 
  
  printf("Please input N: "); 
  scanf("%d",&n[0]);
  if((n[0]<0)||(n[0]>30))
  {
   printf("\nWrong parameters!!!\n");
   exit(0);}
  printf("Please input R: ");
  scanf("%d",&r);
   if((r<=0)||(r>20))
  {
   printf("\nWrong parameters!!!\n");
   exit(0);}
 
  n[r+1]=0;
  for(i=1;i<=r;i++)
  {
    printf("Please input N(%d) : ",i);
    scanf("%d",&n[i]);
     if((n[i]<0)||(n[i]>30))
    {
     printf("\nWrong parameters!!!\n");
     exit(1);
    }
    n[r+1]=n[r+1]+n[i];
  };
  if(n[r+1]!=n[0])
  {
    printf("\nWrong parameters!!!\n");
    exit(0);}
    
  pipe(fd); // Open chanel  
  sprintf(buf1,"%d",fd[1]);
  
  for(i=0;i<=r;i++)
  {
    if((pid[i]=fork())==0) // Child process
    {
      printf("I wait a signal\n");
      pause(); 
      printf("----------------------------------------------------------\n");
      printf("%d Child PID: %d\n",i,getpid());
      sprintf(&buf1[1]," %d\n",n[i]);
      if(execl("fact","fact",buf1,NULL)==0) // Running the program
       { printf("Error in execl\n");exit(1);};
       exit(1);
     };         
   };
  sleep(3);
    for(i=0;i<=r;i++)
  {  printf("I send a signal\n");    
     kill(pid[i],SIGUSR1);
     wait(&s1);
  }
  close(fd[1]);
  printf("   READING\n");
  s=s1=1;
  for(i=0;i<=r;i++)
  {
  read(fd[0],buf1,12);
  sscanf(buf1,"%d %d",&pid[1],&n[i]);
  if(pid[1]==pid[0]) 
       s1=n[i]; //N!
  else s=s*n[i]; 
    };  
    close(fd[0]);
    s=s1/s;  
  printf("Rezultat: C=%d\n",s);
  exit(0);
}

 