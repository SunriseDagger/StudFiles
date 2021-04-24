/**
 * @detailed The programme that able to calculate factorial n!
 * @author Kurochkin A.V.
 * @author Popkov I.V.
 * @version 0.1
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wait.h>

#define SIGNAL_REPORT_SEND SIGUSR1
#define SIGNAL_ORDER_SEND SIGUSR2
/** 
 * @brief The procedure of signal-ignore
 */
void send(int){}
/** 
 * The main function
 * @param Number of arguments
 * @param n
 */
int main(int argc, char **argv)
{
    int direct, ret;
    long n;
    long res;
    signal(SIGNAL_ORDER_SEND, send);
    //<! Waiting before be able to execute
    sigpause(SIGNAL_ORDER_SEND);
    direct = atoi(argv[0]);
    ret = atoi(argv[1]);
    //<! Read from pipe
    read(direct,&n,sizeof(long));
    res = 1;
    //<! Calculation
    for(int i = 2; i <= n; i++)
        res = res * i;
    //<! Returns result with a pipe
    write(ret,&res,sizeof(long));
    //<! Signal that the data in the channel
    kill(getppid(),SIGNAL_REPORT_SEND);
    return 0;
}


