#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>      
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
/*
int flag = 1; 


static void sig_start(int signo) //функция - обработчик сигнала 
{
    flag = 0;
}

static void died_potomok(int signo)//функция - обработчик сигнала
{
    printf("Child process stopped.\n");
}
*/
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
  
  //signal(SIGUSR1,sig_start);//Перехватывать сигналы SIGUSR1 функцией sig_start        
  //signal(SIGCHLD,died_potomok);//Перехватывать сигналы SIGCHLD функцией died_potomok      

  if(argc != 2)  { printf("You should specify N argument - quantity of random points.\n");
		   exit(1);}// Проверяем верность числа параметров переданных при вызове

  N = atoi(argv[1]); // Запоминаем x переданный пограмме извне		   
  
  if(N == 0) {printf("Too small number of points specified: %d\n",N);
       	      exit(1);}//Если х==0 то выйти тк делить на 0 нельзя

  printf("Making a pipe.\n");
  if(pipe(pipeFd) !=0) {printf("Can't build a pipe");//открыть канал
                        exit(1);}	
			
    printf("Starting child process.\n");
    pid = fork();   
  
    if(pid == -1) {printf("Can't start child process");
        exit(1);}
		  
    if(pid == 0) 
    {
	//while(flag) pause; 
	srand48(N);
	
	double* Vals;
	Vals = (double*)malloc(N * sizeof(double));
	
	long ind;
	for(ind = 0; ind < N; ++ind)
	{
	    Vals[ind] = drand48();
	}
	
	int changed = 1;
	
	while(changed == 1)
	{
	    changed = 0;
	    for(ind = 0; ind < N - 1; ++ind)
		if(Vals[ind] > Vals[ind + 1])
		{
		    double val;
		    val = Vals[ind];
		    Vals[ind] = Vals[ind + 1];
		    Vals[ind + 1] = val;
		    changed = 1;
		}
	}
	for(ind = 0; ind < N; ++ind)
	{
	    printf("%.6f\n", Vals[ind]);
	}
	
	write(pipeFd[1], Vals, sizeof(double) * N);
	free(Vals);
        printf("Child process stopped.\n");
        exit(1);
    }                 

    //kill(pid, SIGUSR1); //послать сигнал процессам-потомкам       

    int ind;
    double* Vals;
    Vals = (double*)malloc(N * sizeof(double));
	
    read(pipeFd[0], Vals, sizeof(double) * N);
	
    double integral = 0;
    for(ind = 0;ind < N - 1; ind++)
    {
	integral += (myFunc(Vals[ind]) + myFunc(Vals[ind])) * (Vals[ind + 1] - Vals[ind]) / 2;
    }
   
    free(Vals);
    printf("Integral = %f\n", integral);
}
