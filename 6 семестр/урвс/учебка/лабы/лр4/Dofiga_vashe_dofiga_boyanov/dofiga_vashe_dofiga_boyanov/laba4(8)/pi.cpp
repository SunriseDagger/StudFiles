#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>      
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

double pow(double x,int st)//Вычисляет x в степени st
{double rez =1;
 for(;st>0;st--) rez *=x;	
 return rez;
} 

main(int argc,char *argv[])
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


  while(1) pause;//Ожидать сигнала на завершение
}

