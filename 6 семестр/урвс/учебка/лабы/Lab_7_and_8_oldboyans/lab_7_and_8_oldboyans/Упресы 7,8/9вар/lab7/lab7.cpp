/**	@file	 upr7_9.cpp
*	@brief Единственный файл приложения
 *	@note Лабораторная работа N7 (вариант 9)
 *	@author	Чернов А. К. (shtyle@rambler.ru)
 *	@date	2010
 *	@par
 *	Головной процесс несколько раз порождает потомков, каждый из которых
 *	пытается запустить один из 2 исполлняемых файлов (./lab1, ./lab2).
 *	Если программа уже запущениа, потомок дожидается своей очереди.
 *	Имена программ передаются потомкам через область разделяемой памяти.
 * 	Информация о номере изменяемого семафора передается от потомка через
 *	очередь сообщений.
 */
///#include <<<Some headers...>>>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>	///< Семафоры
#include <sys/msg.h>	///< Сообщения
#include <sys/shm.h>	///< Разделяемая память

/**	Макрос  - обработка некорректного завершения (хочу через #define) 
  *	 @param x - возвращаемый код
 *	 @param y - возвращаемое сообщение
 */
void MyError(int x,const char y[]) {
perror(y);
exit(x);
}

/**	Закрываем всё что открыли	(опять через #define)*/
void CloseAll()
{
msgctl(msgfd,IPC_RMID,0);
semctl(shmfd,IPC_RMID,0);
}

/**	Необходимые константы	*/
enum {
	MAGIC_KEY,	0x010,	///< Для совместимости. Хоть какой-то ключ.
	MSG_SIZE,	250,	///< Размер сообшения
	SHM_SIZE,	1024,	///< Размер разделяемой памяти
	N_SIZE,		10,		///< Длина имени вызываемых программы
	F_NUM,		5,		///< Количество потомкков
};

/**	Определяем структуру семафора	*/
union Semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

/**	Определяем структуру операции над семафором	*/
union Sembuf (
	short sem_num;
	short sem_op;
	short sem_flag;
);

/**	Определяем структуру сообщения	*/
union Membuf {
	long mtype;
	char mtext[MSG_SIZE];
};

int main(int argc, char argv[])
{
	int semfd,msgfd,shmfd;	
	semfd = semget(MAGIC_KEY,2, IPC_CREAT);
	if (semfd < 0)
		MyError(1,"semget error");
	msgfd = msgget(MAGIC_KEY,IPC_CREAT);
	if (msgfd < 0)
		MyError(2,"msgget error");
	shmfd = shmget(MAGIC_KEY,SHM_SIZE,IPC_CREAT)
	if (shmfd < 0) {
		msgctl(msgfd, IPC_RMID,0);
		MyError(3,"shmget error");}
	char *names;
	if ((names = (char *)shmat(shmfd,0,0)) < 0) {
		CloseAll();
		MyError(3,"shmat error");
		}
	struct Semun TmpSem;
	TmpOp[0].val = 1;
	semctl(semfd,0,SETVAL,TmpOp);
	semctl(semfd,1,SETVAL,TmpOp);
	sprintf((char*)names,"ls\0");						///< Пока что так...
	sprintf((char*)(names+N_SIZE),"cat lab7.cpp\0");	///< 
	int p_id;
	randomize(&TmpOp);
	for (int i = 0; i < F_NUM; i++)
	{
		switch(p_id=fork())
		{
			case -1:
				CloseAll();
				MyError(4,"fork error");
				break;
			case 0:
				struct Sembuf tmp;
				struct Msgbuf MyMsg;
				int k = rand()&1;
				tmp.sem_un = k;
				tmp.sem_op = -1;		///< Пытаемся уменьшить
				tmp.sem_flg = 0;
				semop(semfd,tmp,sizeof(tmp));
				sprintf(MyMsg.mtext,"%d\0",k);	///< Сообщаем родителю какую программу заняли
				msgsnd(msgfd,&MyMsg,sizeof(MyMsg),0);	
				char *name = (shmat(shmfd,0,0)+k*N_SIZE);				
				execl(name,name,0,0);
				exit(0);
		}
	}
	int status;
	struct Sembuf tmp;
	struct Msgbuf MyMsg;
	int k;
	tmp.sem_op = 1;
	tmp.sem_flg = 0;
	for (int i = 0; i < F_NUM; i++)
	{
		wait(&status);	///< Ждем потомка
		msgrcv(msgfd,&MyMsg,sizeof(MyMsg),0,0);	///< Узнаем что он выполнял
		sscanf(MyMsg.mtext,"%d",&k);
		tmp.sem_un = k;
		semop(semfd,tmp,sizeof(tmp));	/// < Овобождаем семафор для следующих
	}
	CloseAll();
return 0;
}