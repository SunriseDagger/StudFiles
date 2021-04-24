#include <sys/socket.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

void print_error() {
    char error_text[100];
    perror(error_text);
    printf("Error: %s\n", error_text);
}

int main(int argv, char** argc){

    if(argv != 2){
        printf("Must be one argument - nickname\n");
        exit(1);
    }

    key_t mem_key = ftok("client.c", 0XAB29 + time(0));
    int cl_socket = socket(AF_INET, SOCK_STREAM, 0);

    int mem_id = shmget(mem_key, sizeof(int), IPC_CREAT | 0777);

    if(mem_id < 0){
        if(errno == EEXIST){
            printf("memory exist, connecting\n");
            mem_id = shmget(mem_key, sizeof(int), 0);
            }
        else{
            printf("Can't get memory\n");
            print_error();
            exit(1);
        }
    }

    if(cl_socket < 0){
        printf("Can't get socket\n");
        print_error();
        exit(1);
    }

    int* cl_work = (int*)shmat(mem_id, NULL, SHM_RND);
    *cl_work = 1;

    struct sockaddr_in info;
    info.sin_family = AF_INET;
    info.sin_port = htons(2255);
    info.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int check_connect = connect(cl_socket, (struct sockaddr_in*)&info, sizeof(info));
    if(check_connect < 0){
        printf("Can't connect\n");
        print_error();
        exit(1);
    }

    send(cl_socket, argc[1], strlen(argc[1])+1, 0);

    int fork_pid = fork();

    if(fork_pid == 0){
        while(*cl_work){
            char msg_from_serv[1124];
            int recv_bytes = 0;
            while(recv_bytes <= 0)
                recv_bytes = recv(cl_socket, msg_from_serv, sizeof(msg_from_serv), 0);

            if(strcmp(msg_from_serv, "!end!") != 0)
                printf("%s", msg_from_serv);
            else{
                *cl_work = 0;
                printf("Server was shooting down! Please press Enter.\n");
                break;
            }
        }
        return 0;
    }
    else{
        if(fork_pid == -1){
            printf("Can't fork\n");
            print_error();
            exit(2);
        }
    }

    while(*cl_work){
        char sending_msg[1024];
        gets(sending_msg);
        send(cl_socket, sending_msg, strlen(sending_msg)+1, 0);
        if(strcmp(sending_msg, "bay") == 0 || strcmp(sending_msg, "!end!") == 0){
            *cl_work = 0;
        }
    }

    close(cl_socket);
    shmdt(cl_work);

    kill(fork_pid);
    return 0;
}
