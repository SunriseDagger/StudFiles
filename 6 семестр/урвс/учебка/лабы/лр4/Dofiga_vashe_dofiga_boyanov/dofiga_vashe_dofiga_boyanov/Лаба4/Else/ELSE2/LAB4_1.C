#include<stdio.h>
#include<fcntl.h>

#define NPI 500	// �������� ���������� ��
#define NEXP 10	// �������� ���������� ����������
#define EXP	0
#define PI	1

struct rec	// ��������� ��� �������� ����������� ������ ����������� ��-��
{
  double data;
  int type;
};

int main()
{
  double pi, exp2;
  unsigned i, fd;
  long s;
  struct rec r;
  fd=creat("temp.tmp",0666);		// ���� ��� ������ �������
  if (fd==-1) { perror("Error"); exit(1); }
  if (!fork())		// ��������� ������� ��� ���������� ����������
  { 
    double fact=1, t=1;

      exp2 = 1;
      for (i=1;i<NEXP;i++) 
      {
        t *= 2;
        fact *= i;
        exp2 += t/fact;
      } 
    r.data = exp2; 
    r.type = EXP;
    lseek(fd, 0, SEEK_END);	// ��������� � ����� �����
    write(fd, &r, sizeof(r));	// ����� � ���� ��������� ����������
    close(fd); 
    exit(0);
  }
  else
    if (!fork())	// ��������� ������� ��� ���������� ���������� 
    {
      pi=0;
      for (i=0;i<NPI;i++) 
        if (i%2) pi-=1./(2*i+1);  
            else pi+=1./(2*i+1);
      r.data=pi*4; r.type=PI;
      lseek(fd, 0, SEEK_END);	// ��������� � ����� �����
      write(fd, &r, sizeof(r));	// ����� � ���� ��������� ����������
      close(fd);
      exit(0);
    }
  close(fd);
// ���������� �������� ��������� ���������� �� �����
  fd=open("temp.tmp",O_RDONLY);
  for(i=0; i<2; )	// ��� ������
  {
    s=read(fd, &r, sizeof(r)); // ����� �����
    if (s)	// ���� ���-�� ���������
    {
      i++;
      if (r.type==EXP) exp2 = r.data;
                  else pi=r.data;
    }
  }
  close(fd);
  printf("exp2 = %lf\n", exp2 );
  printf("pi = %lf\n", pi );
  printf("Result: %lf\n", 1./( pi*(1+exp2) ) );
  exit(0);
}
