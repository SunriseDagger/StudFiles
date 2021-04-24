#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>      
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<math.h>

int flag = 1; 

double pow(double x,int st)//Вычисляет x в степени st
{double rez =1;
 for(;st>0;st--) rez *=x;	
 return rez;
} 


void potomok1()
{ char c[20];
  float y=1;
  int i=1;
int str = open("tmp1",O_RDONLY);
  do
   { read(tmp,&y,4);
    if (y==1) printf("\n!!STOP");
    else  {
/////////////////////////////////////////
{ char rez[sizeof(int)+sizeof(double)];

  double pi = 0,sign = 1,x = 0.577350269;//x == 1/sqrt(3)
  int i,pipeFd;

  if(argc != 2) {printf("Use pi.c with one parametr\n");
                 exit(1);}// Проверяем верность числа параметров переданных при вызове

  // вычисляем число pi с помощью разложения в ряд
  for(i=1;i<100;i+=2) {pi += (sign*pow(x,i))/i;sign *= -1;}
  pi *= 6;  

  printf(" pi = %.10f\n",pi);

  ((int*)rez)[0] = 1; //Записать в структуру rez номер процесса(условный)
  ((double*)(rez+sizeof(int)))[0] = pi;//Записать в структуру rez значение  pi

  pipeFd = atoi(argv[1]);//Сохраняем файловый дескриптор для записи в канал

  write(pipeFd,rez,sizeof(int)+sizeof(double)); // Записать в канал открытый предком структуру rez


//  while(1) pause;//Ожидать сигнала на завершение
  }
/////////////////////////////////////////  
 //  } while(y!=1);
 }


void potomok2()
{ char c[20];
  float y=1;
  int i=1;
int str = open("tmp2",O_RDONLY);
  do
   { read(tmp,&y,4);
    if (y==1) printf("\n!!STOP");
    else   { { char rez[sizeof(int)+sizeof(double)]; 

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
//  while(1) pause;  

           } 
   } while(y!=1);
 }


static void sig_start(int signo)
{flag = 0;}   

static void died_potomok(int signo)
{printf("potomok's died\n");}

main(int argc,char *argv[])
{ int pid,pid1,pid2,procNumber,i;
  int pipeFd[2];
  double rez,Expo = 1000,pi=1000,x;
  char buf[sizeof(int)+sizeof(double)];
  char argument[sizeof(int)+1];
  
  signal(SIGUSR1,sig_start);        
  signal(SIGINT,died_potomok);      

  if(argc != 2)  { printf("Use laba4 with 1 parametr - x\n");
		   exit(1);}

  x = atof(argv[1]); 		   
	      
  printf(" make a pipe\n");
  if(pipe(pipeFd) !=0) {printf("Can't build a pipe");
                        exit(1);}
			
  sprintf(argument,"%d",pipeFd[1]);
			
  printf("starting process1\n");
  pid = fork();          
  if(pid == -1) {printf(" Can't start process 1");
                  exit(1);}

  if(pid == 0) {
                 while(flag) pause; 
              //   execlp("./pi","./pi",argument,NULL);
                  potomok1();
		  printf(" Can't execute my file");
                 exit(1);}
                 
   else pid1 = pid;  
                             
  printf(" starting process2\n");
  pid = fork();      
  if(pid == -1) {printf(" Can't start process 2");
                  exit(1);}
  if(pid == 0) {
                 while(flag) pause;
               //  execlp("./Exponent","./Exponent",argv[1],argument,NULL);
                  potomok2();
                  printf(" Can't execute my file");
                 exit(1);}
   else pid2 = pid; 
   
  kill(pid1,SIGUSR1);        
  kill(pid2,SIGUSR1); 

  for(i=0;i<2;i++)
   {
    read(pipeFd[0],&buf,sizeof(int)+sizeof(double));
       
    procNumber = *((int *)buf);   
    rez = *((double *)(buf +sizeof(int)));
    if(procNumber == 1) pi  = rez;        
    else if(procNumber == 2) Expo = rez;
   }
  kill(pid1,SIGINT); 
  kill(pid2,SIGINT); 

  rez = (Expo) / (sqrt(pi*2.)); 
			
  printf(" f(%s) = %.10f\n",argv[1],rez); 
}
