#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void nothing(int sig) {
}

int main() {
  int ch[2];				// program channel
  pipe(ch);
  char arg1[7], arg2[7];		// args for child
  sprintf(arg1,"%d",ch[0]);		// write descriptors to args
  sprintf(arg2,"%d",ch[1]);
  signal(SIGUSR1,nothing);
  pid_t child_pid=fork();		// get new process
  if (child_pid==0) execl("./child",arg1,arg2,0);	// call child program
  pause();				// bacause parent can kill child
  close(ch[1]);				// close out
  kill(child_pid,SIGUSR1);		// start child
  float buf;
  while (1) {
    while (read(ch[0],&buf,sizeof(float))==0);	// while can read from channel
    if (buf<0) break;			// end flag
    printf("%f\n",buf);			// print to screen
  }
  close(ch[0]);				// close in
  kill(child_pid,SIGUSR2);		// kill child
  return 0;
}
