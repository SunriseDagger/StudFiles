/**
 * @detailed The programme that able to calculate C(k,n)=n!/(k!*(n-k)!)
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
void receive(int){}
/** 
 * The main function
 * @param Number of arguments
 * @param n, k
 * @return 1 - Invalid number of arguments
 *         2 - Invalid type of arguments
 *         3 - Arguments are out of range [1,12]
 *         4 - Invalid arguments: n < k
 *         0 - If success
 */
int main(int argc, char *argv[])
{
    int pdn, pdk, pdn_k, pdres;
    long n, k, n_k;
    int direct[2], ret[2];
    char str0[10], str1[10];
    signal(SIGNAL_REPORT_SEND, receive);
    //<! Data control
    if (argc != 3)
    {
        printf("Invalid number of arguments, try again!\nUsage: %s n k\n", argv[0]);
        exit (1);
    }
    if (!sscanf(argv[1], "%ld", &n) || !sscanf(argv[2], "%ld", &k))
    {
        printf("Invalid type of arguments!\n");
        exit (2);
    }
    if (n > 12 || n < 1 || k > 12 || k < 1)
    {
        printf("Arguments are out of range [1,12]!\n");
        exit (3);
    }
    if (n < k)
    {
        printf("Invalid arguments: n < k!\n");
        exit (4);
    }
    //!< Pipes creation
    pipe(direct);
    pipe(ret);
    //!< Prepare numbers of descriptors to send it to children
    sprintf(str0,"%d",direct[0]);
    sprintf(str1,"%d",ret[1]);
    //!< Writing in pipe from parent
    write(direct[1],&n,sizeof(long));
    write(direct[1],&k,sizeof(long));
    n_k = n - k;
    write(direct[1],&n_k,sizeof(long));
    //<! A child-1 creation
    pdn = fork();
    if(pdn == 0)
    {
        close(direct[1]);
        close(ret[0]);
        execl("./process",str0,str1,0);
        exit(0);
    }
    //<! A child-2 creation
    pdk = fork();
    if(pdk == 0)
    {
        close(direct[1]);
        close(ret[0]);
        execl("./process",str0,str1,0);
        exit(0);
    }
    //<! A child-3 creation
    pdn_k = fork();
    if(pdn_k == 0)
    {
        close(direct[1]);
        close(ret[0]);
        execl("./process",str0,str1,0);
        exit(0);
    }
    //<! Wait a time for passing to signal();
    sleep(1);
    //<! Induce childs to execute by signals
    kill(pdn, SIGNAL_ORDER_SEND);
    sigpause(SIGNAL_REPORT_SEND);
    read(ret[0],&n,sizeof(long));

    kill(pdk, SIGNAL_ORDER_SEND);
    sigpause(SIGNAL_REPORT_SEND);
    read(ret[0],&k,sizeof(long));

    kill(pdn_k, SIGNAL_ORDER_SEND);
    sigpause(SIGNAL_REPORT_SEND);
    read(ret[0],&n_k,sizeof(long));
    printf ("%ld\n", n / (k * n_k));
    return 0;
}
    
