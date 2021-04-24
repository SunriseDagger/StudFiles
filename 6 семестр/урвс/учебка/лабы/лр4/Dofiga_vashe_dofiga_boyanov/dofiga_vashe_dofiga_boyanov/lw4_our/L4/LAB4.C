//#include<stdio.h>
//#include<signal.h>
#include<fcntl.h>
//#include<sys/wait.h>
//#include<unistd.h>
//#include<stdlib.h>
//#include<sys/types.h>

int factorial(int n)
{ int i,s=1;
   for(i=1;i<=n;i++)
   {s=s*i;}
   return s;
};
int main()
{ int nn,r,n[10],i,j,s;
  int fd; 
  pid_t pid[10];
  char *buf;
  int buf1[10];
  printf("Please input N: "); 
  scanf("%d",&n[0]);
  printf("Please input R: ");
  scanf("%d",&r);
  n[r+1]=0;
  for(i=1;i<=r;i++)
  {
    printf("Please input N(%d) : ",i);
    scanf("%d",&n[i]);
    n[r+1]=n[r+1]+n[i];
  };
  if(n[r+1]!=n[0])
  {
    printf("\nWrong parameters!!!\n");
    exit(0);
  };
  if((fd=open("temp",O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
 {  printf("Error in open\n");
     exit(1);
  };
  
  for(i=0;i<=r;i++)
  {
    switch (pid[i]=fork())
    {   
       case -1: perror(" Error in call FORK()\n");
                exit(1);
	        break;
       case 0:  printf("Open child process\n");
                sprintf(buf,"%d ",i);
		s=factorial(n[i]);
		sprintf(&buf[2],"%d\n",s);
		write(fd,buf,sizeof(buf));
		printf("child process is died\n");
		exit(0);
		break;
      default:  if(wait(&s)==pid[i])
                printf("ok%d \n",i);
    };  
  };
    s=1;
    fd=open("temp",O_RDONLY);
    for(i=0;i<=r;i++)
    {
      read(fd,buf,4);
      sscanf(buf,"%d",buf1);
      sscanf(&buf[1],"%d",&buf1[1]);
      if (buf1[0])
       s=s/buf1[1];
      else s=s*buf1[1];
    }   
printf("Rezutat: C=%d\n",s);
close(fd);
}

 