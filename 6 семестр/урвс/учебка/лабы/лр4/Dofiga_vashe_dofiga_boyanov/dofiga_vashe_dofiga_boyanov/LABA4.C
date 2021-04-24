#include<stdio.h>
#include<fcntl.h>
#define N 10
union data
{ float x;
  int m;
} d;
int main()
{ int i,j,k,fd2,stop;
  FILE *fd1;
  float s,h;
  char str[32];
  fd2=creat("file2",0666);
  puts("Parent: creating child process");
  if (!fork())
  { close(fd2); stop=0;
    fd2=open("file2",O_RDONLY);
    puts("Child : reading...");
    do
    { while (read(fd2,str,32)==0);
      if (str[0]=='s' && str[1]=='t' && str[2]=='o' && str[3]=='p') stop=1;
      else
      { for (i=31,d.m=0;i>=0;i--) d.m=d.m*2+(str[i]=='1');
        printf("Child : %f\n",d.x);
      }
    } while(!stop);
    puts("Child : exit");
    close(fd2); exit(0);
  }
  fd1=fopen("file1","r");
  while (fscanf(fd1,"%f",&d.x)!=EOF)
  { printf("Parent: read %f\n",d.x);
    h=1; s=d.x;
    for (i=1;i<=N;i++) { h*=d.x*d.x; s+=h/(2*i+1); }
    d.x=s;
    printf("Parent: write %f\n",d.x);
    for(i=0;i<32;i++)
    { str[i]=(d.m%2)+'0';
      d.m/=2;
    }
    write(fd2,str,32);
  }
  strcpy(str,"stop");
  write(fd2,str,32);
  fclose(fd1);
  close(fd2);
  wait(&i);
  return 0;
}