#include <signal.h>
#include <unistd.h>
#include <stdio.h>
void dummy(int i) {}
int main(int argc, char * argv[]) {
    int n, i, fd[2];
    long rez;
    sscanf(argv[1], "%d", &fd[0]);//получает поток для чтения
    sscanf(argv[2], "%d", &fd[1]);//получает поток для записи
    signal(SIGINT, dummy);
    pause();
    read(fd[0], &n, sizeof(int));
    for (i=1, rez=1; i<=n; i++) rez*=i;//вычисляет факториал
    write(fd[1], &rez, sizeof(long));    
    return 0;
}
