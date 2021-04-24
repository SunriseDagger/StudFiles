#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>

#define UP 0
#define DOWN 1

//��������� ���������
int fact (int k)
{
	int fact = 1;
	while (k)
	{
		fact *= k;
		k--;
	}
	return fact;
}

//����� ����������������� ������ �� ��������� ����
//status - UP (���������), DOWN (�����������)
void wr (int status, int ft)
{
	FILE* out;
	out = fopen ("temp", "at+");
	if (!out)
	{
		printf ("������ �������� ����� ��� ������\n");
		exit (1);
	}
	fprintf (out, "%d %d\n", status, ft);
	fclose (out);
}

int main()
{
	int i, n, r, ft, status;
	int *mas;

	FILE* in;
	in = fopen ("temp", "wt"); //������� ���� �� ���������� ����������
	fclose (in);

	printf ("����� ���������� n ��������� � r �������\n");
//������ �������� ������
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

	if (fork() == 0) //���������� ���������� � ���������
	{
		ft = fact (n);
		wr (UP, ft);
		return 0;
	}

	for (i = 0; i < r; i++) //���������� ����������� � �����������
	{
		if (fork() == 0)
		{
			ft = fact (mas[i]);
			wr (DOWN, ft);
			return 0;
		}
	}

	for (i = 0; i <= r; i++) //�������� ���������� ���� �����ģ���� ��������
	{
		wait (&status);
		if (status != 0)
		{
			printf ("��������� �����-�� ������\n");
			return 1;
		}
	}

	in = fopen ("temp", "rt");

	if (!in)
	{
		printf ("������ ��� ������ �����\n");
		return 1;
	}

	ft = 1;
	for (i = 0; i <= r; i++) //������ ���������� ������ � ���������� ����� ����������
	{
		fscanf (in, "%d %d", &status, &n);
		if (status == UP)
			ft *= n;
		else
			ft /= n;
	}

	fclose (in);

	printf ("����� ����������: %d\n", ft);

	return 0;
}
