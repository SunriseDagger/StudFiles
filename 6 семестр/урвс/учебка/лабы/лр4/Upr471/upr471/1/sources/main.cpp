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
/**
  * A structure that contains an id of a proccess
  * and the result of it`s execution
  */
struct record
{
	int procdescr;//!<An id
	long factorial;//!<The result of factorial calculation
} rec1;
/** 
 * @brief The procedure of calculation of a factorial
 * @param A file
 * @param The argument of factorial-function
 */
void process (int tempfile, long temp)
{
    long i, factorial; 
    factorial = 1;
    for(i = 2; i <= temp; i++)//!<Calculation of factorial
    factorial *= i;
    //!<Now writing the result in a temporary file
    rec1.factorial = factorial;
    rec1.procdescr = getpid ();
    write(tempfile, &rec1, sizeof rec1);
}
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
int main (int argc, char *argv[])
{
    int pdn, pdk, pdn_k, tempfile;
    long n, k, temp, fact_n, fact_k, fact_n_k;
    struct stat statbuf;
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
    temp = n;
    //<! Open a temporary file
    tempfile = open("temp", O_CREAT|O_RDWR, 0x0777);
    //<! A child-1 creation
    pdn = fork();
    //<! If a child
    if(pdn == 0)
    {
        process(tempfile, temp);
        exit (0);
    }
    //<! A child-2 creation
    temp = k;
    pdk = fork();
    //<! If a child 
    if(pdk == 0)
    {
        do
        {
            fstat(tempfile, &statbuf);
        }
        while(statbuf.st_size != sizeof (struct record));
        process(tempfile, temp);
        exit (0);
    }
    //<! A child-3 creation
    temp = n - k;
    pdn_k = fork ();
    //<! If a child  
    if(pdn_k == 0)
    {
        do
        {
            fstat(tempfile, &statbuf);
        }
        while(statbuf.st_size != sizeof (struct record) * 2);
        process(tempfile, temp);
        exit (0);
    }
    //<! When children had been written all the results in the file
    do
    {
        fstat(tempfile, &statbuf);
    }
    while(statbuf.st_size != sizeof (struct record) * 3);
    //<! We`d go again all the file
    lseek(tempfile, 0, SEEK_SET);
    //<! Then we read this file
    for(int ij = 1; ij <= 3;ij++)
    {
        while(!read(tempfile, &rec1, sizeof rec1));
        if(rec1.procdescr == pdn)  	 fact_n = rec1.factorial;
        if(rec1.procdescr == pdk)  	 fact_k = rec1.factorial;
        if(rec1.procdescr == pdn_k)	 fact_n_k = rec1.factorial;
    }
    //<! The result output
    close(tempfile);
    printf("%ld\n", fact_n / (fact_k * fact_n_k));
    remove("temp");
    sleep(1);
    return 0;
}

