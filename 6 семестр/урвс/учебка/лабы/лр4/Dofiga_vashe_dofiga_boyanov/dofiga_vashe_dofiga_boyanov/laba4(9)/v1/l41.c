#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>

struct TCell {
    int pid;
    float data;} cell;

int main() {
    int fd,pid_pi,pid_exp;
    float f,x,exp,pi;
    char *fname="file.tmp";
    struct stat tmp_stat;
    
    fd=open(fname,O_RDWR|O_CREAT);    // create file for data transfer
    printf("\n Enter x=");
    scanf("%f",&x);  
/*================================================================================*/
    pid_pi=fork();
    if(pid_pi==0) {          // create child_1
  cell.pid=getpid();
  cell.data=4.-4./3.+4./5.-4./7.+4./9.-4./11.+4./13.;
//  printf ("\n %d: pi=%f",cell.pid,cell.data);  
        write(fd,&cell,sizeof(cell));
        exit(0); }

    pid_exp=fork();
    if(pid_exp==0) {          // create child_2
  cell.pid=getpid();
  cell.data=1-x*x/2+x*x*x*x/8;
//  printf("\n %d: exp(x*x)=%f",cell.pid,cell.data);
  write(fd,&cell,sizeof(cell));
  exit(0); }

    do { fstat(fd,&tmp_stat); }
    while(tmp_stat.st_size != 2*sizeof(cell));
/*================================================================================*/    
    lseek(fd,0,SEEK_SET);  
    
    read(fd,&cell,sizeof(cell));
  if(cell.pid==pid_pi) pi=cell.data;
  if(cell.pid==pid_exp) exp=cell.data;
//  printf("\n first cell.pid=%d cell.data=%f",cell.pid,cell.data);

    read(fd,&cell,sizeof(cell));
  if(cell.pid==pid_pi) pi=cell.data;
  if(cell.pid==pid_exp) exp=cell.data; 
//  printf("\n second cell.pid=%d cell.data=%f",cell.pid,cell.data);
  
    f=exp/(2*pi);          // need sqrt(2*pi), but sqrt() not found
    printf("\n pi=%f",pi);
    printf("\n exp(-x*x/2)=%f",exp);
    printf("\n f(%f)=%f\n",x,f);
    close(fd);
    remove(fname);           // delete temporary file

    waitpid(pid_pi,NULL,0);
    waitpid(pid_exp,NULL,0);
//    wait(0); 
//    wait(0);
    sleep(2);
    return 1;
}
     
     
     



