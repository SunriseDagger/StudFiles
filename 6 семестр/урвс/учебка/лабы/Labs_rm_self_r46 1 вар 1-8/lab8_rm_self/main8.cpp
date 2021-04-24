#pragma region Task description
/*
Эмуляция DNS сервера. Клиент подсоединяется к серверу, IP которого
хранится в файле dns.url, и делает ему запрос на подключение к серверу "Имя
сервера". DNS-сервер имеет список, хранящийся в файле о соответствии имен
серверов и IP-адресов. Если в списке нет "имени сервера", запрошенного
клиентом, то сервер DNS подключается последовательно к другим серверам,
хранящимся в файле dns.url, и т.д. Если сервер не найден, клиенту возвращается
соответствующее сообщение.
*/
#pragma endregion

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

#pragma region Global vars
#define BUFSIZE 256
#define SOCKET_QUEUE 10
#define SV_PORT 2000
const char *_server_file = "dns.url";
const char *_resolv_file = "hosts";
char buf[BUFSIZE];
#define inet_atoa(in_addr_t) inet_ntop(AF_INET,(void*)&in_addr_t,buf+BUFSIZE-INET_ADDRSTRLEN-1,INET_ADDRSTRLEN)
FILE *logfile = stderr;
bool server_mode = false;
#pragma endregion

typedef enum { MT_DATA, MT_ERROR } msg_t; // Типы сообщений
typedef struct { // client-server message template
	msg_t type;
	char str[BUFSIZE];
} data_t;

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

// Read single row from server list and return ready-to-use IP and port.
bool get_server(FILE *f, in_addr_t &ip, ushort &port) {
	if (f == NULL) return false; // если нет файла
	char addr[BUFSIZE];
	if (fscanf(f, "%s %hd", addr, &port) < 2) // %hd means short integer
		return false;
	in_addr ip_addr;
	inet_aton(addr, &ip_addr); // функция перевода ip-адреса в нужный тип
	ip = ip_addr.s_addr; // take only IP value
	return true;
}

// Lookup for host_s in local resolv file.
bool find_ip(const char *host_s, char *addr_s, const char *filename = _resolv_file) {
	FILE *f = fopen(filename, "r");
	if (f == NULL) { // если нет файла
		fprintf(logfile, "[!] Cannot open resolv file\n");
		return false;
	}

	char hostname[BUFSIZE], addr[BUFSIZE];
	while (fscanf(f, "%s %s", hostname, addr) == 2) { // пока не конец файла
		if (strcmp(host_s, hostname) == 0) { // hostnames match
			strncpy(addr_s, addr, BUFSIZE - 1);
			return true;
		}
	}
	fprintf(logfile, "[i] IP address for hostname '%s' has not found locally\n", host_s);
	fclose(f);
	return false;
}

// Query all known `DNS` servers to search for hostname in their databases.
bool query_external_dns(const char *hostname, char *addr, const char *server_file = _server_file) {
	FILE *f = fopen(server_file, "r");
	if (f == NULL) {
		fprintf(logfile, "[!] Server file '%s' couldn't be opened.\n", server_file);
		return false;
	}
	in_addr_t dns_ip;
	u_short dns_port;
	int sockfd;

	while (get_server(f, dns_ip, dns_port)) { // получаем ip-адрес вышестоящего сервера
		try {
			if ((sockfd = socket(PF_INET, SOCK_STREAM, SOL_IP)) == -1) // получаем дескриптор сокета
				throw std::runtime_error("Could not create socket");
		
			if (connect_to(sockfd, dns_ip, dns_port) < 0) { // соединяемся с сервером
				sprintf(buf, "Could not connect to server '%s:%d'", inet_atoa(dns_ip), dns_port);
				throw std::runtime_error(buf);
			}
			
			data_t msg; 
			msg.type = MT_DATA;
			strcpy(msg.str, hostname); // формируем сообщение
			write(sockfd, &msg, sizeof(msg)); // отправляем сообщение серверу
			read(sockfd, &msg, sizeof(msg)); // получаем сообщение сервера; blocking mode

			if (msg.type == MT_DATA) {
				strncpy(addr, msg.str, BUFSIZE - 1);
				return true;
			}
		}
		catch (std::runtime_error e) {
			fprintf(logfile, "[!] %s\n", e.what());
		}
		close(sockfd); // finally
	}
	return false;
}

