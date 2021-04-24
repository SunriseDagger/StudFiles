#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
char *File="test";
float r_rand()     // ������� ���������� ��������������� ����� �� 0 �� 1
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
float fun(float x){ return 4.0/(1.0+x*x); } // �������� �������
void potomok()                         // �������
{ int status=0, i, wri, fail;
  unsigned int n;
  float po[1000];
  printf("%s\n","������� - N");
  scanf("%d",&n);                      // ������ N
  if(n>0 && n<1000)                    // ���� N ������ 0 � ������ 1000, ��... 
  { fail = open(File,O_WRONLY | O_CREAT,0600); // ������� ���� ��� ������
    if( fail > 0)                              // ���� �� ������, ��...
    { wri = write(fail,&n,sizeof(unsigned int));  // ���������� N
      if(wri == sizeof(unsigned int))             // ���� ������ �������, ��...
      { for(i=0; i<n; i++)                           // ���������� ������������������    
           po[i] = r_rand();                         // ��������������� ����� ����� N
        wri = write(fail,po,n*sizeof(float));        // ���������� ������������������
        if(wri != n*sizeof(float)) status=4;         // ���� ������ �� �������, �� status=4
      }
      else status=3;                              // ����� status=3  
      close(fail);                                // ��������� ���� 
    }
    else status=2;                             // ����� status=2 
  }
  else status=1;                       // ����� status=1 
  errors(status);                      // ���������� ������
  exit(status);                        // ����� 
}
void roditel()                         // �������� 
{ int status, i, j, fail;
  unsigned int n=0;
  float po[1000], max, rea;
  wait(&status);                       // �������� ���������� �������
  if(!status)                          // ���� �� ���������� �������, ��...
  { fail = open(File,O_RDONLY);              // ��������� ���� ��� ������
    rea = read(fail,&n,sizeof(unsigned int));// ������ N
    if(rea == sizeof(unsigned int))          // ���� ������ �������, ��...
    { rea = read(fail,po,n*sizeof(float));      // ������ ������������������
      unlink(File);                             // ������� ���� 
      if(rea == n*sizeof(float))                // ���� ������ �������, ��...
      { for(i=0; i<n-1; i++)                       // ������������� ������������������  
          for(j=i+1; j<n; j++)
            if(po[i] > po[j])
            { max=po[i];
              po[i]=po[j];
              po[j]=max;
            }
        max=0;	                                   // ������� ��������
        for(i=0; i<n-1; i++)
          max+=(fun(po[i+1])+fun(po[i]))*(po[i+1]-po[i])/2.0; 	
        printf("�������� = %f\n",max);             // ����� ����������
      }
      else printf("������� ��������� ������!\n");    
    }
    else printf("������� ��������� N!\n");    
  }
}

void main ()
{ int pid;
  pid=fork();                                 // ������� �������
  if(pid==-1) printf("������� �� ������!\n"); // ���� �� ������ - ������
  else                                        // �����      
  { if(pid==0) potomok();                        // ������� 
    else roditel();                              // ��������
  }
  printf("\n");
}
