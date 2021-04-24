#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char *argv[])
{int pdn, pdk, pdn_k, canal[2], temp;
 long n, k, fact_n, fact_k, fact_n_k;
 char string1[10], string2[10];

//�������� �������� ������
 if (argc != 3)
   {printf ("Usage: %s n k\n", argv[0]);
    sleep (2);
    exit (1);
   }
 if (!sscanf (argv[1], "%ld", &n) || !sscanf (argv[2], "%ld", &k))
   {printf ("Parameters invalid: point of parameters - numbers!\n");
    sleep (2);
    exit (1);
   }
 // ��� ��� long �� ����� 2147483648, �� ������ ��������
 if (n > 12 || n < 1 || k > 12 || k < 1)
   {printf ("Parameters invalid: point of parameters - from 1 up to 12!\n");
    sleep (2);
    exit (1);
   }
 // �� ������ ����� ��������� k �� ����� ���� ������ n
 if (n < k)
   {printf ("Parameters invalid: n < k!\n");
    sleep (2);
    exit (1);
   }
 //��������� ����������� �����
 pipe (canal);
 sprintf (string1, "%d", canal[0]);
 sprintf (string2, "%d", canal[1]);

 pdn = fork ();
 if (pdn == 0)
   {execlp ("./process", string1, string2, 0);
    exit (0);
   }

 //������� �������-������� 2
 pdk = fork ();
 //���� ��� �������-�������
 if (pdk == 0)
   {execlp ("./process", string1, string2, 0);
    exit (0);
   }

 //������� �������-������� 3
 pdn_k = fork ();
 //���� ��� �������-�������
 if (pdn_k == 0)
   {execlp ("./process", string1, string2, 0);
    exit (0);
   }
sleep (1);
 // �������� �������� ������� �������
 write (canal[1], &n, sizeof n);
 kill (pdn, SIGINT); 
 wait (&temp);
 read (canal[0], &fact_n, sizeof fact_n);

 // �������� �������� ������� �������
 write (canal[1], &k, sizeof k);
 kill (pdk, SIGINT);
 wait (&temp);
 read (canal[0], &fact_k, sizeof fact_k);

 // �������� �������� �������� �������
 n -= k;
 write (canal[1], &n, sizeof n);
 kill (pdn_k, SIGINT);
 wait (&temp);
 read (canal[0], &fact_n_k, sizeof fact_n_k);

 // �������� ���������
 printf ("\nThe numbers of combination = %ld\n", fact_n / (fact_k * fact_n_k));
 sleep (1);
}



