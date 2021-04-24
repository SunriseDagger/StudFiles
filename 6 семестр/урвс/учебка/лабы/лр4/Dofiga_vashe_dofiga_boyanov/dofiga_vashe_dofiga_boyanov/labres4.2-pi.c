#include <signal.h>
#include <stdio.h>

// flag 'signal received'
int fSignaled;

// signal handler
void OnSignal(int signum)
{
	fputs("pi  : signal received\n", stderr);
	fSignaled = 1;
}

float Pi(void)
{
	// Pi/4 = 1 - 1/3 + 1/5 - 1/7 + ...
	float eps = 1e-10; // accuracy of evaluating Pi
	float sum = 0;
	unsigned int denom = 1;

	while (1)
	{
		float add = (1.0 / denom) - (1.0 / (denom + 2));
		if (add < eps)
			break;
		sum += add;
		denom += 4;
	}
	return sum * 4;
}

int main(void)
{
	float pi;

	fputs("pi  : started\n", stderr);

	// set signal handlers
	signal(SIGUSR1, OnSignal);
	signal(SIGUSR2, OnSignal);
	fputs("pi  : signal handlers are set\n", stderr);

	fSignaled = 0;
	// send signal 'ready' to parent
	kill(getppid(), SIGUSR1);
	fputs("pi  : signal 'started' to parent is sent\n", stderr);

	fputs("pi  : waiting for signal 'start' from parent...\n", stderr);
	// wait for signal 'start' from parent
	while (! fSignaled) pause();
	fputs("pi  : signal 'start' from parent received\n", stderr);
	fSignaled = 0;

	// calculate and write Pi value
	pi = Pi();
	write(1, &pi, sizeof(pi));
	fputs("pi  : data written\n", stderr);

	fputs("pi  : waiting for signal 'stop' from parent...\n", stderr);
	// wait for signal 'stop' from parent
	while (! fSignaled) pause();
	fputs("pi  : signal 'stop' received\n", stderr);

	exit(0);
}

