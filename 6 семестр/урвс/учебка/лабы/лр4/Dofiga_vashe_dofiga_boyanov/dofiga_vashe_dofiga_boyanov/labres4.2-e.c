#include <signal.h>
#include <stdio.h>

// flag - signal received
int fSignaled;

// signal handler
void OnSignal(int signum)
{
	fputs("e   : signal received\n", stderr);
	fSignaled = 1;
}

float E(void)
{
	// E = 1 + 1/1! + 1/2! + 1/3! + ...
	float eps = 1e-7; // accuracing of evaluating E
	float sum = 1; // sum of series
	unsigned int number = 1; // number of member (excluding first)
	float member = 1;

	while (member > eps)
	{
		sum += member;
		member /= ++number;
	}
	return sum;
}

int main(void)
{
	float e;

	fputs("e   : started\n", stderr);

	// set signal handlers
	signal(SIGUSR1, OnSignal);
	signal(SIGUSR2, OnSignal);
	fputs("e   : signal handlers are set\n", stderr);

	fSignaled = 0;
	// send signal 'ready' to parent
	kill(getppid(), SIGUSR2);
	fputs("e   : signal 'started' to parent is sent\n", stderr);

	fputs("e   : waiting for signal 'start' from parent...\n", stderr);
	// wait for signal 'start' from parent
	while (! fSignaled) pause();
	fputs("e   : signal 'start' from parent received\n", stderr);
	fSignaled = 0;

	// calculate and write E value
	e = E();
	write(1, &e, sizeof(e));
	fputs("e   : data written\n", stderr);

	fputs("e   : waiting for signal 'stop' from parent...\n", stderr);
	// wait for signal 'stop' from parent
	while (! fSignaled) pause();
	fputs("e   : signal 'stop' received\n", stderr);

	exit(0);
}
