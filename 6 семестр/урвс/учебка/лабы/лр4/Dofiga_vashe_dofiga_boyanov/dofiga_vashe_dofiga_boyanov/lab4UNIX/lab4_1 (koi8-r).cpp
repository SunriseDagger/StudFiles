#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>

#define UP 0
#define DOWN 1

//Вычисляем факториал
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

//Вывод структурированной записи во временный файл
//status - UP (числитель), DOWN (знаменатель)
void wr (int status, int ft)
{
	FILE* out;
	out = fopen ("temp", "at+");
	if (!out)
	{
		printf ("Ошибка открытия файла для записи\n");
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
	in = fopen ("temp", "wt"); //Очищаем файл от устаревшей информации
	fclose (in);

	printf ("Число размещений n элементов в r ячейках\n");
//Вводим исходные данные
	printf ("Введите n (1 - 12) ");
	scanf ("%d", &n);
	if (n > 12)
	{
		printf ("Факториал числа n выходит за границы int. Выберите число n от 1 до 12\n");
		return 1;
	}
	if (n < 0)
	{
		printf ("Число n не может быть отрицательным\n");
		return 1;
	}
	printf ("Введите r ");
	scanf ("%d", &r);

	mas = new int[r];

	status = 0;
	for (i = 0; i < r; i++)
	{
		printf ("Введите число элементов в ячейке %d ", i);
		scanf ("%d", &mas[i]);
		if (mas[i] < 0)
		{
			printf ("Числа n(i) не могут быть отрицательными\n");
			return 1;
		}
		status += mas[i];
	}

//Проверка на правильность исходных данных
	if (status != n)
	{
		printf ("Сумма n(i) не равна n\n");
		return 1;
	}

	if (fork() == 0) //Вычисление факториала в числителе
	{
		ft = fact (n);
		wr (UP, ft);
		return 0;
	}

	for (i = 0; i < r; i++) //Вычисление факториалов в знаменателе
	{
		if (fork() == 0)
		{
			ft = fact (mas[i]);
			wr (DOWN, ft);
			return 0;
		}
	}

	for (i = 0; i <= r; i++) //Ожидание завершения всех порождённых потомков
	{
		wait (&status);
		if (status != 0)
		{
			printf ("Произошла какая-то ошибка\n");
			return 1;
		}
	}

	in = fopen ("temp", "rt");

	if (!in)
	{
		printf ("Ошибка при чтении файла\n");
		return 1;
	}

	ft = 1;
	for (i = 0; i <= r; i++) //Чтение переданных данных и вычисление числа размещений
	{
		fscanf (in, "%d %d", &status, &n);
		if (status == UP)
			ft *= n;
		else
			ft /= n;
	}

	fclose (in);

	printf ("Число размещений: %d\n", ft);

	return 0;
}
