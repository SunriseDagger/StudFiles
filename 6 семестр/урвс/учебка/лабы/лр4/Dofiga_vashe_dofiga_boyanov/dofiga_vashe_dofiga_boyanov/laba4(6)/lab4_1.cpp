#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>

#define UP 0
#define DOWN 1

//��������� �������
float function (float x, int m)
{
	float f = 0.0, t = 1.0;
	for (k=0; k<=m; k++)
	{
		f=f+pow (x, 2*k+1)*t;
		t=t*x*x*(2*k+1)/(3*k+1);
	}
	return f;
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
	const pi=3.14;
	int i, n, m, r, f, ft;
	float dx, x;
	int *mas;

	FILE* in;
	in = fopen ("temp", "wt"); //������� ���� �� ���������� ����������
	fclose (in);
//������ �������� ������
	printf ("����� ���������:");
	scanf ("%d", &n)
	printf ("����� ����� �� x:");
	scanf ("%d", &m);
//�������� �� ������������ �������� ������
	if (n < 0)
	{
		printf ("����� ��������� �� ����� ���� ������ 0");
		return 1;
	}
	if (m < 1)
	{
		printf ("����� ����� �� ����� ���� ������ 1");
		return 1;
	}
	dx = pi/n;

	for (i = 0; i < n; i++)
	{
		x = dx * i;
		f = function (x, m);
		in = fopen ("temp", "w");
		fprintf (in, "%f", f);
		fclose (in);
		if (fork () == 0)
		{
			in = fopen ("temp", "r");
			fscanf (in, "%f", &ft);
			fclose (in);
			printf("%f", ft);
		}

	}

	for (i = 0; i <= r; i++) //�������� ���������� ���� ���������� ��������
	{
		wait (&status);
		if (status != 0)
		{
			printf ("��������� �����-�� ������\n");
			return 1;
		}
	}

	in = fopen ("temp", "rt");
	fclose (in);
	return 0;
}
