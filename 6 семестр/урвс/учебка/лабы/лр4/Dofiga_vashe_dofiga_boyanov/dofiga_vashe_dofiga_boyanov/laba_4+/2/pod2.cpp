#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{                                     
    int 	MAX = 1000, fd;       
    double 	cos = 0, z = -1, t = 1, x = 0.5;

    for ( int i = 0; i < MAX; i++ )  
    {
	 t *= (x * x)/( (2 * i - 1) * ( 2 * i) );
	 cos += z * t;
	 z *= -1;
    }
	
    sscanf(argv[1], "%d", &fd);
    write(fd, &cos, sizeof(double));
}