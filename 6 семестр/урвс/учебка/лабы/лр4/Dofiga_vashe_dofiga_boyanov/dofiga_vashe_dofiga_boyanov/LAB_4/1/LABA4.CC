#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

struct elem   //struct of data
{
int num_proc;
int result;
};
elem el_1,el_2,el_3;

main(int argc,char *argv[])
{
int f,f3,f1,f2;
int i,j,m,n,k,l,st;
int s1=1,s2=1,s3=1,result;
struct stat info;


printf("Input n \n");scanf("%d",&n);
printf("Input k \n");scanf("%d",&k);

if ((n<0)||(k<0)) {perror("\nBAD DATA\n"); exit(1); }

f1=open("file",O_CREAT|O_WRONLY);
f2=open("file",O_RDONLY);
                                       
  if (fork()==0)                //N!
  {
    for(i=1;i<=n;i++) s1*=i; close(f2);
    el_1.num_proc=1; el_1.result=s1;
    write (f1, &el_1, sizeof el_1);
  exit(0);
  };

if (fork()==0)                 //K!
  {
    for(i=1;i<=k;i++) s2*=i;  close(f2);
    el_2.num_proc=2; el_2.result=s2;
    write(f1,&el_2,sizeof el_2);
    exit(0);
  };

if (fork()==0)                //(N-K)!
  {
    for(i=1;i<=(n-k);i++) s3*=i;  close(f2);
    el_3.num_proc=3; el_3.result=s3;
    write(f1,&el_3,sizeof el_3);  
    exit(0);
  } 
 
 do
 {
  stat ("file", &info);
 } while (info.st_size < 3 * sizeof el_1);

   elem el; int pr;
       for (i = 0; i < 3; i++)
          {  pr=read (f2, &el, sizeof el);
       		if (el.num_proc == 1)
		  el_1 = el;
		else 
		  if (el.num_proc == 2) 
		    el_2 = el;
		  else 
		    el_3 = el;
          } 
    	  
result = el_1.result / (el_2.result * el_3.result);
printf ("\nresultat=%d\n", result);
wait(&st);
wait(&st);
wait(&st);
sleep (1);
close (f1);
close (f2);
remove ("file");
exit(0);
}

