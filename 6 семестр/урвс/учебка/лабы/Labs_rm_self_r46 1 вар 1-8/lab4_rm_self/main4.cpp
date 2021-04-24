#pragma region "Task description"
/*
I. Разработать программу, реализующую действия, указанные в задании к
лабораторной работе с учетом следующих требований:
+	1) все действия, относящиеся как к родительскому процессу, так и к
	порожденным процессам, выполняются в рамках одного исполняемого файла;
+	2) обмен данными между процессом-отцом и процессом-потомком
	предлагается выполнить посредством временного файла: процесс-отец после
	порождения процесса-потомка постоянно опрашивает временный файл, ожидая
	появления в нем информации от процесса-потомка;
N/A	3) если процессов-потомков несколько, и все они подготавливают
	некоторую информацию для процесса-родителя, каждый из процессов помещает в
	файл некоторую структурированную запись, при этом в этой структурированной
	записи содержатся сведения о том, какой процесс посылает запись, и сама
	подготовленная информация.
II. Модифицировать ранее разработанную программу с учетом следующих
требований:
+	1) действия процесса-потомка реализуются отдельной программой,
	запускаемой по одному из системных вызовов execl(), execv() и т.д. из процесса-
	потомка;
+	2) процесс-потомок, после порождения, должен начинать и завершать свое
	функционирование по сигналу, посылаемому процессом-предком (это же
	относится и к нескольким процессам-потомкам);
+	3) обмен данными между процессами необходимо осуществить через
	программный канал.
*/
/*
Вариант 1:
Разработать программу, вычисляющую интеграл на отрезке [A;B] от
функции exp(x) методом трапеций, разбивая интервал на K равных отрезков. Для
нахождения exp(х) программа должна породить процесс, вычисляющий её
значение путём разложения в ряд по формулам вычислительной математики.
*/
#pragma endregion

#include <stdio.h>
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include "common.h"

#pragma region "Global vars"
	#define FMT_W "%.16lf"
	#define RETURN_FAILURE -0.123456789
	#define BUFSIZE 512
	#define RD 0   // Read end of pipe
	#define WR 1   // Write end of pipe

	int pid = 0;
	char *path;
	FILE *logfile;
	bool improved_mode = false;
#pragma endregion

// this function is identical to exp(x). thanks to N. Elveyn
double exponent(double x) {
	// DONE: decomposition
	double ans = 1, power = 1;
	double abs = 1, fac = 1;
	for (int i = 1; abs > 1e-6; i++) {
		fac *= i;
		power *= x;
		abs = power / fac;
		ans += abs;
	}
	return ans;
}

void send_thru_file(double value) {
	FILE *f = fopen("/tmp/lab4.out", "w");
	fprintf(f, FMT_W"\n", value);
	fflush(f);
	fclose(f);
	return;
}

void send_thru_pipe(double value) {
	// this is done automatically via stdout redirection
}

void recv_thru_file(pid_t pid, double &x) {
	FILE *f = fopen("/tmp/lab4.out", "r");
	if(f == NULL || fscanf(f, "%lf", &x) < 1) {
		fprintf(logfile, "[!] File is empty or does not exists\n");
		x = RETURN_FAILURE;
	}
	return;
}

void recv_thru_pipe(pid_t pid, double &x) {
	ssize_t rd;
	char inbuf[BUFSIZE];
	if (((rd = read((int)pid, inbuf, BUFSIZE - 1)) <= 0) || (sscanf(inbuf, "%lf", &x) < 1))
		x = RETURN_FAILURE;
	//fprintf(stderr, "Parent: Read '%s' from child.\n", inbuf);
	return;
}

void(*send_response)(double x) = send_thru_file;
void(*recv_response)(pid_t pid, double &x) = recv_thru_file;

