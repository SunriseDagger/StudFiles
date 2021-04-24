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
 //вычисляем факториал
 factorial = 1;
 for (i = 2; i <= temp; i++)
    factorial *= i;
 //записываем результат во временный файл
 rec1.factorial = factorial;
 rec1.procdescr = getpid ();
 write (tempfile, &rec1, sizeof rec1);
}

int main (int argc, char *argv[])
{int pdn, pdk, pdn_k, tempfile;
 long n, k, temp, fact_n, fact_k, fact_n_k;
 struct stat statbuf;
//проверка исходных данных
 if (argc != 3)
   {printf ("Usage: %s n k\n", argv[0]);
    exit (1);
   }
 if (!sscanf (argv[1], "%ld", &n) || !sscanf (argv[2], "%ld", &k))
   {printf ("Parameters invalid: point of parameters - numbers!\n");
    exit (1);
   }
 // так как long не более 2147483648, то делаем проверки
 if (n > 12 || n < 1 || k > 12 || k < 1)
   {printf ("Parameters invalid: point of parameters - from 1 up to 12!\n");
    exit (1);
   }
 // по смыслу числа сочетаний k не может быть больше n
 if (n < k)
   {printf ("Parameters invalid: n < k!\n");
    exit (1);
   }

 temp = n;
 //открываем временный файл
 tempfile = open ("temp", O_CREAT | O_RDWR, 0x0644);
 //создаём процесс-потомок 1
 pdn = fork ();
printf ("%d adsf\n", pdn);
 //если это процесс-потомок  
 if (pdn == 0)
   {process (tempfile, temp);
    exit (0);
   }
 //создаём процесс-потомок 2
 temp = k;
 pdk = fork ();
 //если это процесс-потомок
 if (pdk == 0)
   {process (tempfile, temp);
    exit (0);
   }
 //создаём процесс-потомок 3
 temp = n - k;
 pdn_k = fork ();
 //если это процесс-потомок
 if (pdn_k == 0)
   {process (tempfile, temp);
    exit (0);
   }
 //ждём, когда в файл будут произведены все записи процессами-потомками
 do
 {fstat (tempfile, &statbuf);
 }
 while (statbuf.st_size != sizeof (struct record) * 3);

 lseek (tempfile, 0, SEEK_SET);

 //считываем первую запись из файла
 while (!read (tempfile, &rec1, sizeof rec1));
 if (rec1.procdescr == pdn)
   fact_n = rec1.factorial;
 if (rec1.procdescr == pdk)
   fact_k = rec1.factorial;
 if (rec1.procdescr == pdn_k)
   fact_n_k = rec1.factorial;

 //считываем вторую запись из файла
 while (!read (tempfile, &rec1, sizeof rec1));
 if (rec1.procdescr == pdn)
   fact_n = rec1.factorial;
 if (rec1.procdescr == pdk)
   fact_k = rec1.factorial;
 if (rec1.procdescr == pdn_k)
   fact_n_k = rec1.factorial;

 //считываем третью запись из файла
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


