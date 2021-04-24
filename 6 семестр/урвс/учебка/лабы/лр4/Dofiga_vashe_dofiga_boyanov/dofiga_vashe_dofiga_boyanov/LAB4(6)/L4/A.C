#include <sys/stat.h>
#include <fcntl.h>
#include<stdio.h>
#include<math.h>
#include<string.h>

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

first(){
	FILE* ofile,*txt;
	char s[14]="file.bin";
	char s2[14]="tmp.txt";
	int m=10,n=30000,j;
	float a[1],x,inter=1.0;

	if((ofile=fopen(s,"w"))==NULL)return 1;
	fclose(ofile);
	for(j=0;j<=m;j++){
		x=inter*j/m;
        	a[0]=f(x,n);
		if((ofile=fopen(s,"a"))==NULL)return 1;
		fwrite((void *)a,sizeof(float),1,ofile);
		fclose(ofile);
        };
	printf("\nEnd first");

	if((txt=fopen(s2,"w"))==NULL)return 1;
	fprintf(txt,"stop");
	fclose(txt);
        return 0;
};

second(){
	FILE *txt;
	char s[14]="file.bin";
	char s2[14]="tmp.txt",str[10]="";
	int fd;
	float a[1];
	while((fd=open(s,O_RDONLY))==-1);
	while(strcmp(str,"stop")){
		if((txt=fopen(s2,"r"))!=NULL){
			fscanf(txt,"%s",&str);
			fclose(txt);
		};
		if(read(fd,a,sizeof(float)))printf("\nsecond:%20f",a[0]);
        };
};

main(){
	int id,id2;
	id=fork();
	if(id){first();wait(&id2);}
	else second();
	return 0;
};