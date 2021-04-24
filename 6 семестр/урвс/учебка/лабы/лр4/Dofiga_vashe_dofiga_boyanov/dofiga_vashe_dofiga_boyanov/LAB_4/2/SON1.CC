#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include<sys/types.h>
int n;

struct elem
{
int num_proc;
int result;
int des[2];
};
 elem el_1,el_2,el_3;

static void run(int signo)
{
int i,s1=1; 

for(i=1;i<=n;i++) s1*=i; 
     el_1.result=s1;
   write(el_1.des[1],&el_1,sizeof el_1);
  exit(0);
}

void main(int argc,char *argv[])
{
int des;
sscanf(argv[1],"%ld",&des);
read(des,&el_1,sizeof el_1);
n=el_1.result;
signal (SIGINT, run);
//printf("%s","Wait for signal");


pause();
exit(0);
}

