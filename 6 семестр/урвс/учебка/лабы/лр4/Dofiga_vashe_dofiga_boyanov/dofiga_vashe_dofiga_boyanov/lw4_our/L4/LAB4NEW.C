#include<fcntl.h>
#include<stdio.h>
int factorial(int n)
{ int i,s=1;
  for(i=1;i<=n;i++)
  s=s*i;
  return s;
}
main() 
{ int n[20],r;  
  int i,s,s1,status,buf1[2]; 
  int fd,pid[20];  
  char buf[20];
  
  printf("Please enter N: "); 
  scanf("%d",&n[0]);
  if((n[0]<0)||(n[0]>30))
  { printf("\nWRONG!!\n");exit(0);}
  printf("Please enter R: ");
  scanf("%d",&r);
  if((r<=0)||(r>20)) {printf("\nWRONG!!\n");exit(0);}; 
  n[r+1]=0;
  for(i=1;i<=r;i++)
  {  printf("Please enter N(%d): ",i);   
     scanf("%d",&n[i]);
     if((n[i]<0)||(n[i]>30)){ printf("\nWrong\n");exit(1);}
     n[r+1]+=n[i];
  };
  if(n[r+1]!=n[0])// Proverka
  {printf("Wrong parameters!!!\n");exit(0);} 
  // Open file for writing
  if((fd=open("temp",O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)  
  { printf("Error in open\n");
    exit(0);
  }
  for(i=0;i<=r;i++)  
  {   if((pid[i]=fork())==0) 
    {/*Child process*/    
     printf("%d Child process ID %d\n",i,getpid());
     sprintf(buf,"%5d %-5d\n",getpid(),factorial(n[i]));
     write(fd,buf,12);
     exit(0);
    };
  };
   printf("Parent process wait\n");
  /*Parent process wait*/
  for(i=0;i<=r;i++)
   wait(&status);
  printf("OK\n");
  
  // Open file for Reading
  fd=open("temp",O_RDONLY);
  s=1;
  for(i=0;i<=r;i++)
  {
    read(fd,buf,12); 
    sscanf(buf,"%d %d",buf1,&buf1[1]);
    if(buf1[0]==pid[0])
      s1=buf1[1];// N!
    else
      s=s*buf1[1];// N[i]!
  };
  s=s1/s;
  printf("Rezultat: C=%d \n",s);
  close(fd);
}     
   
