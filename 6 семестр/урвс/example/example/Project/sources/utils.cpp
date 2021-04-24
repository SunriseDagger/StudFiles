#include "utils.h"

/**
 * Saving data to text file
 * 
 * @param  filename  output filename
 * @param  msg  message which will be save in file
 * @return      TRUE if data were saved successfully, FALSE if there was error
 */
int saveToFile(const char *filename, char *msg)
{
    FILE *fd; // file descriptor    

    if((fd = fopen(filename, "w")) == NULL)
    {
        return false;
    }
    
    if(fprintf (fd, "%s", msg) < 0)
    {
        return false;
    }
    
    fclose(fd);

    return true;
}

/**
 * Reading parameters from file
 * 
 * @param  filename  parameters filename
 * @param  params  parameters array
 * @param  totalParams  number of parameters
 */
void readDataFromFile(const char *filename, int *params, int totalParams)
{
    FILE *fd; // file descriptor    

    if((fd = fopen(filename, "r")) == NULL)
    {
        char msg[100];

        sprintf(msg, "File \"%s\" cound not be read\n", filename);

        error(msg, ERROR_CODE_FILE);
    }
    
    for(int i = 0; i < totalParams; i++)    
    {
        if(!fscanf(fd, "%d", &params[i]))
        {
            error("Parameters reading error", ERROR_CODE_FILE);
        }
    }    
    fclose(fd);
}

/**
 * Returns factorial of number k. 
 * 
 * If number k is not positive result will be incorrect.
 *
 * @param  k  a positive integer
 * @return      factorial of number k
 */
double factorial(int k)
{
    int i; /* loop control variable */
    double f = 1;  /* set to the factorial of the first positive integer */

    // factorial calculation
    for(i = 2; i <= k; i++)
	    f *= i;

    return f;
}

/**
 * Prints error message and terminates program with error code 
 *
 * @param  msg  error message
 * @param  code  error code
 */
void error(const char *msg, int code)
{
	printf("Error: %s\n", msg);
	exit(code);
}
