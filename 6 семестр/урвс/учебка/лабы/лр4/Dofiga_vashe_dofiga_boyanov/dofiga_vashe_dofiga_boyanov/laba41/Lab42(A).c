#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#define n 1
#define FIFO "temp"
#define MAXBUFF 80

double getFunc (double x)
{
	int k;
	double xx=x;
	for (k=1; k<=2*n; k++)
		x*=xx;
	return x/(2*n+1);
}

void signalOn	()
{
	signal (SIGINT,signalOn);
}

int main ()
{	double x, y;
	int fd[2], child;
	char string1[10], string2[10];
	printf("!!!!!!!!!!!!!!!!\n");
	if (mkfifo (FIFO, 010777)<0)
	{
		printf ("ERROR!");
		exit (1);
	}
	printf ("FOR READ\n");
	sleep(2);
	if ((fd[0]=open (FIFO, O_RDWR))<0)
	{
		printf("ERROR! can't open fot read\n");
		exit(1);
	}
	printf("FOR WRITE\n");
	sleep(2);
	fd[1]=open (FIFO, O_RDWR);
	printf ("%d %d\n", fd[0], fd[1]);
	printf ("---------------------------------------\n");
	sprintf (string1, "%d", fd[0]);
	sprintf (string2, "%d", fd[1]);
	child=fork ();
	if (child==0)
	{
		execlp ("./process", string1, string2, 0);
		exit (0);
	}
	signal (SIGINT, signalOn);
	sleep (2);
	for (x=0; x<3.1415296; x+=1)
	{
		y=getFunc (x);
		write (fd[1], &y, sizeof (double));
		kill (child, SIGINT);
		pause ();
	}
	y=-1.0;
	write (fd[1], &y, sizeof (double));
	kill (child, SIGINT);
	pause ();
	close (fd[0]);
	close (fd[1]);
	wait ();
	unlink ("temp");
	return 1;
}