#include<stdio.h>        
#include<sys/types.h>    
#include<unistd.h>       
#include<fcntl.h>        
#include<sys/wait.h>     

int b;                  //diskriptor temp-file;

//***** Function for  caliculating factorial******************
int fac(int n)
{
    int j,r = 1;
    for (j=1; j<=n; j++)
	    r = r*j;
    return r;	    
}
//***** Function for  caliculating a in power n ********************
double pow (double a, int n)
{
    int i;
    double r = 1 ;
    for (i=1; i<=n; i++)
	r = r*a;
    return r;	 
}

int main()
{
    int a1, a2, c, i, n1 = 30, n2 = 100;
    double pi = -0.5, e2 = 0., f, x1, x2;
    int getp1, getp2, qw;
    
    creat("common.txt",0666);             //make temp-file;
    b = open("common.txt",O_WRONLY,0666); //open temp-file to b;
    
    a1 = fork();                         //make 1 son-process;
    if(a1 == 0)
    {
	printf("Enter to 1 doter process....\n");
        for (i=0; i<=n1; i++)            //caliculating e in power 2;
	    e2 =e2 +pow(1,i)/fac(i);
        getp1 = getpid();                //PID of 1 son-process;
	sleep(12);
	printf("1 doter process is writting....\n");
    	    write(b, &getp1, sizeof(int));   //write result to temp-file;
	    write(b, &e2, sizeof(double));
	    
	printf("Go on 1 doter process....\n");
    }
    else
    {	
	a2 = fork();                       //make 2 son-process;
	if(a2 == 0)
	{
	    printf("Enter to 2 doter process....\n");
	    for (i=1; i<=n2; i++)                //caliculating pi;
		pi = pi + pow(-1.,i-1) / i;
	    pi = 2*pi;	
	    
	    getp2 = getpid();              //PID  2 son-process;
	    sleep(12);
	    printf("2 doter process is writting....\n");
		write(b, &getp2, sizeof(int));     //write result to temp-file;
		write(b, &pi, sizeof(double));
	    printf("Go on 2 doter process....\n");
	}
	else
	{
	    printf("Enter to parents process....\n");
	    wait(&qw);                    //wait, until 1 & 2 son-proces working;
    	    c = open("common.txt", O_RDONLY, 0666);
		printf("Parents process is reading...\n");
		read(c, &getp1, sizeof(int));   //read result from temp-file;
		read(c, &x1, sizeof(double));
		read(c, &getp2, sizeof(int));
		read(c, &x2, sizeof(double));
	    close(c);
	    printf("Output result....\n\n");
	    if(getp1 == a1)   //definition e2 & pi in PID son-process;
	    {
		e2 = x1;
		pi = x2;    
	    }
	    else
	    {
		e2 = x2;
		pi = x1;
	    }                   // output result;
	    printf("e2 = %f\n",e2);    
	    printf("pi = %f\n",pi);    
	    f = 1./(pi* (1 + e2));
	    printf("F= %f\n",f);	
	}
    }		
    close(b);				
    return 0;
}