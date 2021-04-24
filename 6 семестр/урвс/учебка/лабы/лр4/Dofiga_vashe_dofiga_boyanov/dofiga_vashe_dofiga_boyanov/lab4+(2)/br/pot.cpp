#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
float r_rand()       // Функция возвращает псевдослучайное число от 0 до 1
{ static char c=0;
  char a=11, b=19, d=50;
  c=(a*c+b)%d;
  return (float)c/(d-1);
}
void errors(int er)   // Функция выдачи ошибок для потомка
{  switch(er)
   { case 1 : printf("Неверно заданно N !\n"); break;
     case 2 : printf("Файл %s не открыт !\n",File); break;
     case 3 : printf("Неверно переданно N !\n"); break;
     case 4 : printf("Неверно переданны данные !\n"); break;
   }
}
void main(int argn,char *argv[])
{ int status=0, i, wri, ka;
  unsigned int n=0;
  float po[1000];
  if(argn > 1) ka = atoi(argv[1]);
  printf("Введите - N (10 sek):\n");
  scanf("%d",&n);                         // Вводим N
  if(n>0 && n<1000)                       // Если N больше 0 и меньше 1000, то... 
  { wri = write(ka,&n,sizeof(unsigned int));// Записываем N
    if(wri == sizeof(unsigned int))         // Если запись успешна, то... 
    { for(i=0; i<n; i++)                       // Генерируем последовательность
        po[i] = r_rand();                      // псевдослучайных чисел всего N 
      wri = write(ka,po,n*sizeof(float));      // Записываем последовательность
      if(wri != n*sizeof(float)) status=3;     // Если запись не успешна, то status=3
    }
    else status=2;                          // Иначе status=2 
  }                                       // Иначе status=1 
  else status=1;                          // Определяем ошибку
  errors(status);                         // Выход 
  exit(status);
}