double exp_fork(double x) {
	// piping by http://stackoverflow.com/a/26384903/1543625
    int ctoppipe[2];	// Child-to-parent pipe
    pid_t childpid;		// Process ID of child
    int wdup;			// dup():ed stdout for child
	pipe(ctoppipe);
	// signals by http://citforum.ru/programming/unix/signals/
	sigset_t newset;
	sigemptyset(&newset);
	sigaddset(&newset, SIGCONT);
	sigprocmask(SIG_BLOCK, &newset, 0);
	switch(childpid = fork()) {
		case -1:
			// error
			printf("[!] Could not fork\n");
		break;
		case 0: {
			// to drop (child) thread from debugger: 'detach inferior <id>' to gdb window
			int pp = getpid();
			// write logfile
			char args[BUFSIZE] = "\0";
			sprintf(args, "/tmp/lab4-%d.log", pp);
			FILE *f = fopen(args, "w");
			setbuf(f, NULL);
			fprintf(f, "Child process PID = %d got x = "FMT_W"\n", pp, x);
			getcwd(args, BUFSIZE - 1);
			fprintf(f, "pwd = %s\n", args);
			double ret = 0;
			if (improved_mode) {
				close(ctoppipe[RD]);
				// wait for parent signal
				int sig;
				while(sigwait(&newset, &sig) < 0 && sig != SIGCONT);
				if(sig == SIGCONT)
					fprintf(stderr, "SIGCONT received\n");
				else
					fprintf(stderr, "Signal %d received\n", sig);

				int wdup = dup2(ctoppipe[WR], STDOUT_FILENO);
				// for debugging, wdup should be equal to 1 there
				sprintf(args, "-x"FMT_W, x);
				// thanks to Hemul1995
				pp = execl(path, path, "-i", args, 0);
				if(pp != 0) { // return code from child
					fprintf(f, "[!] Child process [-i] '%s' '%s' exited with code %d, errno %d\n", path, args, pp, errno);
				}
			}
			else {
				ret = exponent(x);
				send_response(ret);
			}
			fclose(f);

			exit(0);
		}
		break;
	}
	// parent only
	close(ctoppipe[WR]);
	double ret = 0;
	int a = 0;
	if(improved_mode) {
		//usleep(1000); // suggested by Hemul1995; overriden
		kill(childpid, SIGCONT);
	}
	int b = wait(&a); // for inline
	//int b = waitpid(pid, &a, 0); // for function
	if (a != 0) {
		printf("[!] Child process exited with code %d, halt\n", a);
		exit(3);
	}
	recv_response((pid_t)ctoppipe[RD], ret);
	fprintf(stderr, "Read '"FMT_W"' from file/stream\n", ret);
	return ret;
}

int main(int argc, char **argv) {
	// input: a, b, k
	// output: S
	// calculate exp(x) as function(x) and fork
	// here we go
	
	/*	interface:
		no args - interactive mode
		-x <value> calculates exponent(value) and exit
		-i switch sets improved mode (part II)
		-q <string> avoiding prompt of input args
		-s sets simple mode, no forks executions or pipes, just result
	*/
	path = new char[BUFSIZE];

	// open logfile
	pid = getpid();
	sprintf(path, "/tmp/lab4-%d-main.log", pid);
	logfile = fopen(path, "w");
	setbuf(logfile, NULL);

	// get itself
	memset(path, 0, BUFSIZE);
	if(readlink("/proc/self/exe", path, BUFSIZE - 1) < 0) { // could not find itself
		printf("[x] I could not find myself :(\n");
		exit(4);
	}
	fprintf(logfile, "Process PID = %d, PPID = %d started\n", pid, getppid());
	fprintf(logfile, "Command line: ");
	for(int i = 1; i < argc; i++)
		fprintf(logfile, "%s ", argv[i]);
	fprintf(logfile, "\nExecutable itself: %s\n", path);

	// mathematics here
	double a = 0, b = 1, S = 0;
	uint K = 1;
	int e = 0;
	double(*func)(double x) = &exp_fork;

	// command line parameters parse
	bool input_cli = false;
	while ((e = getopt(argc, argv, "ix:sq:")) != -1) {
		fprintf(logfile, "Got argument: %c = %s\n", e, optarg);
		switch (e) {
			case 'q': {
				if (sscanf(optarg, "%lf %lf %u", &a, &b, &K) < 3) {
					printf("Not enough input data; using interactive mode.");
				}
				else 
					input_cli = true;
			}
			break;
			case 'i': {
				send_response = &send_thru_pipe;
				recv_response = &recv_thru_pipe;
				improved_mode = true;
			}
			break;
			case 'x': { // WARNING: order-dependent
				sscanf(optarg, "%lf", &a);
				fprintf(logfile, "Embedded calc got "FMT_W"\n", a);
				fprintf(stdout, FMT_W"\n%c", exponent(a), 0); // %c is to null-terminate
				fclose(logfile);
				exit(0);
			}
			break;
			case 's': func = &exponent; break;
			case '?': fprintf(logfile, "[!] Unknown command-line argument with value: %s\n", optarg); exit(1); break;
		}
	};

	if (!input_cli) {
		fprintf(stdout, "Enter A, B, K:\n");
		fscanf(stdin, "%lf %lf %u", &a, &b, &K);
	}

	// Kotes formula: S = h ((f0 + fn) / 2 + sum(1, n-1, [fi])) [+ En(f), optional]
	if (K < 1) 
		K = 1;
	double h = (b - a) / K;
	double x_curr = a;
	for (int i = 1; i <= K - 1; i++) {
		x_curr += h;
		S += func(x_curr);
	}
	S += (func(a) + func(b)) / 2;
	S *= h;
	
	// done!
	fprintf(stdout, "Result: S ~= %f\n", S);
	return 0;
}
