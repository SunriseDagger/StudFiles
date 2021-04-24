/**
 * Program for compute value of f (x) as sum of series from k = 0 to k = N
 * on expression x ^ (2k +1) / (2k +1)! for values ​​of x, uniformly distributed
 * on interval [0; Pi].
 * It is a parent program.
 *
 * @author      Alexandrov Mikhail
 * @author      Zhigalov Peter
 * @version     0.3
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <math.h>

/** Signal about begin of work */
#define SIGNAL_BEGIN SIGUSR1
/** Signal about end of work */
#define SIGNAL_END SIGUSR2

/**
 * Function for exit with code 2 if child unexpectedly changed status
 */
void child_dead(int)
{
    fprintf(stderr, "Child unexpectedly changed status.\n");
    exit(2);
}

/**
 * Function for calculate sum of series
 *
 * @param x     argument of function
 * @param N     number elements on series
 * @return sum  of series
 */
double f(double x, int N)
{
    double sum = x;
    int tmp;
    for (int i = 1; i < N; i++)
    {
        tmp = i * 2;
        sum += sum * x * x / tmp / (tmp + 1);
    }
    return sum;
}

/**
 * Main function
 *
 * @param argc  number of program arguments
 * @param argv  program arguments
 * @return      1 - incorrect parameters, 2 - child unexpectedly changed status, 0 - all correct
 */
int main(int argc, char* argv[])
{
    /** Check arguments number */
    if (argc != 2)
    {
        fprintf(stderr, "1 arguments expected.\n");
        exit(1);
    }
    /** Config file loading */
    FILE *file = fopen("./config.txt", "r");
    if (!file)
    {
        fprintf(stderr, "Can't open configuration file.\n");
        exit(1);
    }
    int N = -1;
    if (!fscanf(file, "%i", &N) || N == -1)
    {
        fprintf(stderr, "Can't read configuration file.\n");
        fclose(file);
        exit(1);
    }
    fclose(file);
    /** Arguments loading */
    int num_steps = 1;
    if (!sscanf(argv[1], "%i", &num_steps))
    {
        fprintf(stderr, "Can't read arguments.\n");
        exit(1);
    }
    if (num_steps <= 0)
    {
        fprintf(stderr, "Bad arguments.\n");
        exit(1);
    }
    double h = M_PI / num_steps;

    pid_t child_pid;
    double data[2];

    /** Create pipe */
    int fifo[2];
    if (pipe(fifo) == -1)
    {
        fprintf(stderr, "Pipe create failed.\n");
        exit(1);
    }

    switch(child_pid = fork())
    {
        /** Can`t create fork */
    case (pid_t)-1:
        fprintf(stderr, "Error creatinfg fork.\n");
        break;

        /** Code for forked process */
    case (pid_t)0:
    {
        /** Replace stdin with pipe */
        dup2(fifo[0], 0);
        /** Execute child process */
        execv("./summator_children", NULL);
        fprintf(stderr, "Child exec failed\n");
        exit(1);
    }
    }

    /** If child changed status run child_dead function */
    signal(SIGCHLD, child_dead);

    usleep(10000);

    /** Calc */
    for(int i = 0; i <= num_steps; i++)
    {
        double x = h * i;
        data[0] = x;
        data[1] = f(x, N);
        write(fifo[1], data, 2 * sizeof(double));
    }

    /** Send begin signal */
    kill(child_pid, SIGNAL_BEGIN);

    /** Some wait */
    usleep(10000);
    /** Ignoring if child changed status */
    signal(SIGCHLD, SIG_IGN);

    /** Send end signal */
    kill(child_pid, SIGNAL_END);

    /** Some wait */
    int t;
    wait(&t);

    /** Close pipe */
    close(fifo[1]);
    close(fifo[0]);
    return t;
}
