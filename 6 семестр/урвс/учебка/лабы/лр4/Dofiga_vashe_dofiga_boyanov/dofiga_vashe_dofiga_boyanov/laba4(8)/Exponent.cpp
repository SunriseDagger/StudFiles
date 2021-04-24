#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>      
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

main(int argc,char *argv[])
{ char rez[sizeof(int)+sizeof(double)]; 

  double x,x_t,Expon = 1,temp=1;
  int i,pipeFd;
  if(argc != 3) {printf("Use exponent.cpp with 2 parametrs\n");
                 exit(1);}
   x = atof(argv[1]); 
   pipeFd = atoi(argv[2]); 

   x_t=(-1)*x*x/2.;

   for(i=1;i<200;i++) 
	    {
	    temp  *= x_t/i;
	    Expon+=temp;
	    }
  
  ((int*)rez)[0] = 2;       
  ((double*)(rez+sizeof(int)))[0] = Expon;
  
  write(pipeFd,rez,sizeof(int)+sizeof(double));
  while(1) pause;  
}

