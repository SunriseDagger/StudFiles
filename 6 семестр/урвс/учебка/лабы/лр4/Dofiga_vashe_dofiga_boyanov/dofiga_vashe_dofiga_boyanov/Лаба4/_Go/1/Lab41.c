#include <fcntl.h>

#define PI 3.141592
#define NCALC 20
#define N 10

int main()
{
    	int i,j;
	int k,khlp;		
    	int fd; 			// temporary file descriptor
	pid_t pid; 		// child process pid
    	double f,x,hlp; 		// function value and its argument    
	int stop;

	pid = fork();
    	if(pid == -1)
    	{
    		printf("!!! Can't create child process !!!\n");
		exit(1);
	}
    	if(pid)
    	{
    		if((fd = creat("temp.bin",0666)) == -1)
	    	{
			printf("!!! Error creating file temp.bin !!!\n");
			exit(1);
		}
  	    	for(i = 0; i < NCALC; i++)
		{
			f = 0;
			x = i*0.05*PI;
			for(k = 0; k < N+1; k++)
	    		{
				khlp=2*k+1;    			
				hlp=1;
				for(j = 0; j < khlp; j++)
				    hlp*=x;
				f += hlp / khlp;
		    	}
    			write(fd, &f, sizeof(double));
    		}
	    	f = -1; // stop number
    		write(fd, &f, sizeof(double));
	    	close(fd);
    		wait();
    		if(remove("temp.bin") == -1)
		{
			printf("!!! Can't remove temporary file !!!\n");
			exit(1);
		}
	}     
	else
    	{
    		if((fd = open("temp.bin", O_RDONLY)) == -1)
    		{
			printf("!!! Error opening file temp.bin !!!\n");
			exit(1);
		}
        	f = 0;
    		stop = 0;
		while(!stop)
    		{
			if(read(fd, &f, sizeof(double)))
	    		{
				if (f != -1) printf("f(x) = %lf\n", f);
				else stop=1;
	   		}
    		}
		close(fd);
	}
    	exit(0);
}
