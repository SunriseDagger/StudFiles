#include<stdio.h>
//#include<conio.h>
#include<math.h>
#include<fcntl.h>
#include<string.h>

int faktorial(int q)
{ int sum=1,i;
  if (q==0) return 0;
 // int sum=1;
  for (i=1;i<=q;i++) sum=sum*i;
  return sum;

}
void potomok()
{ char c[20];
  double y;
  pause();
  FILE * stream=fopen("qwe","r");
//  while (strcmp("STOP",c)!=0)
  for(;;)
 { if(fscanf(stream,"%s",&c)!=-1)
                  printf("\n%s",c);
   }
 
 }

int main()
{ int N,k=1,pid;
  double x=0,x1=0,slx,xp,xp1;
  FILE * stream=fopen("qwe","w");

 pid=fork();
 if (pid==0) { potomok(); 
              // exit(1);
               }
 //signal o nahale rabota
?? kill(pid,pid) 
 for(N=1;N<=9;N++)
  { slx=3.14*k/N;
    for(k=1;k<=N;k++)
     { x1=faktorial(2*k-1);
       xp=pow(-1,k+1);
       xp1=pow(slx,(2*k-1)/x1); 
     x=x+(xp * xp1);
      }
  fprintf(stream,"%f\n",x);
  }
  fprintf(stream,"STOP");
  fclose(stream);
  exit(pid); //mochim potomka
  return(1);
}