#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<signal.h>
char sig1=1, sig2=1;
int ka[2];
float fun(float x)
{ return 4.0/(1.0+x*x); }
void sign_fun1(int sig)
{ char arg[10]; 
  int pid;
  sig1 = 0;
  sprintf(arg,"%d",ka[1]);
  pid = fork();
  if(pid == 0) execl("./pot","./pot",arg,0);
}
void sign_fun2(int sig){ sig2 = 0; }
void potomok()                           // Потомок
{ printf("Ожидание сигнала для начала\n");
  signal(SIGUSR1, sign_fun1);            // Обработка сигнала 1 
  while(sig1)sleep(1);
  printf("Ожидание сигнала для завершения\n");
  signal(SIGUSR2, sign_fun2);            // Обработка сигнала 2 
  while(sig2)sleep(1);
  exit(0);
}
void roditel(int pid)                    // Родитель
{ int status, rea, i, j;
  unsigned int n;
  float po[1000], max;
  sleep(2);                              // Ожидаем 2 сек.
  kill(pid,SIGUSR1);                     // Посылка сигнала для начала
  sleep(10);                             // Ожидание 10 сек.
  kill(pid,SIGUSR2);                     // Посылка сигнала для завершения
  wait(&status);                         // Ожидание завершения потомка
  if(!status)                            // Если он завершился успешно, то...
  { rea = read(ka[0],&n,sizeof(unsigned int));// Читаем N
    if(rea == sizeof(unsigned int))           // Если чтение успешно, то...
    { rea = read(ka[0],po,n*sizeof(float));     // Читаем последовательность
      if(rea == n*sizeof(float))                // Если чтение успешно, то...
      { for(i=0; i<n-1; i++)                      // Упорядочиваем последовательность
          for(j=i+1; j<n; j++)
            if(po[i] > po[j])
            { max=po[i];
              po[i]=po[j];
              po[j]=max;
            }
        max=0;	                                  // Считаем интеграл
        for(i=0; i<n-1; i++)
          max+=(fun(po[i+1])+fun(po[i]))*(po[i+1]-po[i])/2.0; 	
        printf("Интеграл = %f\n",max);            // Вывод результата
      }
      else printf("Неверно прочитаны данные!\n");    
    }
    else printf("Неверно прочитано N!\n");    
  }
}

void main ()
{ int pid;
  pipe(ka);
  pid=fork();
  if(pid==-1) printf("Потомок не создан!\n");
  else
  { if(pid==0) potomok();
    else roditel(pid);
  }
  printf("\n");
}
