#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

#define CHISPI 1000

int main(int argc, char *argv[])
{
int fd;
double pi=0,t;
unsigned i;

sscanf(argv[0],"%d",&fd);
t=4;
for (i=0;i<CHISPI;i++)
    {
        pi+=t/(2*i+1);
        t*=(-1);
    }
printf("pi = %lf\t\t(calculated by process #%d (begin from #%d))\n",pi,getpid(),getppid());
write(fd, &pi, sizeof(double) );
}

