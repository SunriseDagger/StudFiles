#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
void sign(int z)
{
;
}
int main ()
{
	int r, N, k, i = 1, pid, fd[2], xq;
	float f, g, p, x, t;
	char str1[10], str2[10];
	printf ("N=");
	scanf ("%d", &N);
	printf ("X=");
	scanf ("%f", &x);
//pipe(fd); //open canal for reading and writing
//mkfifo ("fifo", 010777);
	if (fork ())
	{
		t=1;
		f=0.0;
		for (k=0; k<=N; k++)
		{
			f=f+pow (x, 2*k+1)*t;
			t=t*x*x*(2*k+1)/(3*k+1);
			write (fd[1], &f, 4);
		}
		write (fd[1], &f, 4);
		kill (xq, 30);
		sleep (5);
		kill (xq, 9);
		printf ("End of child\n");
		kill (pid, 1);
		wait (&k);
	}
	else
	{
		xq=getpid ();
		signal (30, sign);
		printf ("start child\n");
//writing inode of canal
		sprintf (str1, "%d", fd[0]);
		sprintf (str2, "%d", fd[1]);
//end signal
		signal (9, sign);
		execl ("/root/LAB42.out", "LAB42.out", str1, str2, 0);
	}
	return 1;
}
