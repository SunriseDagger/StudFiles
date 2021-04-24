#include<stdio.h>
//#include<conio.h>
#include<math.h>
#include<fcntl.h>
#include<string.h>
//  потомок
void potomok()
{
	char c[20];
  double y;
  FILE * stream=fopen("qwe","r");
// пока не стоп читаем из файла  
while (strcmp("STOP",c)!=0)
   { if(fscanf(stream,"%s",&c)!=-1)
                  printf("\n%s",c);
   } 
}

int main()
{
 int N,k=1,pid;
  double x=0,x1=0,slx,xp,xp1;
  FILE * stream=fopen("qwe","w");
// инициализация потомка
 pid=fork();
 if (pid==0) { potomok(); exit(1); }
  // вычисления
for(N=1;N<=9;N++)
  {
 slx=3.14*k/N;
    for(k=1;k<=N;k++)
     x=pow(slx,2*k+1)/(2*k+1);
     
  fprintf(stream,"%f\n",x);
 }
  fprintf(stream,"STOP");
  fclose(stream);
  return(1);
}
