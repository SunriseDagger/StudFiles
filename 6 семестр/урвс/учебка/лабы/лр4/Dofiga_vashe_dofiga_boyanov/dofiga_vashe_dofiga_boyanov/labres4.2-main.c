#include <signal.h>
#include <stdio.h>

// flags indicating 'ready' state of children
int fPiRunning = 0, fERunning = 0;

// signal handler - processes signal 'pi ready'
void OnPiRunning(int signum)
{
	fputs("main: signal from 'pi'\n", stderr);
	fPiRunning = 1;
}

// signal handler - processes signal 'e ready'
void OnERunning(int signum)
{
	fputs("main: signal from 'e'\n", stderr);
	fERunning = 1;
}

int main(void)
{
  int pipePi[2], pipeE[2]; // pipes for communicating with children
  int pidPi, pidE; // pids of children
  float pi, e; // values for calculating

  // create pipes
  if (pipe(pipePi) == -1 || pipe(pipeE) == -1)
  {
    perror("Error creating pipes");
    exit(1);
  }
  
	fputs("main: pipes created\n", stderr);

	// set signal handlers to process signals from children
	signal(SIGUSR1, OnPiRunning);
	signal(SIGUSR2, OnERunning);
	fputs("main: signal handlers are set\n", stderr);

  // fork process evaluating Pi
  switch (pidPi = fork())
	{
	case -1:
		perror("Error forking 'pi' child");
		exit(1);

	case 0:
		// replace standart output with pipe
		dup2(pipePi[1], 1);
		// close unneeded files
		close(pipePi[0]); close(pipePi[1]); close(pipeE[0]); close(pipeE[1]);
		// replace process with new executable
		execl("labres4.2-pi", 0);
		perror("Error running 'pi' process");
		exit(1);
	}
	fputs("main: 'pi' process created\n", stderr);

	// fork process evaluating E
	switch (pidE = fork())
	{
	case -1:
		perror("Error forking 'e' child");
		exit(1);

	case 0:
		// replace standart output with pipe
		dup2(pipeE[1], 1);
		// close all opened pipes
		close(pipeE[0]); close(pipeE[1]); close(pipePi[0]); close(pipePi[1]);
		execl("labres4.2-e", 0);
		perror("Error running 'e' process");
		exit(1);
	}
	fputs("main: 'e'  process created\n", stderr);

	fputs("main: waiting for signals from children...\n", stderr);
	// wait for signals 'ready' from children
	while (! fPiRunning || ! fERunning)
		pause();
	fputs("main: signals received\n", stderr);

	// send signals 'start' to children
	kill(pidPi, SIGUSR1);
	kill(pidE , SIGUSR1);
	fputs("main: signals 'start' to children are sent\n", stderr);

	pi = e = 0;
	// we only need to read from pipes, so close descriptors for writing
	close(pipePi[1]); close(pipeE[1]);
	// read values of Pi and E from pipes
	if (read(pipePi[0], &pi, sizeof(pi)) == -1 || read(pipeE[0], &e, sizeof(e)) == -1)
	{
		perror("Error reading from pipes");
		exit(1);
	}
	fputs("main: values are readed\n", stderr);

	// send signals 'stop' to children
	kill(pidPi, SIGUSR2);
	kill(pidE , SIGUSR2);
	fputs("main: signals 'stop' to children are sent\n", stderr);

	// print results
	printf("pi=%f, e=%f, f=%f\n", pi, e, 1 / (pi * (1 + e * e)));

	fputs("main: waiting for children to terminate...\n", stderr);
	// wait until children terminated
	waitpid(pidPi, 0, 0);
	waitpid(pidE , 0, 0);
	fputs("main: children terminated, exit...\n", stderr);
}

