#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>

int work = 0;

// функция реакции на сигнал к работе
void LGS(int num)
{
  printf("Let's get started!! <process %d>, num = %d\n", getpid(), num);
  work++;
}

int main()
{
  int ch1[2], ch2[2], pid1, pid2;
  char arg1[20], arg2[20];
  double pi, exp2;
  unsigned i;
  long s;

// создаем каналы
  printf("Building pipes\n");
  pipe(ch1);
  pipe(ch2);

  switch ( pid1=fork() )
  { 
  case -1:	// в случае ошибки
    printf("Error spawn 1st process");
    exit(1);
    break;

  default:  printf("pid1 = %d\n", pid1);  // для исходного процесса
    break;

  case 0:   	// для порожденного процесса
      signal(11, LGS);	// функция-обработчик сигнала
      while ( !work ) sleep(1);	// ожидание сигнала 11

      sprintf(arg1, "%d", ch1[1]);
      execl("coexp", arg1, 0);	// запуск программы вычисления экспоненты
      pause();	// ожидание сигнала на завершение процесса
    break;        
  }
  sleep(1);
  kill(pid1, 11);  // сигнал первому потомку к началу работы

  switch ( pid2=fork() )
  {
  case -1:	// в случае ошибки
    printf("Error spawn 2nd process");
    exit(1);
    break;

  default:  printf("pid2 = %d\n", pid2);  // для исходного процесса
    break;

  case 0:     // для порожденного процесса
      signal(22, LGS);

      while ( !work ) sleep(1);

      sprintf(arg2, "%d", ch2[1]);
      execl("copi", arg2, 0);     
      pause();
    break;
  }

  sleep(1);
  kill(pid2, 22);

// ждем данных от первого потомка
  while ( read(ch1[0], &exp2, sizeof(double))==0 )  sleep(1);
  kill(pid1, 10);	// сигнал к завершению

// ждем данных от второго потомка
  while ( read(ch2[0], &pi, sizeof(double))==0 )  sleep(1);
  kill(pid2, 20);	// сигнал к завершению

// вывод результатов
  printf("exp2 = %lf\n", exp2 );
  printf("pi = %lf\n", pi );
  printf("Result: %lf\n", 1./( pi*(1+exp2) ) ); 
 
}
