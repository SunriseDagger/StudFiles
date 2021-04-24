#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
char *File="test";
float r_rand()     // Функция возвращает псевдослучайное число от 0 до 1
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
float fun(float x){ return 4.0/(1.0+x*x); } // Заданная функция
void potomok()                         // Потомок
{ int status=0, i, wri, fail;
  unsigned int n;
  float po[1000];
  printf("%s\n","Введите - N");
  scanf("%d",&n);                      // Вводим N
  if(n>0 && n<1000)                    // Если N больше 0 и меньше 1000, то... 
  { fail = open(File,O_WRONLY | O_CREAT,0600); // Создаем файл для чтения
    if( fail > 0)                              // Если он создан, то...
    { wri = write(fail,&n,sizeof(unsigned int));  // Записываем N
      if(wri == sizeof(unsigned int))             // Если запись успешна, то...
      { for(i=0; i<n; i++)                           // Генерируем последовательность    
           po[i] = r_rand();                         // псевдослучайных чисел всего N
        wri = write(fail,po,n*sizeof(float));        // Записываем последовательность
        if(wri != n*sizeof(float)) status=4;         // Если запись не успешна, то status=4
      }
      else status=3;                              // Иначе status=3  
      close(fail);                                // Закрываем файл 
    }
    else status=2;                             // Иначе status=2 
  }
  else status=1;                       // Иначе status=1 
  errors(status);                      // Определяем ошибку
  exit(status);                        // Выход 
}
void roditel()                         // Родитель 
{ int status, i, j, fail;
  unsigned int n=0;
  float po[1000], max, rea;
  wait(&status);                       // Ожидание завершения потомка
  if(!status)                          // Если он завершился успешно, то...
  { fail = open(File,O_RDONLY);              // Открываем файл для чтения
    rea = read(fail,&n,sizeof(unsigned int));// Читаем N
    if(rea == sizeof(unsigned int))          // Если чтение успешно, то...
    { rea = read(fail,po,n*sizeof(float));      // Читаем последовательность
      unlink(File);                             // Удаляем файл 
      if(rea == n*sizeof(float))                // Если чтение успешно, то...
      { for(i=0; i<n-1; i++)                       // Упорядочиваем последовательность  
          for(j=i+1; j<n; j++)
            if(po[i] > po[j])
            { max=po[i];
              po[i]=po[j];
              po[j]=max;
            }
        max=0;	                                   // Считаем интеграл
        for(i=0; i<n-1; i++)
          max+=(fun(po[i+1])+fun(po[i]))*(po[i+1]-po[i])/2.0; 	
        printf("Интеграл = %f\n",max);             // Вывод результата
      }
      else printf("Неверно прочитаны данные!\n");    
    }
    else printf("Неверно прочитано N!\n");    
  }
}

void main ()
{ int pid;
  pid=fork();                                 // Создаем потомка
  if(pid==-1) printf("Потомок не создан!\n"); // Если не создан - ошибка
  else                                        // Иначе      
  { if(pid==0) potomok();                        // Потомок 
    else roditel();                              // Родитель
  }
  printf("\n");
}
