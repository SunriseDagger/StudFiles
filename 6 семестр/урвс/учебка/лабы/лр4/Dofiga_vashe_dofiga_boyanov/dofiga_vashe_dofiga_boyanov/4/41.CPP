#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define PI 3.1415926

int main() {
  int N, M;
  printf("Input N = ");				// input N & M
  scanf("%d",&N);	
  printf("Number of steps = ");
  scanf("%d",&M);
  int fd=open("common",O_CREAT,0600);		// clear common file
  close(fd);
  char ch;
  if (fork()==0) { 				// child's process code
    fd=open("common",O_WRONLY);			// open for writing
    int i, k;
    float x, sum, expr;
    for (i=0; i<=M; i++) {
      x=i*PI/M;
      expr=x;
      for (k=0; k<=N; k++) { sum=expr/(2*k+1); expr*=x*x; }	// calc sum
      write(fd,&sum,sizeof(float));		// write sum in common file
    }
    sum=-1;					
    write(fd,&sum,sizeof(float));		// write end flag
    close(fd);					// close file
  } else {					// child's process code
    fd=open("common",O_RDONLY);			// open for reading
    float buf;
    while (1) {
      while (read(fd,&buf,sizeof(float))==0);	// waiting event
      if (buf<0) break;				// exit condition
      printf("%f\n",buf);			// output result
    }
    close(fd);
  }
  return 0;
}
