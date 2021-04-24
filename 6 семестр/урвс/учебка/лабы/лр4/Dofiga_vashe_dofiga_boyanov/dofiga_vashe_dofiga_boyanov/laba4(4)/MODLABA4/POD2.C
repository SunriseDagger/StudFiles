#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

//*****Function of caliculating factorial n;*******
int fac (int n)
{
    int i, r = 1;
    for (i=1; i<=n; i++)
	r = r*i;
    return r;	
}
//****Function of caliculating a in power n;*******
int pow (double a, int n)
{
    int i, r = 1;
    for (i=1; i<=n;i++)
	r = r*a;
    return r;	
}

int main(int argc, char *argv[])
{                                     
    int n = 30, fd, i;                //n - number iteretion row of e2; 
    double e2 = 0;
    printf("2 douhter process is writting....\n");	
    for (i=0; i<=n; i++)               //row of e in power 2;
	e2 = e2 + pow(2,i)/fac(i); 
	
    sscanf(argv[1],"%d",&fd);	      //records from string to int fd;
    write(fd, &e2, sizeof(double));   //output e2 to canal with FD[1] = fd;
}