#include <stdlib.h>
#include <stdio.h>
long fact(int n) {//вычисляет факториал n!
    int i;
    long rez;
    for (i=1, rez=1; i<=n; i++) rez*=i;
    return rez;
}
int main() {
    int i, null, r, nEl, *n;
    long rez, N=1;
    FILE *fd1, *fd2;
    if ((fd1=fopen("data.in", "r"))==NULL) {printf("Файл data.in не найден\n"); exit(1);}
    fscanf(fd1, "Число ячеек: %d\n", &r);
    fscanf(fd1, "Число элементов: %d ", &nEl);
    fscanf(fd1, "Расположение элементов: ");
    n=(int*)malloc(nEl*sizeof(int));
    for (i=0; i<r; i++) fscanf(fd1, "%d", &n[i]);
    fclose(fd1);
    fd1=fopen("tmp", "w");
    fd2=fopen("tmp", "r");
    for (i=0; i<r; i++) {
	if (fork()==0) {//процесс-потомок
	    fprintf(fd1, "%d %ld\n", i, fact(n[i]));//запись результата в файл
	    exit(i);
	}
    }
    for (i=0; i<r; i++) {//процесс-родитель
	wait();
	fscanf(fd2, "%d %ld\n", &null, &rez);//чтение множителя из файл
        N*=rez;
    }
    fclose(fd1);
    fclose(fd2);
    N=fact(nEl)/N;
    printf("Число размещений %d элементов по %d ячейкам = %ld\n", nEl, r, N);
    return 0;
}
