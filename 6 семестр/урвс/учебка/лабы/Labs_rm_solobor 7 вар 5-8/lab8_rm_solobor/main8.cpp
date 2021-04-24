#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept> // for exceptions
#include <fcntl.h>

#define BUFSIZE 512
#define SOCKET_QUEUE 10
#define FORK_ERROR 1 // Exit status for failed fork()
#define NOFLAGS 0
#define SYSCALL_FAILURE -1
#define SV_PORT 3030 // it's 666 in sixth basis
#define inet_atoa(in_addr_t) inet_ntop(AF_INET,(void*)&in_addr_t,buf+BUFSIZE-INET_ADDRSTRLEN-1,INET_ADDRSTRLEN)

bool server_mode = false;
FILE *logfile = stderr;

char buf[BUFSIZE];

typedef unsigned char byte; // this is missing in standard libs for whatever reason.
typedef struct { byte flags; byte code; } wait_t; // union for wait result
// process creation parameters
typedef struct {
	char *command; // executable file to be found in PATH
	char *argv0; // argv[0] string, required for exec-() functions
	char *arg1, *arg2; // first arg, second arg. no more due to complexity
	int in, out, err; // descriptors of standard streams
} param_t;

// Connect socket to remote address using its IP and port.
int connect_to(int sockfd, in_addr_t ip_addr, ushort port) {
	sockaddr_in s_addr; // структура с информацией о сервере
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(port);
	s_addr.sin_addr.s_addr = ip_addr;
	return connect(sockfd, (sockaddr*)&s_addr, sizeof(s_addr));
}

// Bind socket locally to become a server.
int bind_to(int sockfd, ushort port, in_addr_t addr = INADDR_ANY) {
	sockaddr_in s_addr; // структура с информацией о сервере
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = addr; // 0.0.0.0 by default
	s_addr.sin_port = htons(port);
	return bind(sockfd, (sockaddr*)&s_addr, sizeof(s_addr));
}

// Read single row from server list and return ready-to-use IP.
bool get_server(char *str, in_addr_t &ip) {
	char addr[BUFSIZE];
	in_addr ip_addr;
	if (sscanf(str, "%s", addr) < 1) 
		return false;
	if (!inet_aton(addr, &ip_addr)) // функция перевода ip-адреса в нужный тип
		return false;
	ip = ip_addr.s_addr; // take only IP value
	return true;
}

// Fill param_t with process creation data.
void create_startup_param(
	param_t *param,
	char *command,
	char *arg1,
	char *arg2 = NULL, 
	int out = STDOUT_FILENO,
	int err = STDERR_FILENO,
	int in = STDIN_FILENO) {
	param->command = command;
	param->argv0 = command;
	param->arg1 = arg1;
	param->arg2 = arg2;
	
	param->in = in;
	param->out = out;
	param->err = err;
}

// Overlays current executing process with COMMAND.
int shell_exec(param_t *param) {
	dup2(param->out, STDOUT_FILENO); // this is identical to fcntl(param->out, F_DUPFD, 1)
	dup2(param->err, STDERR_FILENO);
	dup2(param->in, STDIN_FILENO);
	int ret = execlp(param->command, param->argv0, param->arg1, param->arg2, NULL);
	return ((ret == SYSCALL_FAILURE) ? -1 : 0); // return -1 on failure, 0 otherwise
}

// Create new thread in new process via fork() and run callback in it.
pid_t fork_thread(int(func(param_t *param)), void *param = NULL) {
	int childpid;
	switch (childpid = fork()) {
	case SYSCALL_FAILURE:
		fprintf(stderr, "[shell]\t Could not fork to subcommand '%s'\n", ((param_t*)param)->command);
		exit(FORK_ERROR);
		break;
	case 0:
		exit(func((param_t*)param)); // run callback and exit
		break;
	default:
		fprintf(stderr, "[shell]\t Process %d created for '%s'\n", childpid, ((param_t*)param)->command);
	}
	return childpid;
}

// Wait till ALL running subcommands end
int join_threads() {
	pid_t wait_pid;
	wait_t wait_ret;
	int exec_result = 0;
	while ((wait_pid = wait(&wait_ret)) > 0) { 
		printf("[shell]\t Process %d exited with code %d\n", wait_pid, wait_ret.code);
		exec_result |= wait_ret.code; // OR all codes
	}
	return exec_result;
}

