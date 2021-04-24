#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <sys/uio.h>
#include <stdlib.h>

int main()
{
    int ps1,ps2,k,fd,pid=0,j;
    double pi,c,f,x;    
    fd=open("./tmp",O_RDWR |O_CREAT |O_TRUNC, S_IRUSR |S_IWUSR);
    printf("vvod x\n");
    scanf("%lf",&x);
    write(fd,&x,sizeof(x));
    ps1=fork();
    if (ps1==0)
	{
	  int n;
	  double pi,x1,y=0.16666666;
	   x1=1+y;
	   n=2;
    	while(y>0.0000000000001)
	{
	 n=n+2;
	 y=(n-1)*(n-1)*y/(n*(n+1));
	 x1=x1+y;
	}; 
         pi=2*x1;
	 pid=getpid();
	 write(fd,&pid,sizeof(pid));
	 write(fd,&pi,sizeof(pi));
	 exit(0);
	} 	
    else
	{
	  ps2=fork();
	  if(ps2==0)
	  {
	   double  c,y;
	   int n=1,k=1;
	  lseek(fd,0,0);
          read(fd,&x,sizeof(x));
	  y=x*x/2;
	  c=1-y;
	   while(y>0.0001)
	   {n=n+2;
	   y=y*x*x/(n*(n+1));
	   if(k==1)
	         {
		 c=c+y;
		 k++;
		 }
		 else
		   {c=c-y;
		   k--;
		   }  
	   };
	  pid=getpid();
	  write(fd,&pid,sizeof(pid));
	  write(fd,&c,sizeof(c));
	  exit(0);
	 }
	 else
	 { 
	   wait(&k);
	 };
	 wait(&k);
	} 
	 lseek(fd,sizeof(x),0);
	 read(fd,&pid,sizeof(pid));
	 if(pid==ps1) read(fd,&pi,sizeof(pi));
	    else read(fd,&c,sizeof(c));
	 read(fd,&pid,sizeof(pid));
	 if(pid==ps1) read(fd,&pi,sizeof(pi));
	    else read(fd,&c,sizeof(c));
	 printf("pi=%f\n",pi);    
         printf("cos(x)=%f\n",c);
	 printf("x=%f\n",x);
	 f=(1-c)/(pi*x*x);
	 printf("F(x)=%f\n",f);
	 
};
