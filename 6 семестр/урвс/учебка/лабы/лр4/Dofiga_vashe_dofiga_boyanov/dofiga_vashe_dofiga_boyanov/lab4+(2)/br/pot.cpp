#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
float r_rand()       // ������� ���������� ��������������� ����� �� 0 �� 1
{ static char c=0;
  char a=11, b=19, d=50;
  c=(a*c+b)%d;
  return (float)c/(d-1);
}
void errors(int er)   // ������� ������ ������ ��� �������
{  switch(er)
   { case 1 : printf("������� ������� N !\n"); break;
     case 2 : printf("���� %s �� ������ !\n",File); break;
     case 3 : printf("������� ��������� N !\n"); break;
     case 4 : printf("������� ��������� ������ !\n"); break;
   }
}
void main(int argn,char *argv[])
{ int status=0, i, wri, ka;
  unsigned int n=0;
  float po[1000];
  if(argn > 1) ka = atoi(argv[1]);
  printf("������� - N (10 sek):\n");
  scanf("%d",&n);                         // ������ N
  if(n>0 && n<1000)                       // ���� N ������ 0 � ������ 1000, ��... 
  { wri = write(ka,&n,sizeof(unsigned int));// ���������� N
    if(wri == sizeof(unsigned int))         // ���� ������ �������, ��... 
    { for(i=0; i<n; i++)                       // ���������� ������������������
        po[i] = r_rand();                      // ��������������� ����� ����� N 
      wri = write(ka,po,n*sizeof(float));      // ���������� ������������������
      if(wri != n*sizeof(float)) status=3;     // ���� ������ �� �������, �� status=3
    }
    else status=2;                          // ����� status=2 
  }                                       // ����� status=1 
  else status=1;                          // ���������� ������
  errors(status);                         // ����� 
  exit(status);
}
