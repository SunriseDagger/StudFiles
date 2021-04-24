/**
 * Resource management, Lab 05, Variant 02.
 * Developing synchronization mechanism of processes
 * and their interactions through program channels.
 * @author      Alexandrov Mikhail
 * @author      Zhigalov Peter
 * @version     0.3
 */
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Function for ignoring received signal
 */
void ign(int) { }

/**
 * @brief Main function
 * @return 1 if error, 0 - all correct
 */
int main()
{
    int fifo[2];
    long data1;
    double data2;
    int status1, status2;
    /** Create pipe */
    if (pipe(fifo) == -1)
    {
        fprintf(stderr, "Can`t create pipe!\n");
        exit(1);
    }

    signal(SIGUSR1, ign);
    /** Create Child #P1 */
    pid_t child1 = fork();
    if (child1 == -1)
    {
        fprintf(stderr, "Can't fork child #P1!\n");
        close(fifo[0]);
        close(fifo[1]);
        exit(1);
    }
    else if (!child1)
    {
        signal(SIGUSR1, ign);
        pid_t child_p1 = getpid();
        printf("Child #P1: %i created.\n", child_p1);
        kill(getppid(), SIGUSR1);
        sigpause(SIGUSR1);
        printf("Child #P1: signal accepted.\n");
        /** Counter for 40 bit output */
        int n = 40 / sizeof(long);
        for (int i = 0; i < n; i++)
        {
            write(fifo[1], &child_p1, sizeof(int));
            write(fifo[1], &(data1 = (long)(i + 1)), sizeof(long));
        }
        printf("Child #P1: work done.\n");
        exit(0);
    }
    /** Wait launching fork */
    sigpause(SIGUSR1);

    /** Create Child #P2 */
    pid_t child2 = fork();
    if (child2 == -1)
    {
        fprintf(stderr, "Can`t fork child #P2!\n");
        close(fifo[0]);
        close(fifo[1]);
        kill(child1, SIGKILL);
        exit(1);
    }
    if (!child2)
    {
        pid_t child_p2 = getpid();
        printf("Child #P2: %i created.\n", child_p2);
        /** Counter for 40 bit output */
        int n = 40 / sizeof(double);
        for (int i = 0; i < n; i++)
        {
            write(fifo[1], &child_p2, sizeof(int));
            write(fifo[1], &(data2 = (double)(i + 1)), sizeof(double));
        }
        kill(child1, SIGUSR1);
        printf("Child #P2: signal sended.\n");
        printf("Child #P2: work done.\n");
        exit(0);
    }

    close(fifo[1]);
    /** Print results */
    pid_t pid;
    while (read(fifo[0], &pid, sizeof(int)))
    {
        if (pid == child1)
        {
            read(fifo[0], &data1, sizeof(long));
            printf("%i: %li\n", pid, data1);
        }
        else if (pid == child2)
        {
            read(fifo[0], &data2, sizeof(double));
            printf("%i: %lf\n", pid, data2);
        }
        else
        {
            fprintf(stderr, "Unknown PID value in pipe!\n");
            kill(child1, SIGKILL);
            kill(child2, SIGKILL);
            exit(1);
        }
    }

    /** Wait stopping child */
    waitpid(child1, &status1, WUNTRACED);
    waitpid(child2, &status2, WUNTRACED);
    close(fifo[0]);
    /** Some errors messages */
    if(status1 && WIFSIGNALED(status1))
    {
        fprintf(stderr, "Child #P1 received unexpected signal %i\n", WTERMSIG(status1));
        exit(1);
    }
    if(status2 && WIFSIGNALED(status2))
    {
        fprintf(stderr, "Child #P2 received unexpected signal %i\n", WTERMSIG(status2));
        exit(1);
    }
    if(status1 && WIFSTOPPED(status1) && WSTOPSIG(status1))
    {
        fprintf(stderr, "Child #P1 stopped with signal %i\n", WSTOPSIG(status1));
        exit(1);
    }
    if(status2 && WIFSTOPPED(status2) && WSTOPSIG(status2))
    {
        fprintf(stderr, "Child #P2 stopped with signal %i\n", WSTOPSIG(status2));
        exit(1);
    }
    return 0;
}
