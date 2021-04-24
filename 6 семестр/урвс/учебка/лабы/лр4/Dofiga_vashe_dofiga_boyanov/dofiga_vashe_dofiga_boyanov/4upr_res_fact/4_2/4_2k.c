#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
int main() {
    int i, r, nEl, fd[2], *n, *pids;
    char rfd[10], wfd[10];
    long rez, N=1, factN;
    FILE *fd1;
    if ((fd1=fopen("data.in", "r"))==NULL) {printf("Файл data.in не найден\n"); exit(1);}
    fscanf(fd1, "Число ячеек: %d\n", &r);
    fscanf(fd1, "Число элементов: %d ", &nEl);
    fscanf(fd1, "Расположение элементов: ");
    n=(int*)malloc(nEl*sizeof(int));//выделяет память для данных
    pids=(int*)malloc(nEl*sizeof(int));//выделяет память для идентификаторов
    for (i=0; i<r; i++) fscanf(fd1, "%d", &n[i]);
    fclose(fd1);
    pipe(fd);
    sprintf(rfd, "%d", fd[0]);//сохраняет идентификатор потока для чтения
    sprintf(wfd, "%d", fd[1]);//сохраняет идентификатор потока для записи
    for (i=0; i<r; i++) {
	if ((pids[i]=fork())==0) {//порождает потомков
	    execlp("./fact", "fact", rfd, wfd, 0);//запускает fact
	    exit(0);
	}
    }
    for (i=0; i<r; i++) {
	write(fd[1], &n[i], sizeof(int));//передает данные в fact
	kill(pids[i], SIGINT);//размораживает fact
	wait(0);//ждет завершения fact
	read(fd[0], &rez, sizeof(long));//считывает данные из fact
	N*=rez;
    }
    if ((pids[0]=fork())==0) {//порождает потомка для вычисления числителя
	execlp("./fact", "fact", rfd, wfd, 0);
	exit(0);
    }
    write(fd[1], &nEl, sizeof(int));
    kill(pids[0], SIGINT);
    wait(0);
    read(fd[0], &factN, sizeof(long));
    close(fd[0]);
    close(fd[1]);
    N=factN/N;
    printf("Число размещений %d элементов по %d ячейкам = %ld\n", nEl, r, N);
    return 0;
}
