#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

struct record
{int procdescr;
 long factorial;
} rec1;

void process (int tempfile, long temp)
{long i, factorial; 
 //��������� ���������
 factorial = 1;
 for (i = 2; i <= temp; i++)
    factorial *= i;
 //���������� ��������� �� ��������� ����
 rec1.factorial = factorial;
 rec1.procdescr = getpid ();
 write (tempfile, &rec1, sizeof rec1);
}

int main (int argc, char *argv[])
{int pdn, pdk, pdn_k, tempfile;
 long n, k, temp, fact_n, fact_k, fact_n_k;
 struct stat statbuf;
//�������� �������� ������
 if (argc != 3)
   {printf ("Usage: %s n k\n", argv[0]);
    exit (1);
   }
 if (!sscanf (argv[1], "%ld", &n) || !sscanf (argv[2], "%ld", &k))
   {printf ("Parameters invalid: point of parameters - numbers!\n");
    exit (1);
   }
 // ��� ��� long �� ����� 2147483648, �� ������ ��������
 if (n > 12 || n < 1 || k > 12 || k < 1)
   {printf ("Parameters invalid: point of parameters - from 1 up to 12!\n");
    exit (1);
   }
 // �� ������ ����� ��������� k �� ����� ���� ������ n
 if (n < k)
   {printf ("Parameters invalid: n < k!\n");
    exit (1);
   }

 temp = n;
 //��������� ��������� ����
 tempfile = open ("temp", O_CREAT | O_RDWR, 0x0644);
 //������� �������-������� 1
 pdn = fork ();
printf ("%d adsf\n", pdn);
 //���� ��� �������-�������  
 if (pdn == 0)
   {process (tempfile, temp);
    exit (0);
   }
 //������� �������-������� 2
 temp = k;
 pdk = fork ();
 //���� ��� �������-�������
 if (pdk == 0)
   {process (tempfile, temp);
    exit (0);
   }
 //������� �������-������� 3
 temp = n - k;
 pdn_k = fork ();
 //���� ��� �������-�������
 if (pdn_k == 0)
   {process (tempfile, temp);
    exit (0);
   }
 //�ģ�, ����� � ���� ����� ����������� ��� ������ ����������-���������
 do
 {fstat (tempfile, &statbuf);
 }
 while (statbuf.st_size != sizeof (struct record) * 3);

 lseek (tempfile, 0, SEEK_SET);

 //��������� ������ ������ �� �����
 while (!read (tempfile, &rec1, sizeof rec1));
 if (rec1.procdescr == pdn)
   fact_n = rec1.factorial;
 if (rec1.procdescr == pdk)
   fact_k = rec1.factorial;
 if (rec1.procdescr == pdn_k)
   fact_n_k = rec1.factorial;

 //��������� ������ ������ �� �����
 while (!read (tempfile, &rec1, sizeof rec1));
 if (rec1.procdescr == pdn)
   fact_n = rec1.factorial;
 if (rec1.procdescr == pdk)
   fact_k = rec1.factorial;
 if (rec1.procdescr == pdn_k)
   fact_n_k = rec1.factorial;

 //��������� ������ ������ �� �����
 while (!read (tempfile, &rec1, sizeof rec1));
 if (rec1.procdescr == pdn)
   fact_n = rec1.factorial;
 if (rec1.procdescr == pdk)
   fact_k = rec1.factorial;
 if (rec1.procdescr == pdn_k)
   fact_n_k = rec1.factorial;

 printf ("\nThe numbers of combination = %ld\n", fact_n / (fact_k * fact_n_k));
 remove ("temp");
 sleep (1);
}


