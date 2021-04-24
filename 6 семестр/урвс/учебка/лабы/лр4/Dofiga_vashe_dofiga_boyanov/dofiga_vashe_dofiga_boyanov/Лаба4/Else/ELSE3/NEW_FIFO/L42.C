#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#define FIFO "fifo.1"

void OnSign() {;}

int main () {
    int pid_pi,pid_exp,pd[2];
    float x,pi,exp,f;
    char rpd[10],wpd[10];		// contain pipe discriptors numbers

    mkfifo(FIFO,0600);				// create pipe

    printf ("\n Enter x=");
    scanf("%f",&x);
    
    if((pid_pi=fork())==0) {		// child 1
	execlp("./pi","./pi",FIFO,0);		
	exit(0); }

    if((pid_exp=fork()) == 0) {		// child 2
        sleep(2);
	execlp ("./exp","./exp",FIFO,0);
        exit(0); }

//    sleep(1);

//    kill(pid_pi,SIGINT); 		// call child 1
//    wait(pid_pi,NULL,0);

//    signal(SIGINT,OnSign);
//    pause();    
    
    pd[0]=open(FIFO,O_RDONLY);
    sleep(1);
//    wait(0);
    read(pd[0],&pi,sizeof(pi));		// read pi
    printf("\n parent read pi=%f from pipe",pi);
//    close(pd[0]);

    pd[1]=open(FIFO,O_WRONLY);
    write(pd[1],&x,sizeof(x));
    printf("\n parent wrote x=%f into the pipe",x);
//    kill(pid_exp,SIGINT);
//    wait(pid_exp,NULL,0);
    sleep(2);
//    wait(0);
//    signal(SIGINT,OnSign);
//    pause();    
    read(pd[0],&exp,sizeof(exp));
    printf("\n parent read exp(-x*x/2)=%f from pipe",exp);

    printf("\n f(x)=%f\n",exp/(2*pi));	// should be sqrt(2*pi)
//    sleep(3);
    close(pd[0]);
    close(pd[1]);
//    wait(pid_pi,NULL,0);
//    wait(pid_exp,NULL,0);
}



