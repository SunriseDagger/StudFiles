#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#define N 30000
struct rec
{ double data;
  int type;
};
int main()
{ double x,pi,cos,t;
  unsigned i,fd;
  long s;
  struct rec rec;
  printf("Enter X:"); scanf("%lf",&x);
  fd=creat("lab4.tmp",0666);
  if (fd==-1) { perror("Error"); exit(1); }
  if (!fork())
  { cos=1; t=1;
    for (i=1;i<N;i++) 
    { t*=x*x/((2*i-1)*2*i);
      if (i%2) cos-=t; else cos+=t;
    } 
    rec.data=cos; rec.type=1;
    lseek(fd,0,2);
    write(fd,&rec,sizeof(rec));
    close(fd); exit(0);
  }
  else
    if (!fork()) 
    { pi=0;
      for (i=0;i<N;i++) 
        if (i%2) pi-=1./(2*i+1);  
            else pi+=1./(2*i+1);
      rec.data=pi*4; rec.type=2;
      lseek(fd,0,2);
      write(fd,&rec,sizeof(rec));
      close(fd); exit(0);
    }
  close(fd);
  fd=open("lab4.tmp",O_RDONLY);
  for(i=0;i<2;)
  { s=read(fd,&rec,sizeof(rec));
    if (s)
    { i++;
      if (rec.type==1) cos=rec.data;
                  else pi=rec.data;
    }
  }
  close(fd);
  printf("cos=%lf\n",cos);
  printf("pi =%lf\n",pi);
  x=(1-cos)/(pi*x*x);
  printf("Result: %lf\n",x);
}