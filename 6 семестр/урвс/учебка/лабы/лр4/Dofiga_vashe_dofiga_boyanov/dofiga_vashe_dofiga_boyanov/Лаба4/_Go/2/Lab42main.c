#include <fcntl.h>
#include <signal.h>

#define PI 3.141592
#define NCALC 20
#define N 10

int fd[2];

void start(int sig)
{
    close(0);
    fcntl(fd[0], F_DUPFD, 0);
    execl("lab42");
}

void stop(int sig)
{
    double f;
    f = -1;
    write(fd[1], &f, sizeof(double));
}

int main()
{
    	int i,j;
	int k,khlp;		
	pid_t pid; 		// child process pid
    	double f,x,hlp; 		// function value and its argument    
	
	if(pipe(fd) == -1)
	{
		printf("!!! Error creating a pipe !!!\n");
		exit(1);
	}
	   	
	signal(SIGUSR1, start);
    	signal(SIGUSR2, stop);
    
	pid = fork();
    	if(pid)
    	{
    		kill(pid, SIGUSR1);

		for(i = 0; i < NCALC; i++)
		{
			f = 0;
			x = i*0.05*PI;
			for(k = 0; k < N+1; k++)
	    		{
				khlp=2*k+1;    			
				hlp=1;
				for(j = 0; j < khlp; j++)
				    hlp*=x;
				f += hlp / khlp;
		    	}
			write(fd[1], &f, sizeof(double));
    		}
	    	kill(pid, SIGUSR2);
		wait();
		close(fd[0]);
		close(fd[1]);
	}     
	else
		sleep(1);
	exit(0);
}
