#include <sys/types.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

void print_error(){
    char error_text[100];
    perror(error_text);
    printf("Error: %s\n", error_text);
}


int main(int argc, char** argv){
    if(argc != 2){
        printf("Can be only one argument\n");
        exit(1);
    }

    printf("\t!!!! %s\n", argv[0]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0){
        printf("Can't create socket\n");
        print_error();
        exit(1);
    }

    printf("Socket created\n");

    struct sockaddr_in info;
    info.sin_family = AF_INET;
    info.sin_port = htons(2222);
    info.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    printf("Connection...\n");

    int check_connect = connect(sock, (struct sockaddr*) &info, sizeof(info));

    if(check_connect < 0){
        printf("Connect error\n");
        print_error();
        close(sock);
        exit(1);
    }
    else{
        printf("Connection done\n");
    }

    strcat(argv[1], "\0");
    printf("Sending...\n");
    printf("Sending:\t%s\n", argv[1]);
    send(sock, argv[1], strlen(argv[1])+1, 0);
    printf("Send done\n");
    if(strcmp(argv[1], "end") != 0){
        char recv_ip[100];
        recv(sock, recv_ip, sizeof(recv_ip), 0);
        printf("IP:\t%s\n", recv_ip);
    }

    close(sock);
    return 0;
}
