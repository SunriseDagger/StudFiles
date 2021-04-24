#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>      
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

double pow(double x,int st)//������ x � �⥯��� st
{double rez =1;
 for(;st>0;st--) rez *=x;	
 return rez;
} 

main(int argc,char *argv[])
{ char rez[sizeof(int)+sizeof(double)];

  double pi = 0,sign = 1,x = 0.577350269;//x == 1/sqrt(3)
  int i,pipeFd;

  if(argc != 2) {printf("Use pi.c with one parametr\n");
                 exit(1);}// �஢��塞 ��୮��� �᫠ ��ࠬ��஢ ��।����� �� �맮��

  // ����塞 �᫮ pi � ������� ࠧ������� � ��
  for(i=1;i<100;i+=2) {pi += (sign*pow(x,i))/i;sign *= -1;}
  pi *= 6;  

  printf(" pi = %.10f\n",pi);

  ((int*)rez)[0] = 1; //������� � �������� rez ����� �����(�᫮���)
  ((double*)(rez+sizeof(int)))[0] = pi;//������� � �������� rez ���祭��  pi

  pipeFd = atoi(argv[1]);//���࠭塞 䠩���� ���ਯ�� ��� ����� � �����

  write(pipeFd,rez,sizeof(int)+sizeof(double)); // ������� � ����� ������ �।��� �������� rez


  while(1) pause;//������� ᨣ���� �� �����襭��
}

