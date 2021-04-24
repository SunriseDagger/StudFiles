#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

struct elem
{
int num_proc;
int result;
int des[2];
};
elem el_1,el_2,el_3;

 

main(int argc,char *argv[])
{
int f,f1,f2,f3;
int i,j,m,n,k,l,st;
int s1=1,s2=1,s3=1,result;
struct stat info;

int fldes[2];

f1=open("file",O_CREAT|O_WRONLY);
f2=open("file",O_RDONLY);

fldes[0]=f2;  //for reading
fldes[1]=f1;  //

//pipe(fldes);



FILE *res;
 
int i1,i2,i3;
printf("Input n \n");scanf("%d",&i1);
printf("Input k \n");scanf("%d",&i2);

if ((i1<0)||(i2<0)) {perror("\nBAD DATA\n"); exit(1); }
pid_t pid1,pid2,pid3; 

if((pid1=fork())==0){
    
    el_1.num_proc=1; 
    el_1.result=i1;
    el_1.des[0]=fldes[0];   
    el_1.des[1]=fldes[1];


	char op[10];
	sprintf(op,"%d",fldes[0]); //deskriptor for reading

write(fldes[1],&el_1,sizeof el_1);
execl("son1","son1",op,0);

printf("\n--- Child process---\n");
perror("Error executing program son");
exit(0);
}

if((pid2=fork())==0){
    
    el_2.num_proc=2; 
    el_2.result=i2;
    el_2.des[0]=fldes[0];   
    el_2.des[1]=fldes[1];
    
    char op[10];
    sprintf(op,"%d",fldes[0]); //deskriptor for reading

write(fldes[1],&el_2,sizeof el_2);
execl("son1","son1",op,0);

printf("\n--- Child process---\n");
perror("Error executing program son");
exit(0);
}


if((pid3=fork())==0){
    el_3.num_proc=3; 
    el_3.result=i1-i2;
    el_3.des[0]=fldes[0];   
    el_3.des[1]=fldes[1];

    char op[10];
    sprintf(op,"%d",fldes[0]); //deskriptor for reading

write(fldes[1],&el_3,sizeof el_3);
execl("son1","son1",op,0);
    
printf("\n--- Child process---\n");
perror("Error executing program son");
exit(0);
}               


          printf("--- Father process---\n");
    

sleep(1);
             kill(pid1,SIGINT); 
	     kill(pid2,SIGINT);
	     kill(pid3,SIGINT);
	    wait(&st);
	    wait(&st);
	    wait(&st);

elem el;
for (i = 0; i < 3; i++)
          {  read(fldes[0],&el,sizeof el);
       		if (el.num_proc == 1)
		  el_1 = el;
		else 
		  if (el.num_proc == 2) 
		    el_2 = el;
		  else 
		    el_3 = el;
          } 

result = el_1.result / (el_2.result * el_3.result);
printf ("\nresultat=%d\n\n", result);

              printf("Process #%d died with code %d\n",pid1,st);


close(f1);
close(f2);
remove("file");
exit(0);
}



