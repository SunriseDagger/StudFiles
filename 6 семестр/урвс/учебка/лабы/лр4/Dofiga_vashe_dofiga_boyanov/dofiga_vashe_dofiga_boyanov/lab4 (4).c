#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
void main()
{
   int r,N,k,fd,i=1;
   float f,g,p,x,t;
   printf("enter N=");
   scanf("%d",&N);
   printf("enter X=");
   scanf("%f",&x);
   if (fork())
   {  /*Parent*/
      fd=open("a.txt",O_WRONLY|O_CREAT);
      t=x;
      for (k=1;k!=N;k++)
      {  
         f=f+pow(-1,k+1)*t;
         t=t*x*x/(2*k*(2*k+1));
         write (fd,&f,4);
      }
      f=0.;
      write (fd,&f,4);
      close (fd);
      wait(&k);
   }
   else
   {  /*Child*/
      fd=open("a.txt",O_RDONLY);
      while ((g!=0)||(i=0))
      {
         i=read(fd,&g,4);
         if (g!=0.) printf("%f\n",g);
      }
         exit(0);
   }
}
