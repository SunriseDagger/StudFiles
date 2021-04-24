#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>

int flag = 1;

void sigcatcher (int) //Отлов сигнала от родителя для начала действий
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

	pipe (znam);	//Канал для факториалов в знаменателе дроби
	pipe (chisl);	//Канал для факториала в числителе дроби

//Ввод исходных данных
	printf ("Число размещений n элементов в r ячейках\n");
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

	signal (SIGINT, sigcatcher); //Задаём функцию обработки сигнала

	status = 1;

	if (fork() == 0) //Вычисление факториала в числителе
	{
		while (flag); //Ждём сигнала
		//Раскладываем число в строку
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
		close (1); //Закрываем стандартный вывод
		dup (chisl[1]); //И переназначаем на него канал
		execl ("lab4_22", "lab4_22", tmp, 0); //Вызываем программу подсчёта факториала
		return 0;
	}

	for (i = 0; i < r; i++) //Вычисление факториалов в знаменателе
	{
		if (fork() == 0)
		{
			while (flag); //Ждём сигнала
			//Раскладываем число в строку
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
			close (1); //Закрываем стандартный вывод
			dup (znam[1]); //И переназначаем на него канал
			execl ("lab4_22", "lab4_22", tmp, 0); //Вызываем программу подсчёта факториала
			return 0;
		}
	}

	kill (-getpgrp(), SIGINT); //Посылаем сигнал потомкам (и себе) для начала работы

	for (i = 0; i <= r; i++) //Ожидание завершения всех порождённых потомков
	{
		wait (&status);
		if (status != 0)
		{
			printf ("Произошла какая-то ошибка\n");
			return 1;
		}
	}

	//Чтение переданных данных и вычисление числа размещений
	ft = 1;
	read (chisl[0], &n, sizeof(int)); //Считываем факториал в числителе - n!
	ft *= n;

	for (i = 0; i < r; i++) //Считываем факториалы знаменателя
	{
		read (znam[0], &n, sizeof(int));
		ft /= n;
	}

	printf ("Число размещений: %d\n", ft);

	return 0;
}
