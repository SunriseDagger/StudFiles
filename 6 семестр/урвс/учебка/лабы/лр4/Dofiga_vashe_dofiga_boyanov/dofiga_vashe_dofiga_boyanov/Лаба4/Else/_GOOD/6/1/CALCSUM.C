/* calcsum - programs to calculate several values of f(x) */

#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

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

int main(int argc, char* argv[])
{
    int i, j, print;
    int n; // number of calculations
    int k; // number of summands
    int fd; // temporary file descriptor
    int status; // executed process status
    time_t t; // current time
    pid_t pid; // child process pid
    int m; // temp variable
    double f, x; // function value and its argument
    char tempfilename[] = "temp.file";

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
    srand((unsigned) time(&t));
    pid = fork();
    if(pid == -1)
    {
    	error("Can't create child process");
    }
    if(pid)
    {
    	/* parent */
        if((fd = creat(tempfilename, 0666)) == -1)
    	{
		error("Error creating file temp.file\n");
    	}
    	for(i = 0; i < n; i++)
    	{
    	f = 0;
    	for(j = 0; j <= k; j++)
    	{
    		m = 2*j + 1;
        	x = (double) rand() / RAND_MAX * PI;
		f += pow(x, m) / factorial(m);
    	}
    	write(fd, &f, sizeof(double));
    }
    f = INF; // stop number
    write(fd, &f, sizeof(double));
    close(fd);
    wait(&status);
    if(remove(tempfilename) == -1)
	error("Can't remove temporary filen\n");
    }
    else
    {
    	/* child */
    	if((fd = open(tempfilename, O_RDONLY)) == -1)
    	{
		printf("Error opening file temp.file\n");
		exit(1);
    	}
    	f = 0;
    	for(;;)
    	{
		print = 0;
		if(read(fd, &f, sizeof(double)))
	    		print = 1;
        	if(f == INF)
	    		break;
		if (print)
			printf("f(x) = %lf\n", f);
    	}
    close(fd);
    }
    return 0;
}
