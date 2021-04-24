#include <sys/stat.h>
#include<stdio.h>
#include<math.h>
#include<string.h>
#include<signal.h>
float f(x,n) float x;int n;
{
	int k;
	float s=x,xx=x;
	for(k=3;k<2*n+1;k=k+2){ 
		xx=xx*x;
		s=s+xx/k;
	};
	return s;
};

first(id,fd)
	int fd[2];
	int id;
{
	int m=10,n=30000,j;
	long int i;
	float a[1],x,pi=3.1415926,inter=1.0;
	sleep(1);
	for(j=0;j<=m;j++){
		x=inter*j/m;
        	a[0]=f(x,n);
		write(fd[1],a,sizeof(float));
        };
	write(fd[1],a,sizeof(float));
	close(fd[1]);
	sleep(1);
	kill(id,SIGSTOP);
	printf("\nEnd first");
        return 0;
};

int itoa(i,str2)
    int i;
    char* str2;
{
    int p,j;
    char* str;
    str=str2;
    j=i;
    for(p=1;j>=10;j/=10)p*=10;
    for(;p>0;p/=10){
	*str++='0'+i/p;
	i%=p;
    };
    *str='\0';
};

main(){
	int id,fd[2];
	char str[10];
	pipe(fd);
	itoa(fd[0],str); 
	id=fork();
	if(id){ first(id,fd);}
	else execl("b.out",str,0);
	return 0;
};