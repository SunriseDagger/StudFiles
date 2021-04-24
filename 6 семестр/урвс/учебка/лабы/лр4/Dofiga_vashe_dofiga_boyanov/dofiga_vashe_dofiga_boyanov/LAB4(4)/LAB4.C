#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
main()
{
struct stat st;
FILE *f,*f1;
int status;
unsigned long int n,r,ni[15];
unsigned long int i,j,k,l,m,p,q,res;
unsigned long int fac[15];
puts("Input n");scanf("%d",&n);
f=fopen("input","wb");
fwrite(&n,4,1,f);
puts("Input r");scanf("%d",&r);
for(i=1;i<r+1;i++)
{printf("Input n(%d)\n",i);
scanf("%d",&j);ni[i]=j;
fwrite(&j,4,1,f);};
fclose(f);
q=0;
for(i=1;i<r+1;i++)
q=q+ni[i];
if((q!=n)||(r>n)||(n<=0)||(r<=0))
{puts("incorrect numbers");
exit(0);
};
if (fork()==0)
{f=fopen("input","r");
fread(&j,4,1,f);
fclose(f);
k=1;
for(l=2;l<j+1;l++)k=k*l;
f1=fopen("res","ab");
fwrite(&k,4,1,f1);
fclose(f1);
exit(0);
};


for(i=1;i<r+1;i++)
{if(fork()==0)
{
f=fopen("input","r");
fseek(f,i*4,0);
fread(&j,4,1,f);
fclose(f);
k=1;
for(l=2;l<j+1;l++)k=k*l;
f1=fopen("res","ab");
fwrite(&k,4,1,f1);
fclose(f1);
exit(0);
};
};
for(i=1;i<r+2;i++)wait(&status);
for(;;)
{

f=fopen("res","r");
stat("res",&st);
if(st.st_size>=(r+1)*4)
{fread(&m,4,1,f);
fac[1]=m;
for(i=1;i<r+1;i++)
{fread(&m,4,1,f);
fac[i+1]=m;};
break;
};};
p=1;
for(i=1;i<r+1;i++)
{if(fac[i+1]>fac[p])p=i+1;};
res=fac[p];
for(i=1;i<r+2;i++)
{if(i!=p)res=res/fac[i];};
printf("Result:%d\n",res);
fclose(f);
exit(0);

}


