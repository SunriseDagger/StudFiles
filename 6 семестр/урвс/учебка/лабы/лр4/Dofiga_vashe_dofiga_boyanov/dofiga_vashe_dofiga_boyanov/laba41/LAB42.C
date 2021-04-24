#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

void main (int argc, char *argv[])
{
	int i, k=0, fd[2];
	float g;
	sscanf (argv[1], "%d", &fd[0]);
	while (g!=0.0||i==0)
	{
		read (fd[0], &g, 4);
		if (g!=0.0)
			printf ("%f\n", g);
	}
	exit (0);
}


