#include<stdio.h>
#include<fcntl.h>

#define NPI 500	// Параметр вычисления пи

int main(int argc, char *argv[])
{
  int Fd;
  double pi=0;
  unsigned i;

  sscanf(argv[0],"%d",&Fd);	// получаем ид-ор канала

  for (i=0;i<NPI;i++) 
    if (i%2) pi-=1./(2*i+1);  
      else pi+=1./(2*i+1);
  pi=pi*4;

  printf("Pi calculated!\n");
  write(Fd, &pi, sizeof(double) );  // вывод в канал
}
