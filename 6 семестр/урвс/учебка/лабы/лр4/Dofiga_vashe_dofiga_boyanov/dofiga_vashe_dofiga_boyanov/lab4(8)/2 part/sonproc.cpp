#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>


using namespace std;

struct randpare
{
    double x;
    double y;
}pare;

int main(int argc, char *argv[])
{
    int fd,  num;
    fd = atoi(argv[0]);
    num = atoi(argv[1]);
    
    for(int i=0;i<num; i++)
    {
	pare.x = ((double)rand())/RAND_MAX;/*[0..1]*/
	pare.y = 4*((double)rand())/RAND_MAX;/*[0..4]*/ 
	write(fd,&pare,sizeof(pare));
    }
    cout<<"son process, argument= "<<argv[0]<<endl;
    return 0;
}

