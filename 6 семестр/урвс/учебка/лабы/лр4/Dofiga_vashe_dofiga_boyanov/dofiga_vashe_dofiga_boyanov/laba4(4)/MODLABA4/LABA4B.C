#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>

int index = 0;

void StartSignal(int n)
{
    printf("Process %d  catch signal %d \n",getpid(),n);
    index++;
}

int main()
{
    int a1, a2;
    double e2 = 0, pi = 0, f, qw;
    char argum[10];
    int fd1[2], fd2[2];
        
    printf("Start make the canals....\n");
    pipe(fd1);	
    pipe(fd2);
    printf("Finish make the canals....\n");
    //make 1 child process;
    switch (a1 = fork())
    {
	case -1: printf("Error to child process 1.\n");exit(1);
	case  0: 
	    printf("Enter to 1 child process(pi)....\n");
	    signal(10,StartSignal);           //catch signal from parents;
	    while (index == 0)  sleep(1);
	    sprintf(argum,"%d",fd1[1]);      //Put int fd1[1] to string;
	    execl("./pod1","pod1",argum,0);  //Change programm in pod1;
	break;    
    }
    printf("Parents cries to 1 child...\n");
    sleep(1);
    kill(a1,10);                             //Call son-process a1;
    sleep(1);                                //Wait execution son-process 1; 
    //make 2 child process;
    switch (a2 = fork())
    {
	case -1: printf("Error to child process 2.\n");
	case  0: 
	    printf("Enter to 2 child process(e2)....\n");
	    signal(20,StartSignal);         //Catch signal from parents;
	    while (index == 0)  sleep(1);
	    
	    sprintf(argum,"%d",fd2[1]);     //Put int fd2[1] to string;
	    execl("./pod2","pod2",argum,0);       //Chang program in pod2;
	break;    
    }
    printf("Parents cries to 2 child....\n");
    sleep(1);
    kill(a2,20);                            //Call son-process 2;  
    wait(&qw);                              //Wait execution son-process 2; 
    printf("Parents process begining read....\n");

    read(fd1[0],&pi,sizeof(double));       //reading result from canals;
    read(fd2[0],&e2,sizeof(double));	    
    
    printf("Parents was read....\n");
                                           //output result;
    f = 1. / (pi* (1. + e2));
    printf("F= %f\n",f);		
    return 0;
}