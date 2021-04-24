#include <stdio.h>

#include <stdlib.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <unistd.h>

#include <sys/wait.h>
#define pi 3.14159265

main(int argc,char *argv[])

{
int f,f1;

int i,j,k,m,n,l,st;

float x,y,s;

struct stat info;
FILE *res;
if(argc!=1)
{printf("No parameters expected\n");
exit(0);
}
printf("Input n- for sum for i=1 to n\n");scanf("%d",&n);

printf("Input m- how many points on the interval [0;pi]\n");scanf("%d",&m);

f1=open("file",O_WRONLY|O_CREAT,0644);

  if (fork()==0)
  {
  f=open("file",O_RDONLY);
  res=fopen("result","w");
  l=1;

  for(;;)
    {
    j=stat("file",&info);
    if(info.st_size>=4*l)

       {
       read(f,&s,sizeof(s));
       if(s==-100)break;

       fprintf(res,"f(%f*pi)=%f\n",(l-1.0)/m,s);

       l++;

       };
    };

  fclose(res);
  close(f);
  exit(0);
  };


for(i=0;i<m;i++)

{

x=(i+0.0)*pi/m;

y=x;
s=x;

for(k=1;k<=n;k++)
{
s=s*x*x;
y=y+s/(2*k+1);
printf("s=%f,y=%f",s,y);
};
printf("\n");
write(f1,&y,sizeof(y));

};

y=-100;

write(f1,&y,sizeof(y));
close(f1);
wait(&st);
exit(0);

}
