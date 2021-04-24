#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include <cstdlib>

using namespace std;

int got = 0;

struct randpare
{
    double x;
    double y;
}pare;

double catalog_f(double x)
{
    return(4/(1+x*x));
}

void sigfunc(int signum)
{
    got++;
    signal(SIGUSR1, sigfunc);
}

int main(int argc, char* argv[])
{
    int channel[2], pid, fd, num;
    int checkin = 0;
    double integral;
    char  arg0[20], arg1[20];
    double val;
    if(argc!=2)
    {
	cout<<"Input program argument!"<<endl;
	cin>>num;
    }else num = atoi(argv[1]);
    	
    pipe(channel);
    
    if(!(pid=fork()))/*daughter process*/
    {
	signal(SIGUSR1, sigfunc); /*set function for signal = SIGUSR1*/
	while(!got) sleep(1); /*while(signal didn't got)*/
	sprintf(arg0, "%d", channel[1]);/* fd for writing*/
	sprintf(arg1, "%d", num);/* number of random */
	execl("son",arg0, arg1,0);
	//if exec failed
	perror("exec failed!");
	exit(1);
    }/*main proccess */
    sleep(1);/*if in son process void(signal()) isn't set yet! */
    kill(pid, SIGUSR1);
    for(int i=0; i<num; i++)
    {
	if(read(channel[0],&pare,sizeof(pare)))
	{
	    if(catalog_f(pare.x)>=pare.y)
	    {
		checkin++;
	    }
	}else num++;
    }
    integral = 4*((double)checkin)/num;
    cout<<"in points= "<<checkin<<endl;
    cout<<"Integral= "<<integral<<endl;
    exit(0);
}
