#include<sys/types.h>
#include<math.h>
#include<stdio.h>
#include<signal.h>
#include<fcntl.h>
#include<unistd.h>
int factorial(int n)
{int i,s=1;
 for(i=1;i<=n;i++)
 s=s*i;
 printf("%d!=%d\n",n,s);
 return s;
};

int main(argc,argv)
int argc;
char *argv[];
{
  int i,n,fd,f;
  char *buf;
  sscanf(argv[1],"%d %d",&fd,&n); 
//  printf("fd=%d n=%d\n",fd,n);
  f=factorial(n);
  sprintf(buf,"%d %d\n",getpid(),f);
//  printf("buf=%s",buf);
  write(fd,buf,12);
 printf("----------------------------------------------------------\n");
  exit(0);
}

 
 
  
  
 
 