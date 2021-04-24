#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

void OnSignal (){;}
int main (int argc, char *argv[]) {
    float pi;
    int pd;						// pipe write discriptor

    sscanf(argv[0],"%d",&pd);
    signal(SIGINT, OnSignal);
    pause();
	for(int i=1.,pi=4.;i<10.;i++) //4.-4./3.+4./5.-4./7.+4./9.-4./11.+4./13.;
		if(i%2==0)
			pi-=pi/(2.*i+1) ;		
		else pi+=pi/(2.*i+1);
    write(pd,&pi,sizeof(pi));			// write pi into pipe
}
