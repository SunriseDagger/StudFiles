#include<stdio.h>

main(argc,argv)
	int argc;
	char* argv[];
{
	int fd2;
	float a[1];
	fd2=atoi(argv[0]);
	while(1){
		while((read(fd2,a,sizeof(float)))==-1);
		printf("\nsecond...%f",a[0]);
	}
};