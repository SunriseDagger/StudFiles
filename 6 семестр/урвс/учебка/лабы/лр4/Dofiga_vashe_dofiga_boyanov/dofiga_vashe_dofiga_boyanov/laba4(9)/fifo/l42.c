#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#define FIFO "fifo.1"

int main () {
    int pid_pi,pid_exp,pd,temp;
    float x,pi,exp,f;
    char rpd[10],wpd[10];		// contain pipe discriptors numbers

    printf ("\n Enter x=");
    scanf("%f",&x);

    mkfifo(FIFO);				// create pipe
    pd[0]=open(FIFO,O_RDONLY);
    pd[1]=open(FIFO,O_WRONLY);
    sprintf(rpd,"%d",pd[0]);	
    sprintf(wpd,"%d",pd[1]);

    if((pid_pi=fork())==0) {		// child 1
	execlp("./pi",wpd,0);		
	exit(0); }

    if((pid_exp=fork()) == 0) {		// child 2
	execlp ("./exp",rpd,wpd,0);
        exit(0); }

    sleep(1);

    kill(pid_pi,SIGINT); 		// call child 1
    wait(0);
    read(pd[0],&pi,sizeof(pi));		// read pi
    printf("\n parent read pi=%f from pipe",pi);

    printf("\n parent write x=%f into pipe",x);
    write(pd[1],&x,sizeof(x));
    kill(pid_exp,SIGINT);
    wait(0);
    read(pd[0],&exp,sizeof(exp));
    printf("\n parent read exp(-x*x/2)=%f from pipe",exp);

    printf("\n f(x)=%f\n",exp/(2*pi));	// should be sqrt(2*pi)
    sleep(3);
}



