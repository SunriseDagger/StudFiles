#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
struct stat buf;
char *ctime();
int fd1,fd2;
int i=0;
//fork();
/************************* pros1 **********************************/
void process(char  *fl1,char *fl2 )
{
// int fd1,fd2;

fd1=open(fl1,0);
//i=i+1;
//fstat(fd1,&buf);
/*c=ctime(&(buf.st_mtime));*/
printf("\n %s           %d    ",fl1,fd1);
 
fd2=open(fl2,0);
//i=i+1;
//fstat(fd2,&buf);
/*t=(buf.st_mode&S_IFMT);
*c=ctime(&(buf.st_mtime));*/
printf("\n %s           %d  ",fl2,fd2);
//close(fd1);
//close(fd2);
}
int main(void)
{//int fd1,fd2;
int t;                                        
/*char *c;                                      
printf("tabl. otkr. failov ");                
printf("\n |   file name    |  fd  |  "); 
printf("\npros 0 otkr 2 fila ");
process("f1.txt","f2.txt");
close(fd1);
close(fd2);
printf("\npros 1 otkr 2 fila ");
process("f1.txt","f3.txt");
close(fd1);
close(fd2);
printf("\npros 2 otkr 2 fila ");
process("f1.txt","f2.txt");
close(fd1);
close(fd2);
printf("\npros 0 porogd pros 3");
process("f1.txt","f2.txt");     
close(fd1);
close(fd2);
printf("\npros 1 pogogd pros 4 & otkr fail");
process("f1.txt","f3.txt");
process("f2.txt","f4.txt");
close(fd1);
close(fd2);    
close(fd1);
close(fd2); 
printf("\npros 2 porog pros 5 & otkr 2 fila ");
process("f1.txt","f2.txt");
process("f4.txt","f1.txt");
close(fd1);
close(fd2);     
*/
//forkk();
//goto t;
//ps();
/**************************** pros2 *****************************/
//fd1=open("fil.txt",0);                                   
stat("fil.txt",&buf); 
/*i=buf.st_dev;                                       
c=ctime(&(buf.st_mtime));                           
printf("\n f2.txt           %d     %d",fd1,i); 

fd2=open("f3.txt",0);                                   
fstat(fd2,&buf);
i=buf.st_dev;                                        
c=ctime(&(buf.st_mtime));                           
printf("\n f3.txt           %d     %d",fd2,i); 
*/
if((buf.st_mode&S_IFMT)==S_IFREG) {printf("\noioiioi");}
printf("\nOk\n");
//t:
return 0;
}

