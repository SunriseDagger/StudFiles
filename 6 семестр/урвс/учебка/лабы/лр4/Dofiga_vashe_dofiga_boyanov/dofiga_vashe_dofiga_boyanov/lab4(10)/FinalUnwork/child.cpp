#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>      
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>

int flag = 0;

void hand_signal(int signo) {
	flag = signo;
	signal(signo, hand_signal);
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
    double *Vals;
    int outp, N;
  

  if(argc != 3)  { printf("You should specify 2 arguments.\n");
                   exit(1);}

  N = atoi(argv[1]);
  outp = atoi(argv[2]); // Читаем вторым параметром номер дескриптора, для вывода

	signal(SIGUSR1, hand_signal);
	signal(SIGUSR2, hand_signal);

    // Посылаем предку сигнал о готовности
	kill(getppid(), SIGUSR1);

    // Работаем, пока не пришел сигнал SIGUSR2
	while (flag != SIGUSR2) {
		flag = 0; while (! flag) pause(); // Ждем SIGUSR1 или SIGUSR2
   		printf("Child recieved signal\n");

		if (flag == SIGUSR1) { // Если пришел SIGUSR1
    		printf("Start Proccess\n");
	        srand48(N);
        
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
        
	        write(outp, Vals, sizeof(double) * N);
        }
	}
    free(Vals);
    printf("Child process stopped.\n");
    exit(1);
}  
