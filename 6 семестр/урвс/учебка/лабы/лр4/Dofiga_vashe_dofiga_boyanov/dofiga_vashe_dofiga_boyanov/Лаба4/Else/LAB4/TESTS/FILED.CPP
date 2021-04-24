#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main(int argc, char* argv[])
{
    long offset;
    int fd;
    if(!fork())
    {
	fd = creat("testf.tmp",0666);
	for(int i=0; i<10; i++)
	{
	    //write(fd, &i, sizeof(int));
	    offset = lseek(fd,0L,1);//return current pointer
	    write(fd, &i, sizeof(int));
	    cout<<"pointer write= "<<offset<<endl;
	}
	close(fd);
    }else{
	    fd = open("testf.tmp",O_RDONLY);
	    for(int i=0; i<10; i++)
	    {
		//read(fd, &i, sizeof(int));
		offset = lseek(fd,0L,1);//return current pointer
		read(fd, &i, sizeof(int));
		cout<<"pointer read= "<<offset<<endl;
	    }
	    close(fd);
	 }

}

