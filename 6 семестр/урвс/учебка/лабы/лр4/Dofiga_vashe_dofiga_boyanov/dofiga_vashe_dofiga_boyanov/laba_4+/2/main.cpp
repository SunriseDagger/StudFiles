#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int 	flag = 0;

void Signal( int n )
{
    printf("Process %d  catch signal %d \n", getpid(), n);
    flag++;
}

int main()
{
    double      x = 0.5;
    double 	cos = 0, pi = 0, f, time;
    char 	argum[10];
    int 	fd1[2], fd2[2], pid1, pid2;
        
    pipe(fd1);	
    pipe(fd2);
    switch ( pid1 = fork() )
    {
	case -1: printf("Error: fork1...\n");break;
	case  0: 
	    signal(10, Signal);
	    while ( flag == 0 )  
		sleep(1);
	    sprintf(argum, "%d", fd1[1]);
	    execl("./pod1.exe", "pod1.exe", argum, 0);
	break;    
    }
    kill(pid1,10);
    sleep(1); 

    switch ( pid2 = fork() )
    {
	case -1: printf("Error: fork2...\n");break;
	case  0: 
	    signal(20, Signal);
	    while (flag == 0)  
		sleep(1);
	    sprintf(argum, "%d", fd2[1]);
	    execl("./pod2.exe", "pod2.exe", argum, 0);
	break;    
    }
    kill(pid2, 20);  
    wait(&time); 

    read(fd1[0], &pi,  sizeof(double));
    read(fd2[0], &cos, sizeof(double));	    
    
    f = (1 - cos) / (pi * x * x);
    printf("f(x)= %f\n", f);		
    return 0;
}