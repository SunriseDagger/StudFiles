#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>

#define CHISEXP 30

int main(int argc,char *argv[])
{
int fd;
unsigned i,n,p,c,k;
double exp=1;

sscanf(argv[0],"%d",&fd);//chitaem literi iz zepochki argv[0]
for (n=1;n<CHISEXP;n++)
    { 
     for(i=n,p=1;i>1;i--)
        p=p*i;
	c=1;
	for(k=1;k<=n;k++)
	 c=c*2;
	 exp=exp+c/p;
    }
printf("exp(2) = %lf\t(calculated by process #%d (begin from #%d))\n",exp,getpid(),getppid());
write(fd,&exp,sizeof(double) );
}

