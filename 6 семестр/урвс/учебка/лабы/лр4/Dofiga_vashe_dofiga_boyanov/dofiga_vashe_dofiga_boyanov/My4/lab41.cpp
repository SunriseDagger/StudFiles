#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include <stdlib.h>

#define CHISPI	1000
#define CHISCOS	10 
#define COS	0
#define PI	1

struct record
{
double data;
int type;
}rec;
    
void main(int argc,char* argv[])
{
 double pi=0,cos=1,x;
 double fact=1,t=1;
 unsigned i, fd;
 if (argc==2)
    x=atof(argv[1]);
 else
    {
    printf("Incorrect! Correct syntax: ./lab4 <x>!!!!\nInput x:");
    scanf("%d",&x);
    }
 fd=creat("temp.tmp",0666);
 if (fd==-1)
    {
    printf("Error (can't create temporary file)");
    exit(1);
    }
 if (!fork())
    { 
    for (i=2;i<CHISCOS;i+=2) 
	{
	t=t*x*x*(-1);
	fact*=(i*(i-1));
	cos+=t/fact;
	} 
    rec.data=cos; 
    rec.type=COS;
    lseek(fd, 0, SEEK_END);
    write(fd, &rec, sizeof(rec));
    printf("cos(x) = %lf\t(calculated by process #%d)\n",rec.data,getpid());
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
	if (rec.type==COS) cos=rec.data;
	else pi=rec.data;
	}
    }
 close(fd);
 printf("Result: %lf\t(calculated by main process #%d)\n",(1-cos)/(pi*x*x),getpid());
 exit(0);
}

