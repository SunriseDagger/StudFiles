#include <sys/stat.h>
#include <fcntl.h>
#include<stdio.h>
#include<math.h>
#include<string.h>

float func(float x,int n) 
{
	int k;
	float s=0.,c=x*x,r=x;
	for(k=3;k<=2*n+1;k=k+2){
		r=r*c;
		s=s+r/k;
	};
	return s;
};

predok()
{
	FILE *bin,*txt;
	int m=10,n=300,j;
	float a[1],x,inter=1.0;

	if((bin=fopen("f1.bin","w"))==NULL)return 1;
	fclose(bin);
	for(j=0;j<=m;j++)
          {
		x=inter*j/m;
        	a[0]=func(x,n);
		if((bin=fopen("f1.bin","a"))==NULL)return 1;
		fwrite((void *)a,sizeof(float),1,bin);
		fclose(bin);
           };
	printf("\nEnd predok");

	if((txt=fopen("tmp.txt","w"))==NULL)return 1;
	fprintf(txt,"stop");
	fclose(txt);
        return 0;
};

potomok()
{
	FILE *txt;
	str[10]="";
	int fd;
	float a[1];
	while((fd=open("f1.bin",O_RDONLY))==-1);
	while(strcmp(str,"stop"))
        {
		if((txt=fopen("bmp.bin","r"))!=NULL)
                {
			fscanf(txt,"%s",&str);
			fclose(txt);
		};
		if(read(fd,a,sizeof(float)))printf("\n%f",a[0]);
        };
};

main()
{
	int id;
	
	if(fork()==0){predok();wait(&id);}
	else potomok();
	return 0;
};