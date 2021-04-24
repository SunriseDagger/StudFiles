/**
 * Program for compute value of f (x) as sum of series from k = 0 to k = N
 * on expression x ^ (2k +1) / (2k +1)! for values ​​of x, uniformly distributed
 * on interval [0; Pi].
 * It is a child program.
 *
 * @author      Alexandrov Mikhail
 * @author      Zhigalov Peter
 * @version     0.3
 */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/** Signal about begin of work */
#define SIGNAL_BEGIN SIGUSR1
/** Signal about end of work */
#define SIGNAL_END SIGUSR2

/**
 * Function for exit with code 0 if end signal received
 */
void end(int)
{
    exit(0);
}

/**
 * Function for beginning print if begin signal received
 */
void begin(int)
{
    double data[2];

    while(read(0, data, 2 * sizeof(double)) > 0)
        printf("f(%lf) = %lf\n", data[0], data[1]);

    /** If received end signal run end function */
    signal(SIGNAL_END, end);
    /** Wait end signal */
    sigpause(SIGNAL_END);
}

/**
 * Main function
 *
 * @return 1 if error, 0 - all correct
 */
int main()
{
    /** If received begin signal run begin function */
    signal(SIGNAL_BEGIN, begin);
    /** Wait begin signal */
    sigpause(SIGNAL_BEGIN);
    return 1;
}
