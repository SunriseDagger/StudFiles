/**
 * Program for calculation value of sin(x)
 * 
 * @author      Ivanov Ivan
 * @author      Petrov Petr
 * @version     1.0
 */

#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "constants.h"
#include "utils.h"


/**
 * Generate random number in range [0,1]
 * 
 * @return      random number from 0 to 1
 */
inline double generateRandomNumber()
{
    return (double) rand() / RAND_MAX;
}

/**
 * Returns sin(x)
 * 
 * @param  x  function argument
 * @param  n  number of iterations
 * @return      factorial of number k
 */
double sinFunction(double x, int n)
{
    int i; /* loop control variable */    
    double f = 0.0; /* calculation result */
    int temp;

    // sin(x) calculation using formula: sin(x) = x - x^3/3! + x^5/5! - ...
    for(i = 0; i <= n; i++)
    {
        temp = 2*i+1;
        f += pow(-1.0,i)*pow(x, temp) / factorial(temp);
    }	    

    return f;
}


int main(int argc, char* argv[])
{
    int i;        
    //time_t t; // current time
    char msg[100]; // temp variable for system messages
    double f, x; // function value and its argument
    int totalIterations; // total number of iterations for sin(x)
    int params[TOTAL_PARAMETERS]; // input parameters

    // Fixing initial value for random generator so we could use automated tests
    srand(RAND_INITIAL_VALUE);
    //srand((unsigned) time(&t));

    if(DEBUG)
    {
        for(i = 0; i < 10; i++)
            printf("%f\n", generateRandomNumber());
    }

    if(argc > 2)
    	error("Usage: calculate {<filename>}\n", ERROR_CODE_RANGE);

    // Reading number of iterations param from file    
    readDataFromFile(PARAMS_FILENAME, params, TOTAL_PARAMETERS);
    
    totalIterations = params[0];

    if(DEBUG)
    {
        printf("%d\n", totalIterations);
    }

    // Checking parameters ranges
    if((totalIterations < 1) || (totalIterations > 100))
    {
    	error("Number of iterations must be between 1 and 100\n", ERROR_CODE_RANGE);
    }

    printf("Enter x (between -100 to 100):\n");
    scanf("%lf", &x);
    if((x < -100) || (x > 100))
    {
        error("Number x must be between -100 and 100\n", ERROR_CODE_RANGE);
    }

    f = sinFunction(x, totalIterations);

	// Print calculated value and result of function sin(x) from math library to check correctness of calculations
    printf("sin(x) = %f (%f)\n", f, sin(x));

    if(argc == 2)
    {	
		// Saving result to file    
		sprintf(msg, "sin(x) = %lf\n", f);
		if(!saveToFile(argv[1], msg))
		{
			error("File I/O error\n", ERROR_CODE_FILE);
		}
	}

    return 0;
} 
