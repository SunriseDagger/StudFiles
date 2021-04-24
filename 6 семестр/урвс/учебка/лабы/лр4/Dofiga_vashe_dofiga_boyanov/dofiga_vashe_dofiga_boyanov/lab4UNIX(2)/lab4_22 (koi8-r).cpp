#include <string.h>
#include <unistd.h>
#include <stdio.h>

int fact (int k) //Вычисление факториала
{
	int fact = 1;
	while (k)
	{
		fact *= k;
		k--;
	}
	return fact;
}

int main (int argc, char* argv[])
{
	int ch = 0;
	int i = 0;
	while (argv[1][i] != '\0') //Преобразуем строку в число
	{
		ch *= 10;
		ch += argv[1][i] - '0';
		i++;
	}
	ch = fact (ch);
	write (1, &ch, sizeof(int)); //Выводим в канал полученное значение
	return 0;
}
