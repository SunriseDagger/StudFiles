#include <fstream>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

#define MAX 1000000

double f(double x)
{
    return 4/(1+x*x);
}

int main(int argc, char *argv[])
{
    int num = 0;
    double x,y, integral;
    for(int i=0; i<MAX; i++)
    {
	x = ((double)rand())/RAND_MAX;
	y = 4*((double)rand())/RAND_MAX;
	if(f(x)>=y)
	{
	    num++;
	}    
    }
    integral = 4*((double)num)/MAX;
    cout<<"int= "<<integral<<endl;
    return 0;
}