// Perform DNS server behaviour, which may be as authoritative as forwarder.
int perform_server(ushort port = SV_PORT, const char *resolv_file = _resolv_file, const char *dns_file = _server_file) {
	data_t msg;
	int sockfd;  
	
	try {
		if ((sockfd = socket(PF_INET, SOCK_STREAM, SOL_IP)) < 0) // получение дескриптора сокета сервера
			throw std::runtime_error("Cannot create socket");
		if (bind_to(sockfd, port) < 0) // связываем сокет с ip-адресом и портом
			throw std::runtime_error("Error in bind");
		if (listen(sockfd, SOCKET_QUEUE) < 0) // открываем доступ для клиентов
			throw std::runtime_error("Could not set listen mode");
		fprintf(logfile, "[i] Server started on port %d\n", port);
	}
	catch (std::runtime_error e) {
		fprintf(logfile, "[x] Server: %s\n", e.what());
		close(sockfd);
		return EXIT_FAILURE;
	}
	// ждем подключений клиентов, устанавливаем соединение
	int cl_sockfd;
	while ((cl_sockfd = accept(sockfd, NULL, NULL)) > 0) { 
		read(cl_sockfd, &msg, sizeof(msg)); // получаем сообщение клиента
		// msg.str = hostname
		//char srv_ip[BUFSIZE];
		msg.type = MT_DATA;
		if (find_ip(msg.str, msg.str, resolv_file)) { // если нашли заданное имя в таблице
		}
		else if (query_external_dns(msg.str, msg.str, dns_file)) { // если нашли заданное имя на другом dns-сервере
		}
		else { // если не нашли
			msg.type = MT_ERROR;
			memset(msg.str, '\0', BUFSIZE);
		}
		write(cl_sockfd, &msg, sizeof(msg)); // посылаем сообщение клиенту
		close(cl_sockfd); // закрываем дескриптор на сокет клиента
	}
	close(sockfd); // закрываем дескриптор на сокет сервера
	return EXIT_SUCCESS;
}

// Perform DNS client behaviour.
int perform_client(const char *hostname = "ultibot.ru", const char *server_file = _server_file) {
	char addr[BUFSIZE];
	if (query_external_dns(hostname, addr, server_file)) {
		printf("Server '%s' has ip '%s'\n", hostname, addr);
		return EXIT_SUCCESS;
	}
	else {
		printf("No records for server name '%s'\n", hostname);
		return EXIT_FAILURE;
	}
}

// Main entry point of application.
int main(int argc, char **argv) {
	char *host = NULL;
	ushort port;
	int e;
	while ((e = getopt(argc, argv, ":c:d:")) != -1) {
		//fprintf(logfile, "Got argument: %c = %s\n", e, optarg);
		switch (e) {
			case 'c': host = optarg; break;
			case 'd': server_mode = true; host = optarg; break;
			// read http://stackoverflow.com/a/25218265/1543625 about optional args
			case '?': fprintf(logfile, "[!] Unknown command-line argument with value: %s\n", optarg); exit(1); break;
		}
	};

	if (server_mode) {
		// we can open logfile, but stderr is better for presentation
		//sprintf(buf, "/tmp/lab8.log");
		//logfile = fopen(buf, "w");
		setbuf(logfile, NULL);
		if (host != NULL && sscanf(host, "%hd", &port) == 1)
			return perform_server(port);
		else // this 'else' left for readability
			return perform_server();
	}
	if (host != NULL)
		return perform_client(host);
	else
		return printf("Usage:\n-c <hostname>\t - resolve <hostname> using %s file\n"
		              "-d [<port>]\t - run as server on port <port>, default is %d\n", _server_file, SV_PORT);
}
