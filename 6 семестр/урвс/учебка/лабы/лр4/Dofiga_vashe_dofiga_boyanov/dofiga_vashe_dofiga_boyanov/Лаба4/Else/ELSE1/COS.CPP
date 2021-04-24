#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>

#define CHISCOS 30

int main(int argc, char *argv[])
{
int fd;
unsigned i;
double cos,fact=1,t=1,x;

sscanf(argv[0],"%d",&fd);
x=atof(argv[1]);
cos=1;
for (i=2;i<CHISCOS;i+=2)
    {
        t=t*x*x*(-1);
        fact*=(i*(i-1));
        cos+=t/fact;
    }
printf("cos(x) = %lf\t(calculated by process #%d (begin from #%d))\n",cos,getpid(),getppid());
write(fd,&cos,sizeof(double) );
}