int perform_client(char *server_ip) {
	/*
	1. create socket
	2. connect to server
	*/
	int sockfd;
	in_addr_t ip;
	if (!get_server(server_ip, ip)) {
		printf("[x] Invalid server IP\n");
		return EXIT_FAILURE;
	}
	
	try {
		if ((sockfd = socket(PF_INET, SOCK_STREAM, SOL_IP)) == -1) // получаем дескриптор сокета
			throw std::runtime_error("Could not create socket");
		
		if (connect_to(sockfd, ip, SV_PORT) < 0) { // соединяемся с сервером
			sprintf(buf, "Could not connect to server '%s:%d'", inet_atoa(ip), SV_PORT);
			throw std::runtime_error(buf);
		}
		
		printf("Enter command: ");
		fgets(buf, BUFSIZE - 1, stdin); // формируем сообщение
		buf[strlen(buf) - 1] = '\0';
		if (write(sockfd, buf, BUFSIZE - 1) == SYSCALL_FAILURE) // отправляем сообщение серверу
			throw std::runtime_error("Server unexpectedly dead");
		//read(sockfd, &msg, sizeof(msg)); // получаем сообщение сервера; blocking mode
		ssize_t cnt, cnt2;// = recv(sockfd, &buf, BUFSIZE - 1, MSG_PEEK);
		//dup2(sockfd, STDOUT_FILENO);
		do {
			//cnt = sendfile(sockfd, STDOUT_FILENO, NULL, BUFSIZE);
			cnt = recv(sockfd, buf, BUFSIZE - 1, NOFLAGS);
			cnt2 = write(STDOUT_FILENO, buf, cnt);
		} while (cnt > 0 && cnt2 > 0);
	}
	catch (std::runtime_error e) {
		fprintf(logfile, "[!] %s\n", e.what());
	}
	close(sockfd); // finally
	return EXIT_SUCCESS;
}

int perform_server() {
	/*
	1. create socket
	2. bind locally
	3. listen
	4. after each accept do actions
	*/
	int sockfd;
	try {
		if ((sockfd = socket(PF_INET, SOCK_STREAM, SOL_IP)) < 0) // получение дескриптора сокета сервера
			throw std::runtime_error("Cannot create socket");
		if (bind_to(sockfd, SV_PORT) < 0) // связываем сокет с ip-адресом и портом
			throw std::runtime_error("Error in bind");
		if (listen(sockfd, SOCKET_QUEUE) < 0) // открываем доступ для клиентов
			throw std::runtime_error("Could not set listen mode");
		fprintf(logfile, "[i] Server started on port %d\n", SV_PORT);
	}
	catch (std::runtime_error e) {
		fprintf(logfile, "[x] Server: %s\n", e.what());
		close(sockfd);
		return EXIT_FAILURE;
	}
	// ждем подключений клиентов, устанавливаем соединение
	int cl_sockfd;
	while ((cl_sockfd = accept(sockfd, NULL, NULL)) > 0) { // TODO: move work to thread
		// TODO: add loop
		read(cl_sockfd, buf, BUFSIZE - 1); // получаем сообщение клиента
		if (strncmp(buf, "exit", 4) == 0) {
			close(cl_sockfd);
			fprintf(logfile, "[i] Server was stopped by user command\n");
			break;
		}
		param_t param;
		create_startup_param(&param, buf, NULL, NULL, cl_sockfd); // WARNING: duplicate dup2()
		fork_thread(shell_exec, &param);
		join_threads();
		close(cl_sockfd); // закрываем дескриптор на сокет клиента
	}
	close(sockfd); // закрываем дескриптор на сокет сервера
	return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
	char *host = NULL;
	int e;
	while ((e = getopt(argc, argv, ":c:d")) != SYSCALL_FAILURE) {
		//fprintf(logfile, "Got argument: %c = %s\n", e, optarg);
		switch (e) {
		case 'c': host = optarg; break;
		case 'd': server_mode = true; break;
		case '?': fprintf(logfile, "[!] Unknown command-line argument with value: %s\n", optarg); exit(1); break;
		}
	};
	
	if (server_mode)
		return perform_server();
	if(host != NULL)
		return perform_client(host);
	return printf("Usage:\n-c <hostname>\t - connect to <hostname>\n"
	              "-d\t\t - run as server, default port is %d\n", SV_PORT);
}
