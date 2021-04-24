
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <cstdlib>

using namespace std;

struct randpare
{
    double x;
    double y;
} pare;

double catalog_f(double x)
{
    return(4/(1+x*x));
}

int main(int argc, char** argv)
{
    ofstream out;
    
    int i, fd, x, y, checkin = 0, num;
    double integral;
    if(argc!=2)
    {
	cout<<"Input program argument!"<<endl;
	cin>>num;
    }else num = atoi(argv[1]);
    
    if(fd==-1)
    {
	cout<<"Can't create connection file!"<<endl;
	exit(1);
    }
    if(!fork())
    /* daughter process*/
    {
	fd = creat("connectfile.tmp",0666);
	for(int i=0; i<num; i++)
	{
	    pare.x = ((double)rand())/RAND_MAX; /*[0...1]*/
	    pare.y = 4*((double)rand())/RAND_MAX; /*[0..4]*/
	    write(fd, &pare, sizeof(pare)); 
	}
	close(fd);
	exit(0);	
    }
    else 
    {  /*main process*/
	fd = open("connectfile.tmp",0666);
	for(int i=0; i<num; i++)
	    if(read(fd, &pare, sizeof(pare)))
	    {	
		if(catalog_f(pare.x)>=pare.y)	
		{
		    checkin ++;
		}    
	    }	    
	close(fd);
	/*integral = squre(0..1;0..4)*integral*/
    	integral = 4*((double)checkin)/num;
	cout<<"in points: "<<checkin<<endl;
    	cout<<"Integral: "<<integral<<endl;
    }	
    exit(0);
}
