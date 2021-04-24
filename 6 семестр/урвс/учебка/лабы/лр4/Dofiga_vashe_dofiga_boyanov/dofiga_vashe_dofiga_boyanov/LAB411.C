#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
void sign(int z)
{
;
}
void main()
{
   int r,N,k,i=1,pid,fd[2],xq;
   float f,g,p,x,t;
   char str1[10],str2[10];
   printf("enter N=");
   scanf("%d",&N);
   printf("enter X=");
   scanf("%f",&x);
    /*Parent*/
   pipe(fd); //open canal for reading and writing
   t=x;
   for (k=1;k!=N;k++)
   {
       f=f+pow(-1,k+1)*t;
       t=t*x*x/(2*k*(2*k+1));
       write (fd[1],&f,4);
   }
   f=0.;
   write (fd[1],&f,4);
   if (fork()==0)
   {  /*Child*/
      xq=getpid();
      signal(30,sign);
      printf("start child\n");
      //writing inode of canal
      sprintf(str1,"%d",fd[0]);
      sprintf(str2,"%d",fd[1]);
      //end signal
      signal(9,sign);
      execl("/root/LAB42.out","LAB42.out",str1,str2,0);      
   }
   //parent continue    
      kill(xq,30);
      sleep(5);
      kill(xq,9);
      printf("End of child\n");
      kill(pid,1);
      wait(&k); 
}
