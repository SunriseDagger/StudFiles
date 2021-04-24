/* calcsum - programs to calculate several values of f(x) */

#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <sys/param.h>

#define PI 3.1415926535897932384626433832795
#define INF 1e500 // stop number

/* returns factorial of number k */
double factorial(int k)
{
    int i;
    double f = 1;
    for(i = 2; i <= k; i++)
	f *= i;
    return f;
}

void error(char *msg)
{
	printf("%s\n", msg);
	exit(1);
}

int fd[2];

// starts child process
void start(int sig)
{
    close(0);
    fcntl(fd[0], F_DUPFD, 0);
    execl("read", "read", 0);
}

// stops child process
void stop(int sig)
{
    double f;
    f = INF;
    write(fd[1], &f, sizeof(double));
}

int main(int argc, char *argv[])
{
    int i, j, print;
    int n; // number of calculations
    int k; // number of summands
    int status; // executed process status
    pid_t pid; // child process pid
    time_t t; // current time
    double f, x; // function value and its argument
    int m; // temp variable

    if(argc != 1)
    	error("Usage: calcsum (without any params)\n");
    printf("Enter number of summands (1-100):\n");
    scanf("%d", &k);
    if((k<1) || (k>100))
    {
    	error("Number of summands must be between 1 and 100\n");
    }
    printf("Enter number of calculations (1-100):\n");
    scanf("%d", &n);
    if((n<1) || (n>100))
    {
    	error("Number of calculations must be between 1 and 100\n");
    }
    if(pipe(fd) == -1)
    {
	error("Error creating a pipe\n");
    }
    srand((unsigned) time(&t));
    signal(SIGUSR1, start);
    signal(SIGUSR2, stop);
    pid = fork();
    if(pid)
    {
	/* parent */
	kill(pid, SIGUSR1); // starting child process
	for(i = 0; i < n; i++)
	{
	    f = 0;
	    for(j = 0; j <= k; j++)
	    {
		m = 2*j + 1;
    		x = (double) rand() / RAND_MAX * PI;
		f += pow(x, m) / factorial(m);
	    }
	write(fd[1], &f, sizeof(double));
	}	
	
    kill(pid, SIGUSR2); // stoping child process
    wait(&status);
    close(fd[0]);
    close(fd[1]);
    }
    else
    {
    /* child */
    for(;;)
	sleep(1);
    }
    return 0;
}
