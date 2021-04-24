#include <sys/types.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define fork_n 3

struct client_msg {
    long mtype;
    char name[100];
    char text[1024];
    int fork_source;
};

void print_error() {
    char error_text[100];
    perror(error_text);
    printf("Error: %s\n", error_text);
}

void make_msg(struct client_msg* inp_m, char* out_m) {
    strcpy(out_m, inp_m->name);
    strcat(out_m, ":\t");
    strcat(out_m, inp_m->text);
    strcat(out_m, "\n");

}

int main() {

    //Keys
    key_t msg_key = 0X203;
    key_t mem_key = 0X205;

    key_t fork_keys[fork_n];
    int key_iter;

    for(key_iter = 0; key_iter < fork_n; key_iter++) {
        fork_keys[key_iter] = ftok("server.c", key_iter+1);
    }

    int main_socket = socket(AF_INET, SOCK_STREAM, 0);
    int msg_to_main = msgget(msg_key, IPC_CREAT | 0777);
    int mem_for_stop = shmget(mem_key, sizeof(int),IPC_CREAT | 0777);
    int msg_to_forks[fork_n];

    if(main_socket < 0) {
        printf("Can't create socket\n");
        print_error();
        exit(1);
    }

    if(msg_to_main < 0) {
        if(errno == EEXIST) {
            printf("Main queue exists, connecting\n");
            msg_to_main = msgget(msg_key, 0);
        } else {
            printf("Can't create main queue\n");
            print_error();
            exit(1);
        }
    }

    if(mem_for_stop < 0) {
        if(errno == EEXIST) {
            printf("Memory exist, connecting");
            mem_for_stop = shmget(mem_key, sizeof(int), 0);
        } else {
            printf("Can't create shared memory\n");
            print_error();
            exit(1);
        }
    }

    int queue_iter;
    for(queue_iter = 0; queue_iter < fork_n; queue_iter++) {
        msg_to_forks[queue_iter] = msgget(fork_keys[queue_iter], IPC_CREAT | 0777);
        if(msg_to_forks < 0) {
            if(errno == EEXIST) {
                printf("Queue for fork %d exist, connecting\n", queue_iter);
                msg_to_forks[queue_iter] = msgget(fork_keys[queue_iter], 0);
            } else {
                printf("Can't create queue for fork %d", queue_iter);
                print_error();
                exit(1);
            }
        }
    }

    struct sockaddr_in info;
    info.sin_family = AF_INET;
    info.sin_port = htons(2255);
    info.sin_addr.s_addr = htonl(INADDR_ANY);

    int check_bind = bind(main_socket, (struct sockaddr*)&info, sizeof(info));
    if(check_bind < 0) {
        printf("Bind error\n");
        print_error();
        exit(1);
    }

    int* serv_work = (int*)shmat(mem_for_stop, NULL, SHM_RND);
    *serv_work = 1;

    int fork_pids[fork_n];
    int fork_i;

    for(fork_i = 0; fork_i < fork_n; fork_i++) {

        fork_pids[fork_i] = fork();
        if(fork_pids[fork_i] == 0) {

            int fork_to_cl;
            int client_sock;

            while(*serv_work) {

                int check_listen = listen(main_socket, 3);
                if(check_listen < 0) {
                    printf("Listen error\n");
                    print_error();
                    *serv_work = 0;
                    exit(1);
                }
                printf("Fork %d listen\n", fork_i);

                client_sock = accept(main_socket , NULL, NULL);
                char client_name[100];
                recv(client_sock, client_name, sizeof(client_name), 0);

                printf("User %s connected to %d\n", client_name, fork_i);

                struct client_msg msg_hello;
                msg_hello.fork_source = fork_i;
                msg_hello.mtype = 1;
                strcpy(msg_hello.name, client_name);
                strcpy(msg_hello.text, "in chat!");

                msgsnd(msg_to_main, &msg_hello, sizeof(msg_hello), 0);

                fork_to_cl = fork();

                if(fork_to_cl == 0) {
                    while(*serv_work){
                        struct client_msg msg_to_client;
                        char msg_txt[1124];
                        int read_bytes = 0;
                        printf("Fork fork %d waiting msg from others\n", fork_i);

                         while(read_bytes <= 0 && *serv_work)
                            read_bytes = msgrcv(msg_to_forks[fork_i], &msg_to_client, sizeof(msg_to_client), 1, 0);

                        int end_var = strcmp(msg_to_client.text, "!end!");
                        if(end_var != 0)
                            make_msg(&msg_to_client, msg_txt);
                        else{
                            strcpy(msg_txt,"!end!");
                            printf("Fork %d send %s to %s\n", fork_i, msg_txt, client_name);
                        }
                        send(client_sock, msg_txt, strlen(msg_txt)+1, 0);
                        if(end_var == 0)
                            break;
                    }

                    printf("Fork fork %d end\n", fork_i);
                    return 0;
                }
                else {
                    if(fork_to_cl < 0) {
                        printf("Can't fork for send in fork %d\n", fork_i);
                        print_error();
                        *serv_work = 0;
                        exit(2);
                    }
                }
                int client_active = 1;

                while(client_active && *serv_work){
                    char client_msg_txt[1024];
                    printf("Fork %d waiting msg from client\n", fork_i);
                    recv(client_sock, client_msg_txt, sizeof(client_msg_txt), 0);


                    struct client_msg msg_to_serv;
                    msg_to_serv.mtype = 1;
                    strcpy(msg_to_serv.name, client_name);
                    strcpy(msg_to_serv.text, client_msg_txt);
                    msg_to_serv.fork_source = fork_i;
                    msgsnd(msg_to_main, &msg_to_serv, sizeof(msg_to_serv),0);

                     if(strcmp(client_msg_txt, "bay") == 0){
                        client_active = 0;
                        close(client_sock);
                        kill(fork_to_cl, SIGKILL);
                    }

                }

            }
            close(client_sock);
            kill(fork_to_cl, SIGTERM);
            printf("Fork %d end\n", fork_i);
            return 0;

        } else {
            if(fork_pids[fork_i] < 0) {
                printf("Can't fork %d\n", fork_i);
                print_error();
                *serv_work = 0;
                exit(2);
            }
        }
    }

    while(*serv_work){
        struct client_msg msg_from_fork;
        int recv_bytes = 0;
        while(recv_bytes <= 0)
            recv_bytes = msgrcv(msg_to_main, &msg_from_fork, sizeof(msg_from_fork), 1, 0);

        int resend_iter;
         for(resend_iter = 0; resend_iter < fork_n; resend_iter++)
            if(resend_iter != msg_from_fork.fork_source || strcmp(msg_from_fork.text, "bay") == 0 || strcmp(msg_from_fork.text, "!end!") == 0){
                msgsnd(msg_to_forks[resend_iter], &msg_from_fork, sizeof(msg_from_fork), 0);
         }
        if(strcmp(msg_from_fork.text, "!end!") == 0)
            *serv_work = 0;
    }

    close(main_socket);
    shmdt(serv_work);

    msgctl(msg_to_main, IPC_RMID, NULL);

    int del_iter;
    for(del_iter = 0; del_iter < fork_n; del_iter++){
        msgctl(msg_to_forks[del_iter], IPC_RMID, NULL);
        kill(fork_pids[del_iter], SIGTERM);
    }


    printf("Main stream end\n");
    return 0;

}
