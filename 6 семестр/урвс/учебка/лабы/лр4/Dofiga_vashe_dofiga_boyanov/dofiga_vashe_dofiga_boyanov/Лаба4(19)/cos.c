#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h> 
void OnSignal (){;}
int main (int argc, char *argv[]) {
    float x,cos;
    int pd[2];					// pipe r/w discriptor
    sscanf(argv[0],"%d",&pd[0]);			// get pd numbers
    sscanf(argv[1],"%d",&pd[1]);
    signal(SIGINT, OnSignal);
    pause();
    read(pd[0],&x,sizeof(x));			// read x from pipe
    cos=1-x*x/2.+x*x*x*x/24.-x*x*x*x*x*x/720.;
    write(pd[1],&cos,sizeof(cos));			// write cos into pipe
}
