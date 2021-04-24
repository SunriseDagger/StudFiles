#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>

int main() {

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in info;
    info.sin_family = AF_INET;
    info.sin_port = htons(2256);
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    int check = connect(sock, (struct sockaddr*)&info, sizeof(info));
    if(check < 0){
        printf("Connecting failng\n");
        exit(1);
    }

    int fork_pid = fork();
    if(fork_pid == 0) {
        int work  = 1;
        while(work) {
            char mes[100];
            int check_r = recv(sock, mes, sizeof(mes), 0);
            if(check_r < 0){
                printf("Something wring with server. Shooting down\n");
                close(sock);
                work = 0;
            }
            else
                printf("%s\n", mes);

        }

        return 0;
    }
    else {
        if(fork_pid < 0) {
            printf("Can't create fork\n");
            close(sock);
            exit(1);
        }
    }

    char tmp[100];
    getchar();
    send(sock, tmp, strlen(tmp)+1, 0);

    close(sock);
    kill(fork_pid, SIGTERM);

    return 0;
}
