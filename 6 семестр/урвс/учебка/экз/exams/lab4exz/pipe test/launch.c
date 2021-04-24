#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>

void sig_b(){
    printf("Signal!\n");
}

int main(){
    int op_f = open("data.txt", O_WRONLY);
    double a = 10.0;
    int iter;
    for(iter = 0; iter < 100; iter++)
        write(op_f, &a, sizeof(a));
    close(op_f);
    return 0;
}
