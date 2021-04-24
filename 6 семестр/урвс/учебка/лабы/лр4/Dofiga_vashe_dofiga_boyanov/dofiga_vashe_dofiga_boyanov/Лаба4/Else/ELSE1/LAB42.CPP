#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int work=0;

void EndWork(int num)
{
work++;
}
    
void main(int argc, char* argv[])
{
int chan1[2], chan2[2], pid1, pid2;
char arg1[20], arg2[20],arg3[20];
double pi, cos,x;
unsigned i;

if (argc==2)
    x=atof(argv[1]);
else
    {
    printf("Incorrect! Correct syntax: ./lab4 <x>!!!!\nInput x:");
    scanf("%d",&x);
    }

pipe(chan1);
pipe(chan2);
if(!(pid1=fork()))
    {
    signal(SIGUSR1,EndWork);
    while (!work) sleep(1);
    sprintf(arg1,"%d",chan1[1]);
    sprintf(arg2,"%lf",x);
    execl("cos",arg1,arg2,0);
    pause();
    }
else
    {
    sleep(1);
    kill(pid1,SIGUSR1);
    if (!(pid2=fork()))
	{
	signal(SIGUSR2,EndWork);
	while (!work) sleep(1);
	sprintf(arg3, "%d", chan2[1]);
	execl("pi", arg3, 0);     
	pause();
	}
    sleep(1);
    kill(pid2,SIGUSR2);
    }

while (read(chan1[0],&cos,sizeof(double))==0)
    sleep(1);
kill(pid1,SIGCLD);
while (read(chan2[0],&pi,sizeof(double))==0)
    sleep(1);
kill(pid2,SIGCLD);
printf("Result: %lf\t(main process: #%d)\n", (1-cos)/(pi*x*x),getpid()); 
}

