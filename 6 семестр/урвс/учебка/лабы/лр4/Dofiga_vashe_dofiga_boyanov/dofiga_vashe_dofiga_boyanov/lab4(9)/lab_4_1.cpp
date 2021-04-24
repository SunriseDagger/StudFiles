#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include <stdlib.h>

#define CHISPI	1000
#define CHISEXP	10 
#define EXP	0
#define PI	1

struct record
{
double data;
int type;
}rec;
    
int main()
{
 double pi=0,exp=1,t=1;
 unsigned i,n,p,c,k,fd;

 fd=creat("temp.tmp",0666);//cozdaem new fail
 if (fd==-1)
    {
    printf("Error (can't create temporary file)");
    exit(1);
    }
 if (!fork())
    { 
    for(n=1;n<CHISEXP;n++)
     {
      for(i=n,p=1;i>1;i--)
       p=p*i;
       c=1;
       for(k=1;k<=n;k++)
          c=c*2;
         exp=exp+c/p;	  
     }
    rec.data=exp; 
    rec.type=EXP;
    lseek(fd, 0, SEEK_END);
    write(fd, &rec, sizeof(rec));
    printf("exp(2) = %lf\t(calculated by process #%d)\n",rec.data,getpid());
    close(fd); 
    exit(0);
    }
 else if (!fork())
    {
    t=4;
    for (i=0;i<CHISPI;i++) 
	{
	pi+=t/(2*i+1);
	t*=(-1);
	}
    rec.data=pi;
    rec.type=PI;
    lseek(fd, 0, SEEK_END);
    write(fd, &rec, sizeof(rec));
    printf("pi = %lf\t\t(calculated by process #%d)\n",rec.data,getpid());
    close(fd);
    exit(0);
    }
 close(fd);
 fd=open("temp.tmp",O_RDONLY);
 i=0;
 while(i<2)
    {
    if (read(fd,&rec,sizeof(rec)))
	{
	i++;
	if (rec.type==EXP) exp=rec.data;
	else pi=rec.data;
	}
    }
 close(fd);
 printf("Result: %lf\t(calculated by main process #%d)\n",1/(pi*(1+exp)),getpid());
 exit(0);
}

