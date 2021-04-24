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
    
int main()
{
int chan1[2], chan2[2], pid1, pid2;
char arg1[20],arg3[20];
double pi, exp;
unsigned i;

pipe(chan1);//sozdaem kanali dlya vzaimodeistvia
pipe(chan2);
if(!(pid1=fork()))
    {
    signal(SIGUSR1,EndWork);
    while (!work) sleep(1);//gdem signala
    sprintf(arg1,"%d",chan1[1]);//zapisivaet dannie v cepochku arg1,zavershaya
                                //ix pustoi literoi 
    execl("exp",arg1,0);//vipolnit fail "exp"
    pause();//priostanavlivaem pr-ss,poka ne budet poluchen signal
    }
else
    {
    sleep(1);
    kill(pid1,SIGUSR1);//posilaem signal sigusr1 pr-su s identificatorom pid1
    if (!(pid2=fork()))//zapuskaem 2-oi prozess-potomok
	{
	signal(SIGUSR2,EndWork);
	while (!work) sleep(1);
	sprintf(arg3, "%d", chan2[1]);
	execl("pi", arg3, 0);//vizivaem ispolnyaemiy modul "pi"     
	pause();
	}
    sleep(1);
    kill(pid2,SIGUSR2);
    }

while (read(chan1[0],&exp,sizeof(double))==0)
    sleep(1);                                //gdem informaziu v kanalax
kill(pid1,SIGCLD);                           //i ubivaem prozessi-potomki
while (read(chan2[0],&pi,sizeof(double))==0)
    sleep(1);
kill(pid2,SIGCLD);
printf("Result: %lf\t(main process: #%d)\n", 1/(pi*(1+exp)),getpid()); 
}

