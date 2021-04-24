#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>      
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>

int flag = 1;

void hand_signal(int signo) {
	flag = 0;
}


double myFunc(double x)
{
    return 4 / (1 + x * x);
}

main(int argc,char *argv[])
{ 
    int pid,pid1;
    int pipeFd[2];
    double rez;
    int N = 0;
  

  if(argc != 2)  { printf("You should specify N argument - quantity of random points.\n");
                   exit(1);}

  N = atoi(argv[1]);       
  
  if(N == 0) {printf("Too small number of points specified: %d\n",N);
              exit(1);}

  printf("Making a pipe.\n");
  if(pipe(pipeFd) !=0) {printf("Can't build a pipe");
                        exit(1);}       

	signal(SIGUSR1, hand_signal);

                        
    printf("Starting child process.\n");
    pid = fork();   
  
    if(pid == -1) {printf("Can't start child process");
        exit(1);}
                  
    if(pid == 0) 
    {
    	// Creating command line for child
        char fdString[80];
        sprintf(fdString, "%d", pipeFd[1]); // adding pipe's id
        execl("./child", "child", argv[1], fdString, 0);
    	
        // execl returned error
        perror("execl\n"); 
        exit(1);
    }
	printf("Waiting for signal\n");
	//  Waiting for signal from child
	while (flag) pause();

    // Command child to start calculation
	usleep(1000);
	kill(pid, SIGUSR1);


    int ind;
    double* Vals;
    Vals = (double*)malloc(N * sizeof(double));
        
    read(pipeFd[0], Vals, sizeof(double) * N);
        
    double integral = 0;
    for(ind = 0;ind < N - 1; ind++)
    {
        integral += (myFunc(Vals[ind]) + myFunc(Vals[ind])) * (Vals[ind + 1] - Vals[ind]) / 2;
    }

    // Command child to exit
	kill(pid, SIGUSR2);

  	wait();

    free(Vals);
    printf("Integral = %f\n", integral);
}  
