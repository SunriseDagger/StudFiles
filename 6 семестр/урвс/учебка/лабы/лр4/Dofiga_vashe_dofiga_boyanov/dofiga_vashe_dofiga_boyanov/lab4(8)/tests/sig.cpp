#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int c;
void sigfunc(int signum)
{
    cout<<"I'm in!"<<endl;
    signal(SIGUSR1, sigfunc);
    c++;
}

int main(int argc, char *argv[])
{   int pid, fd;     
    if(!(pid=fork()))    
    {     
	int son = 0;
	signal(SIGUSR1, sigfunc);
	while(!c) sleep(1);
	fd = creat("tm.tmp",0666);
	for(int i=0; i<3; i++) 
	{
	    son++;      
	    write(fd,&son, sizeof(son));
	    cout<<"son = "<<son<<endl;
	}    
	close(fd);
    } 
    int mon = 0;
    sleep(1);	
    kill(pid, SIGUSR1);
    fd = open("tm.tmp",O_RDONLY);
    for(int i=0; i<3; i++)
    {	
	read(fd, &mon, sizeof(int));
	cout<<"mon = "<<mon<<endl;
    }	 
}

