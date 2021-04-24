#include <sys/types.h>
#include <sys/wait.h>         
#include <unistd.h>
#include <stdio.h>               
#include <fcntl.h>        

int fd_w, fd_r;

int main()
{
    int 	pid1, pid2;
    const int 	MAX = 1000;
    double 	pi = 0, cos = 1, z, f, x, x1, x2;
    int 	getpid1, getpid2, time;
    
    printf("Input x: ");
    scanf("%lf", &x);
    
    creat("common.txt", 0666);
    fd_w = open("common.txt", O_WRONLY, 0666);
        
    if( (pid1 = fork()) == 0)
    {
	double t = 1;
	z = -1;
        for( int i = 1; i < MAX; i++ )
	{
	    t *=  (x * x) / ( (2 * i - 1) * (2 * i) );
	    cos += z * t;
	    z *= -1;
	}    
        getpid1 = getpid();
    	write(fd_w, &getpid1, sizeof(int));
	write(fd_w, &cos,  sizeof(double));
    }
    else
    {	
	if( (pid2 = fork()) == 0)
	{
	    z = 4;
	    for( int i = 0; i < MAX; i++ )
	    {
		pi += z / (2 * i + 1) ;
		z *= -1;
	    }
	    getpid2 = getpid();
	    write(fd_w, &getpid2, sizeof(int));
	    write(fd_w, &pi,   sizeof(double));
	}
	else
	{
	    wait(&time);
    	    fd_r = open("common.txt", O_RDONLY, 0666);
	    printf("Parents process is reading...\n");
	    read(fd_r, &getpid1, sizeof(int));
	    read(fd_r, &x1, sizeof(double));
	    read(fd_r, &getpid2, sizeof(int));
	    read(fd_r, &x2, sizeof(double));
	    close(fd_r);
	    if(getpid1 == pid1)
	    {
		cos = x1;
		pi  = x2;    
	    }
	    else
	    {
		cos = x2;
		pi  = x1;
	    }
	    
	    f = (1 - cos) / (pi * x * x);
	    printf("x   = %f\n", x);
	    printf("cos = %f\n", cos);    
	    printf("pi  = %f\n", pi);    
	    printf("f(x)= %f\n", f);	
	}
    }		
    close(fd_w);				
    return 1;
}