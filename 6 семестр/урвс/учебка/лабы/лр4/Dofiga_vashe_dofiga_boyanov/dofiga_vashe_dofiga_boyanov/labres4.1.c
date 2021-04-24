#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

float EvaluatePi(void)
{
	// Pi/4 = 1 - 1/3 + 1/5 - 1/7 + ...
	const float eps = 1e-10; // accuracy of evaluating Pi
	float sum = 0;
	unsigned int denom = 1; // denominator of the member
	while (1)
	{
		float add = (1.0 / denom) - (1.0 / (denom + 2));
		if (add < eps)
			break;
		sum += add;
		denom += 4;
	}

	return 4 * sum;
}

float EvaluateE(void)
{
	// E = 1 + 1/1! + 1/2! + 1/3! + ...
	const float eps = 1e-7; // accuracing of evaluating E
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

// record for writing to temporary file
struct FileRecord
{
	pid_t pid; // pid of the process which writes record
	float result; // data (pi of e)
};

int main(void)
{
	pid_t pidPi; // identificator of process evaluating Pi
	pid_t pidE;  //      -//-                   -//-    E

	int fdWriteTemp, fdReadTemp; // descriptors of temporary file
	struct FileRecord record; // data record in file

	float pi, e, f;
	char fPiReaded, fEReaded; // flags indicating that we read values
	int cReaded; // counter of bytes readed from file

	// template for name of temporary file
	char filename[] = "labres4.1tempXXXXXX";
	// get unique name from template - security risk!
	if (! mktemp(filename))
	{
		perror("Error getting unique file name\n");
		exit(1);
	}
	// create and open temporary file for writing
	if ((fdWriteTemp = creat(filename, 0600)) == -1)
	{
		perror("Error opening temporary file for writing\n");
		exit(1);
	}
	// open the same file for reading
	if ((fdReadTemp = open(filename, O_RDONLY)) == -1)
	{
		perror("Error opening temporary file for reading\n");
		exit(1);
	}
	// unlink file - it's name will be deleted from the directory,
	// but it will be exists while in using
	if (unlink(filename) == -1)
	{
		// on error we report, but continue execution
		perror("Error unlinking temporary file\n");
	}

	// try to create child process for evaluating Pi
	switch (pidPi = fork())
	{
	case -1: // error occuring, no child created
		perror("Can't create child process evaluating Pi\n");
		exit(1);

	case 0: // ok, this is a child process
		// we need only write to file, so close descriptor for reading
		close(fdReadTemp);
		// evaluate Pi
		record.pid = getpid();
		record.result = EvaluatePi();
		// send it to main process
		if (write(fdWriteTemp, &record, sizeof(record)) == -1)
		{
			close(fdWriteTemp);
			perror("Pi: error writing to file\n");
			exit(1);
		}
		close(fdWriteTemp);
		exit(0);
	}

	// try to create child process for evaluating E
	switch (pidE = fork())
	{
	case -1: // error occuring, no child created
		perror("Can't create child process evaluating E\n");
		exit(1);

	case 0: // ok, this is a child process
		// we need only write to file, so close descriptor for reading
		close(fdReadTemp);
		// evaluate E
		record.pid = getpid();
		record.result = EvaluateE();
		// send it to main process
		if (write(fdWriteTemp, &record, sizeof(record)) == -1)
		{
			close(fdWriteTemp);
			perror("E: error writing to file\n");
			exit(1);
		}
		close(fdWriteTemp);
		exit(0);
	}

	// here is the main process

	// we needn't write to file, close descriptor
	close(fdWriteTemp);

	// try to read from file until we get two needed values
	fPiReaded = fEReaded = 0;
	while (! (fPiReaded && fEReaded))
	{
		// try to read record from file
		cReaded = read(fdReadTemp, &record, sizeof(record));
		if (cReaded == -1)
		{
			// nothing readed, error
			perror("Error reading from file\n");
			exit(1);
		}
		if (cReaded == sizeof(record))
		{
			// ok, we get record, decide what is it
			if (record.pid == pidPi)
			{
				// check that pi value is not duplicated
				if (fPiReaded)
				{
					fputs("dublicating Pi value\n", stderr);
					exit(1);
				}
				pi = record.result;
				fPiReaded = 1;
			}
			else if (record.pid == pidE)
			{
				// check that e value is not duplicated
				if (fEReaded)
				{
					fputs("dublicating E value\n", stderr);
					exit(1);
				}
				e = record.result;
				fEReaded = 1;
			}
			else
			{
				// unknown pid in file
				fputs("unknown pid in file\n", stderr);
				exit(1);
			}
		}
		else if (cReaded != 0)
		{
			// we read something, but not right size
			fputs("invalid record in file\n", stderr);
		}

		// we readed 0 bytes - no info in file, try again
	}
	close(fdReadTemp);

	printf("Pi=%f\n", pi);
	printf("E=%f\n", e);
	// evaluate function value
	f = 1 / (pi * (1 + e * e));
	printf("F=%f\n", f);
	exit(0);
}

