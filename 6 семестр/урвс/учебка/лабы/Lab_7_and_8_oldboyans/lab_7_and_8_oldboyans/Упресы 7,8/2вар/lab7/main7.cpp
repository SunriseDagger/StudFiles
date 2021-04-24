/**
 * Межпроцессное взаимодействие программ
 * @author	Крайнюк Михаил
 * @author	Сидоров Сергей
 * @version	1.0
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <wait.h>
#define N 1536
#define K 13
// Данных от потомков
struct B{
	bool free;			/// Считанный текст одного из процессов
	char info[N];		/// Записаный текст
};
// Управляющие параметры
union semun{
	int val;				/// Значение семафора
	struct semid_ds *mbuf;	/// Управляющие параметры набора
	ushort *array;			/// Масив значений симафоров
};
// Работа потомков
void proc(int i, char name[], int semafor, int memory){
	// Буферизация стихов		
		int fd = open(name, O_RDONLY);       
		char Buf[N];			/// Буфер для всех четверостиший
		int len;				/// Длина буфера
		len = (int)read(fd, Buf, sizeof(Buf));
		close(fd);
		// Вывод четверостиший
		int stroka = 0;			/// Количество пройденных строк
		int n = 0;			/// Счетчик выведенных символов
		struct sembuf pod[2];		/// Подключение семафоров
		while(n < len){
			// Блокируем семафор
			pod[0].sem_num = 0;		/// индекс семафора
			pod[0].sem_op = -1;		/// Уменьшаем значение семафора на 1
			pod[0].sem_flg = 0;		/// Флаг операции
			pod[1].sem_num = 1;		/// индекс семафора
			pod[1].sem_op = 0;		/// Ожидание нулевого значения
			pod[1].sem_flg = 0;		/// Флаг операции
			if (semop(semafor, pod, 2) == -1){		/// Блокируем семафор
				printf("Процесс %i: Семафоры 1 и 2 не заблокированы\n", i);
				exit(-1);
			}
			printf("Процесс %i: Семафоры 1 и 2 заблокированы.\n", i);
			char*vmer = (char*)shmat(memory, 0, 0);	/// переменная для работы с памятью
			if (vmer == (char*)-1){
				printf("Процесс %i: Память не подключена\n", i);
				exit(-1);
			}
			printf("Процесс %i: Память подключена.\n", i);
			if(i == 1)  vmer[0] = '1';
			else vmer[0] = '2';
			// Чтение 4 строк
			for(; n < len && stroka < 4; n++){
				if(Buf[n] == '\n') stroka++;
				vmer[n+1] =  Buf[n];			///Считываем четверостишья
			}
			// Подготовка к чтению следующего четверостишья
			stroka = 0;
			n++;
			write(1, vmer, (size_t)N);
			if (shmdt(vmer) == -1){			/// Отключаем память
				printf("Процесс %i: Память не отключилась.\n", i);
				exit(-1);
			}
			printf("Процесс %i: Память отключена\n", i);
			// Открытие первого семафора
			pod[0].sem_num = 0;		/// индекс семафора 1
			pod[0].sem_op = 1;		/// Увеличиваем значение семафора 1 на 1
			pod[0].sem_flg = 0;		/// Флаг операции
			//Подключение второго семафора
			pod[1].sem_num = 1;		/// индекс семафора
			pod[1].sem_op = 1;		/// Увеличиваем значение семафора на 1
			pod[1].sem_flg = 0;		/// Флаг операции
			if (semop(semafor, pod, 2) == -1){			/// Изменение семафора 2
				printf("Процесс %i: Семафоры 1 и 2 не разблокированы.\n", i);
				exit(-1);
			}
			printf("Процесс %i: Семафоры 1 и 2 разблокированы.\n", i);
		}		
}

int main(int argc, char *argv[]){
	char name_nechet[] = "nechet";		/// Файл с нечетными четверостишьями
	char name_chet[]   = "chet";		/// Файл с четными четверостишьями
	int procID1;						/// Первый потомок (нечетный)
	int procID2;						/// Второй потомок (четный)
	// Создание семафора
	int semafor = semget(IPC_PRIVATE, 2, IPC_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if(semafor < 0){					/// Создаем набор семафоров
		fprintf(stderr, "Ошибка создания семафоров!\n");
		exit(-1);
	}
	// Выделение памяти
	int memory = shmget(IPC_PRIVATE, (size_t)512, IPC_CREAT|0644);		/// Создаем разделяемую память
	if(memory < 0){
		fprintf(stderr, "Ошибка создания разделяемого сегмента!\n");
		exit(-1);
	}
	union semun arg;		/// Установка нулевого семафора
	arg.val = 1;			/// Значение в нулевом семафоре
	if(semctl(semafor,0, SETVAL, arg) == -1){			/// Устанавливаем 0 семафор
		fprintf(stderr,"Ошибка установки семафора 1\n");
		semctl(semafor,0,IPC_RMID,0);		/// Удаляем 0 семафор
		shmctl(memory,IPC_RMID,0);			/// Убираем память
		exit(-1);
	}
	union semun arg1;		/// Установка первого семафора
	arg1.val = 0;			/// Значение в первом семафоре
	if(semctl(semafor,1, SETVAL, arg1) == -1){			/// Устанавливаем 1 семафор
		fprintf(stderr,"Ошибка установки семафора 2");
		semctl(semafor,1,IPC_RMID,0);					/// Удаляем 1 семафор
		shmctl(memory,IPC_RMID,0);						/// Убираем память
		exit(-1);
	}
	// Старт потомков
	// 1 потомок
	procID1 = fork();			/// Создаем процесс 1
	if(procID1 < 0){
		fprintf(stderr, "Невозможно создать дочерний процесс 1!\n");
		exit(-1);
	}
	if(procID1 == 0){
		printf("Нечетный потомок запущен!\n");
		proc(1, name_nechet, semafor, memory);
		printf("Нечетный потомок завершен!\n");
		exit(0);
	}
	// 2 потомок
	procID2 = fork();			/// Создаем процесс 2
	if(procID2 < 0){
		fprintf(stderr, "Невозможно создать дочерний процесс 2!\n");
		exit(-1);
	}
	if(procID2 == 0){
		printf("Четный потомок запущен!\n");
		proc(2, name_chet, semafor, memory);
		printf("Четный потомок завершен!\n");
		exit(0);
	}
	// Работа родителя
	B mas[K];					/// Куда пишем от потомков
	int chN = 0, nechN = 1;		/// Счетчики заполненности	
	struct sembuf pod[2];		/// Подключение семафоров
	while(chN < N-1 || nechN < N){
		// Блокируем семафор
		pod[0].sem_num = 0;		/// индекс семафора
		pod[0].sem_op = -1;		/// Уменьшаем значение семафора на 1
		pod[0].sem_flg = 0;		/// Флаг операции
		pod[1].sem_num = 1;		/// индекс семафора
		pod[1].sem_op = -1;		/// Уменьшаем значение семафора на 1
		pod[1].sem_flg = 0;		/// Флаг операции
		if (semop(semafor, pod, 2) == -1){
			printf("Родитель: Семафоры 1 и 2 не заблокированы\n");
			exit(-1);
		}
		printf("Родитель: Семафоры 1 и 2 заблокированы.\n");
		char*vmer = (char*)shmat(memory, 0, 0);	/// тот кто работает с памятью			
		if (vmer == (char*)-1){
			printf("Родитель: Память не подключена\n");
			exit(-1);
		}
		printf("Родитель: Память подключена.\n");
		if(vmer[0] == '1'){
			// Чтение 4 строк
			for(int nR = 0; nR < N && vmer[nR+1] != '\0'; nR++){
				mas[nechN].info[nR+1] = vmer[nR];			///Считываем четверостишья
			}
			nechN += 2;
		}
		else{
			// Чтение 4 строк
			for(int nR = 0; nR < N && vmer[nR+1] != '\0'; nR++){
				mas[chN].info[nR+1] = vmer[nR];			///Считываем четверостишья
			}
			chN += 2;
		}
		write(1, vmer, (size_t)N);
		if (shmdt(vmer) == -1){
			printf("Родитель: Память не отключилась.\n");
			exit(-1);
		}
		printf("Родитель: Память отключена\n");
		// Разблокируем семафор
		pod[0].sem_num = 0;		/// индекс семафора
		pod[0].sem_op =  1;		/// Увеличиваем значение семафора 1 на 1
		pod[0].sem_flg = 0;		/// Флаг операции
		if (semop(semafor, pod, 1) == -1){
			printf("Родитель: Семафоры 1 и 2 не разблокированы\n");
			exit(-1);
		}
		printf("Родитель: Семафоры 1 и 2 разблокированы.\n");
	}	
	// Ожидание потомков
	int retproc;				/// Закончившийся процес
	int retval;					/// Значение, возвращенное потомком
	int iP;						/// Счетчик завершенных потомков
	int all = 0;				/// Итоговое возвращаемое значение	
	for(iP = 0; iP < 2; iP++){
		retproc = wait(&retval);
		if(retproc == procID1){
			printf("Потомок 1 нечетный завершен = %i\n", retval);
			all += retval;
		}
		else if(retproc == procID2){
			printf("Потомок 2 четный завершен = %i\n", retval);
			all += retval;
		}		
		else printf("o_O\n");
	}
	// Вывод стихотворения
	int fd = open("rez",O_WRONLY);
	for(int fwn = 0; fwn < K; fwn++){
		write(fd, mas[fwn].info, sizeof(mas[fwn].info));
	}
	// Возвращаем сумму результатов потомков
	return all;
}
