#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

void OnSignal ()
{;
}

int main (int argc, char *argv[])
{long i, factorial, temp;
 int canal[2];
 //���������� � �����������
 sscanf (argv[0], "%d", &canal[0]);
 sscanf (argv[1], "%d", &canal[1]);
 signal (SIGINT, OnSignal);
 pause ();
 read (canal[0], &temp, sizeof temp);

 //��������� ���������
 factorial = 1;
 for (i = 2; i <= temp; i++)
    factorial *= i;

 //���������� ��������� � ����������� �����
 write (canal[1], &factorial, sizeof factorial);

}


