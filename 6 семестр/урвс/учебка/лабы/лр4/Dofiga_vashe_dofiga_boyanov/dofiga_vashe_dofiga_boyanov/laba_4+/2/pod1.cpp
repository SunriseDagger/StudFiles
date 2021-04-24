#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int 	MAX = 1000, fd;
    double 	pi  = 0, z = 4;
	
    for ( int i = 0; i < MAX; i++ )
    {
	pi += z / (2 * i + 1);
	z *= -1;
    }
    
    sscanf(argv[1], "%d", &fd);
    write(fd, &pi, sizeof(double));
 }       