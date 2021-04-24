#include<stdio.h>
#include<io.h>
#include<math.h>
#include<fcntl.h>
#include<string.h>

void potomok()
{
	char c[20];
	float y=1;
  	int i=1;
	int str = open("qwe",O_RDONLY);
  	do
   	{
		read(str,&y,4);
		if (y==1) printf("\n!!STOP");
    		else printf("\n** %f",y);
   	} while(y!=1);
}

int main()
{
	int N,k=1,pid;
  	float x=0,x1=0,slx,xp,xp1;
  	int str = open("qwe",O_WRONLY|O_CREAT);
	for(N=1;N<=6;N++)
  	{
		slx=3.14*k/N;
    		for(k=1;k<=N;k++)
     		x=pow(slx,2*k+1)/(2*k+1);
  		write(str,&x,4);
  	}
  	x=1;
  	write(str,&x,4);
	potomok();
	return(1);
}