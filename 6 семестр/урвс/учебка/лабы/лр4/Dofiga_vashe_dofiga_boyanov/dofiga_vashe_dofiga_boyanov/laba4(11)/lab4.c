#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
struct TCell
{					// structure for data transfer
    int pid;
    float data;
} cell;
int main()
{
    int fd,pid_pi,pid_cos;
    float f,x,cos,pi;
    char *fname="file.tmp";
    struct stat tmp_stat;
    fd=open(fname,O_RDWR|O_CREAT);		// create file for data transfer
    printf("\n Enter x=");
    scanf("%f",&x);	
/*================================================================================*/
    pid_pi=fork();
    if(pid_pi==0)
	{					// create child_1
		cell.pid=getpid();
		cell.data=4.-4./3.+4./5.-4./7.+4./9.-4./11.+4./13.;
		write(fd,&cell,sizeof(cell));
		exit(0);
	}
    pid_cos=fork();
    if(pid_cos==0) 
	{				// create child_2
		cell.pid=getpid();
		cell.data=1-x*x/2.+x*x*x*x/24.-x*x*x*x*x*x/720.;
		write(fd,&cell,sizeof(cell));
		exit(0); 
	}
    do {
		fstat(fd,&tmp_stat); 
		}while(tmp_stat.st_size != 2*sizeof(cell));
/*================================================================================*/    
    lseek(fd,0,SEEK_SET);	
    read(fd,&cell,sizeof(cell));
	if(cell.pid==pid_pi) pi=cell.data;
	if(cell.pid==pid_cos) cos=cell.data;
    read(fd,&cell,sizeof(cell));
	if(cell.pid==pid_pi) pi=cell.data;
	if(cell.pid==pid_cos) cos=cell.data; 
    f=(1-cos)/(pi*x*x);					
    printf("\n pi=%f",pi);
    printf("\n cos(x)=%f",cos);
    printf("\n f(%f)=%f\n",x,f);
    close(fd);
    remove(fname); 				// delete temporary file
    waitpid(pid_pi,NULL,0);
    waitpid(pid_cos,NULL,0);
    sleep(2);
}
