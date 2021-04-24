#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#define PI 3.1415926

int sw=0;				// flag for start process
int ch[2];				// channel

void startwrite(int sig) { sw++; }	// call for start process
void quit(int sig) {			// call for exit from process
  close(ch[1]);
  exit(sig);
}


int main(int argc, char * argv[]) {	// ****** main program ******
  signal(SIGUSR1,startwrite);		// define reaction on signals
  signal(SIGUSR2,quit);
  
  kill(getppid(),SIGUSR1);		// say parent that ready
  
  while (!sw);				// wait signal
  sscanf(argv[0],"%d",&ch[0]);		// get channel descriptors from argv
  sscanf(argv[1],"%d",&ch[1]);
  close(ch[0]);				// close in
  int N, M;
  printf("Input N = ");			// input  N & M
  scanf("%d",&N);
  printf("Number of steps = ");
  scanf("%d",&M);
  int i, k;
  float x, sum, expr;
  for (i=0; i<=M; i++) {
    x=i*PI/M;
    expr=x;
    for (k=0; k<=N; k++) { sum=expr/(2*k+1); expr*=x*x; }	// calc sum
    write(ch[1],&sum,sizeof(float));		// write
  }
  sum=-1;
  write(ch[1],&sum,sizeof(float));	// output end flag
  close(ch[1]);
  return 0;
}
