#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

/* Coliculating the pi (= 3.14) in the row */
int main(int argc, char *argv[])
{
    int i, n = 100, fd;  //n - number iteration for row of Pi;
    double pi = 0;
    
    printf("1 dauhter process is writting....\n");
    for (i=1;i<=n;i++)                     //row of Pi;
	pi = pi + pow(-1,i) / i;
    pi = 2*pi;
    
    sscanf(argv[1],"%d",&fd);        //Recods from string to int fd;
    write(fd, &pi, sizeof(double));  //Output to canal with FD[1] = fd;
 }       