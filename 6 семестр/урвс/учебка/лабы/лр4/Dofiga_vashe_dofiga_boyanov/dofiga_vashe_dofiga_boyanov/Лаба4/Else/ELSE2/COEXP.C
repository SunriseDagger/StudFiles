#include<stdio.h>
#include<fcntl.h>

#define NEXP 30	// Параметр вычисления экспоненты

int main(int argc, char *argv[])
{
  int Fd;
  double exp2;
  unsigned i;
  double fact=1, t=1;

  sscanf(argv[0],"%d",&Fd);	// получаем ид-ор канала
    exp2 = 1;
    for (i=1;i<NEXP;i++) 
    {
      t *= 2;
      fact *= i;
      exp2 += t/fact;
    } 

  printf("Exp calculated!\n");
  write(Fd, &exp2, sizeof(double) );  // вывод в канал
}
