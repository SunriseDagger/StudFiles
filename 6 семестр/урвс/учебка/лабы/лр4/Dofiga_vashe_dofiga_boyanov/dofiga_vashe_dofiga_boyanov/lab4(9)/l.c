#include<stdio.h>

#include<math.h>
#include<fcntl.h>


void potomok1()
{ 
  float y;
  int n,i,p;
  FILE * stream=fopen("in","w");
  y=1;
  for(n=1;n<=9;n++)
   for(i=n,p=1;i>1;i--)
    p=p*i;
  y=y+pow(2,n)/p;
  fprintf(stream,"%f %c\n",y ,'!');
 }

void potomok2()
{
  float pi;
  int n;
  FILE * stream=fopen("in","w");
  pi=1;
  for(n=1;n<=9;n++)
   pi=pi+pow((-1),n)*4/(2*n+1);
   fprintf(stream,"%f %c\n",pi,'*');
}
int main()
{ int id;
  float x,y,pi;

  FILE * stream=fopen("in","r");


id=fork(); //vozvrashaet nomer porogdennogo prozesa podit. pr-su
 if (id==0) { potomok1(); exit(1); }
// fscanf(stream,"%f\n",&y);
 id=fork();
 if (id==0) { potomok2(); exit(1); }
// fscanf(stream,"%f",&pi);
 for(;;)
 if(fscanf(stream,"%f %c",&d,&c)!=0)
 if(c=='!')
{ y=d;
 break;
 }
 else
// if(fscanf(stream,"%f %c",&d,&c)!=0)

if(c=='*')
{d=pi;
 break;
 }
 x=1/(pi*(1+y));
 fprintf(stream,"%f\n",x);

 fclose(stream);
 return(1);
}