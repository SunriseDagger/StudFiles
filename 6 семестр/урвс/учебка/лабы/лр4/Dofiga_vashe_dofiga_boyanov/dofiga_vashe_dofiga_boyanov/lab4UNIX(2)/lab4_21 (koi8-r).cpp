#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>

int flag = 1;

void sigcatcher (int) //����� ������� �� �������� ��� ������ ��������
{
	flag = 0;
}

int main()
{
	int i, j, k, n, r, ft, status;
	int *mas;

	int znam[2];
	int chisl[2];

	char chislo[10], tmp[10];

	pipe (znam);	//����� ��� ����������� � ����������� �����
	pipe (chisl);	//����� ��� ���������� � ��������� �����

//���� �������� ������
	printf ("����� ���������� n ��������� � r �������\n");
	printf ("������� n (1 - 12) ");
	scanf ("%d", &n);
	if (n > 12)
	{
		printf ("��������� ����� n ������� �� ������� int. �������� ����� n �� 1 �� 12\n");
		return 1;
	}
	if (n < 0)
	{
		printf ("����� n �� ����� ���� �������������\n");
		return 1;
	}
	printf ("������� r ");
	scanf ("%d", &r);

	mas = new int[r];

	status = 0;
	for (i = 0; i < r; i++)
	{
		printf ("������� ����� ��������� � ������ %d ", i);
		scanf ("%d", &mas[i]);
		if (mas[i] < 0)
		{
			printf ("����� n(i) �� ����� ���� ��������������\n");
			return 1;
		}
		status += mas[i];
	}

//�������� �� ������������ �������� ������
	if (status != n)
	{
		printf ("����� n(i) �� ����� n\n");
		return 1;
	}

	signal (SIGINT, sigcatcher); //������ ������� ��������� �������

	status = 1;

	if (fork() == 0) //���������� ���������� � ���������
	{
		while (flag); //�ģ� �������
		//������������ ����� � ������
		i = 0;
		while (n)
		{
			chislo[i] = n % 10 + '0';
			n /= 10;
			i++;
		}
		tmp[i] = '\0';
		i--;
		k = 0;
		while (i >= 0)
		{
			tmp[k] = chislo[i];
			i--;
			k++;
		}
		close (1); //��������� ����������� �����
		dup (chisl[1]); //� ������������� �� ���� �����
		execl ("lab4_22", "lab4_22", tmp, 0); //�������� ��������� ����ޣ�� ����������
		return 0;
	}

	for (i = 0; i < r; i++) //���������� ����������� � �����������
	{
		if (fork() == 0)
		{
			while (flag); //�ģ� �������
			//������������ ����� � ������
			j = 0;
			while (mas[i])
			{
				chislo[j] = mas[i] % 10 + '0';
				mas[i] /= 10;
				j++;
			}
			tmp[j] = '\0';
			j--;
			k = 0;
			while (j >= 0)
			{
				tmp[k] = chislo[j];
				j--;
				k++;
			}
			close (1); //��������� ����������� �����
			dup (znam[1]); //� ������������� �� ���� �����
			execl ("lab4_22", "lab4_22", tmp, 0); //�������� ��������� ����ޣ�� ����������
			return 0;
		}
	}

	kill (-getpgrp(), SIGINT); //�������� ������ �������� (� ����) ��� ������ ������

	for (i = 0; i <= r; i++) //�������� ���������� ���� �����ģ���� ��������
	{
		wait (&status);
		if (status != 0)
		{
			printf ("��������� �����-�� ������\n");
			return 1;
		}
	}

	//������ ���������� ������ � ���������� ����� ����������
	ft = 1;
	read (chisl[0], &n, sizeof(int)); //��������� ��������� � ��������� - n!
	ft *= n;

	for (i = 0; i < r; i++) //��������� ���������� �����������
	{
		read (znam[0], &n, sizeof(int));
		ft /= n;
	}

	printf ("����� ����������: %d\n", ft);

	return 0;
}
