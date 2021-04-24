#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h> 
int main () {
    int pid_pi,pid_cos,pd[2],temp;
    float x,pi,cos,f;
    char rpd[10],wpd[10];				// contain pipe discriptors numbers
    printf ("\n Enter x=");
    scanf("%f",&x);
    pipe(pd);					// create pipe
    sprintf(rpd,"%d",pd[0]);	
    sprintf(wpd,"%d",pd[1]);
    if((pid_pi=fork())==0)
	{				// child 1
		execlp("./pi",wpd,0);		
		exit(0); 
	}
    if((pid_cos=fork()) == 0) 
	{			// child 2
		execlp ("./cos",rpd,wpd,0);
        exit(0);
	}
	sleep(1);
    kill(pid_pi,SIGINT); 				// call child 1
    wait(0);
    read(pd[0],&pi,sizeof(pi));			// read pi
    printf("\n parent read pi=%f from pipe",pi);
    printf("\n parent write x=%f into pipe",x);
    write(pd[1],&x,sizeof(x));
    kill(pid_cos,SIGINT);
    wait(0);
    read(pd[0],&cos,sizeof(cos));
    printf("\n parent read cos(x)=%f from pipe",cos);
    printf("\n f(x)=%f\n",(1-cos)/(x*x*pi));	
    sleep(3);
}