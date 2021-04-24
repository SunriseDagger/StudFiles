#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>      
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/wait.h>

double pow(double x,int st)//������ x � �⥯��� st
{double rez =1;
 for(;st>0;st--) rez *=x;	
 return rez;
} 


 int main(int argc,char *argv[])
{ 
  int tmp=creat("tmp.tmp",0666);
  int pid1,pid2, procNumber;
  double rezult,Expo = 1000,x,pi;
  char buf[sizeof(int)+sizeof(double)],rez[sizeof(int)+sizeof(double)];
 
/*  if(argc != 2)  { printf("Use laba4 with 1 parametr - x\n");
 		   exit(1);}*/
  x=atof(argv[1]);	 

  printf("starting process1\n");

  if(!(pid1=fork()) ) {
   double pin = 0,sign = 1,y = 0.577350269;//x == 1/sqrt(3)
  
  for(int i=1;i<100;i+=2) {pin += (sign*pow(y,i))/i;sign *= -1;}
  pin *= 6;  
  printf(" pi = %.10f\n",pin);

  ((int*)rez)[0] = 1; //������� � �������� rez ����� �����(�᫮���)
  ((double*)(rez+sizeof(int)))[0] = pin;//������� � �������� rez ���祭��  pi

  lseek(tmp, 0,SEEK_END);
  write(tmp,rez,sizeof(int)+sizeof(double)); // ������� � ����� ������ �।��� �������� rez
   close(tmp);
     exit(0);
}
                         
  printf(" starting process2\n");
  
  if(!(pid2=fork())) {
    double  x_t,Expon = 1,temp=1;
   printf("x= %f\n",x);
   x_t=(-1)*x*x/2.;

   for(int i=1;i<200;i++) 
	    {
	    temp  *= x_t/i;
	    Expon+=temp;
	    }
  printf("Exp = %f\n",Expon);
  ((int*)rez)[0] = 2;       
  ((double*)(rez+sizeof(int)))[0] = Expon;
  lseek(tmp ,0,SEEK_END);  
  write(tmp,rez,sizeof(int)+sizeof(double));             
   close(tmp);
   exit(0);
   }
    pid1= wait(0);
    pid2= wait(0);
   
  close(tmp);

//  printf("!!!!!!!\n"); 
 tmp=open("tmp.tmp",O_RDONLY);
  for(int i=0;i<2;i++)
   {
    read(tmp,&buf,sizeof(int)+sizeof(double));
     
    procNumber = *((int *)buf);   
    rezult = *((double *)(buf +sizeof(int)));
    if(procNumber == 1){ pi  = rezult;   printf("%f\n",pi);     }
    else if(procNumber == 2){ Expo = rezult; printf("%f\n",Expo);   }}
//  kill(pid1,SIGINT); 
 // kill(pid2,SIGINT);
  printf("!!!!!!!\n"); 
  close(tmp);
  rezult = (Expo) / (sqrt(pi*2.)); 
		
  printf(" f(%s) = %.10f\n",argv[1],rezult); 
return 0;
//getch();
}
