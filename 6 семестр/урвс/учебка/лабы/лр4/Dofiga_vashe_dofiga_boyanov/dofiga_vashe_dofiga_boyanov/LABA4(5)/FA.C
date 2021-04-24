#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
main (int argc, char *argv[])
{
	int n,fd,number,result=1,tmp[2];char c;
	strcpy(&c,argv[1]);
	n=c;c='\0';
	strcpy(&c,argv[2]);fd=c;c='\0';
	if (argc==3) number=0;
	else
		{strcpy(&c,argv[3]);number=c;}
	tmp[0]=number;
	while(n>0)
		{result*=n;n--;}
	tmp[1]=result;
	write(fd,tmp,8);
	return 0;
}
