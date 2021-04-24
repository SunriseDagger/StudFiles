#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/msg.h>

#define N 3

struct ip_info {
    int init;
    struct sockaddr info;
};

struct in_msg{
    long mtype;
    int fork_n;
};

void print_error() {
    char err_msg[100];
    perror(err_msg);
    printf("Error:\t%s\n", err_msg);
}

void get_ip(struct ip_info* info, char* g_ip) {
    if(info->init != -1) {
        struct sockaddr_in *info2 = (struct sockaddr_in*)info;
        char *tmp = inet_ntoa(info2->sin_addr);
        strcpy(g_ip, tmp);
    }
    else {
        strcmp(g_ip, "");
    }
}

int main() {

    int forks_pid[N];
    int main_socket;
    struct ip_info *fork_ip;

    main_socket = socket(AF_INET, SOCK_STREAM, 0);

    key_t mem_key = 0x222;
    key_t mem_key2 = 0x223;

    key_t msgkeys[N+1];
    int msg_q[N+1];
    int msg_iter;
    for(msg_iter = 0; msg_iter <= N; msg_iter++){
        msgkeys[msg_iter] = time(0) + msg_iter + 500;
        msg_q[msg_iter] = msgget(msgkeys[msg_iter], IPC_CREAT | 0777);
        if(msg_q[msg_iter] < 0){
            if(errno = EEXIST) {
                printf("Queue %d exist, connecting\n", msg_iter);
                msg_q[msg_iter] = msgget(msgkeys[msg_iter], 0);
            }
            else {
                printf("Can't create msg queue %d\n", msg_iter);
                print_error();
                exit(1);
            }
        }
    }


    int mem_id = shmget(mem_key, sizeof(struct ip_info)*N, IPC_CREAT | 0777);
    int mem_id2 = shmget(mem_key, sizeof(int), IPC_CREAT | 0777);
    if(mem_id < 0) {
        if(errno = EEXIST) {
            printf("Memmory exist, connecting\n");
            mem_id = shmget(mem_key, sizeof(struct ip_info)*N, 0);
        }
        else {
            printf("Can't create ip memory\n");
            print_error();
            exit(1);
        }
    }

    if(mem_id2 < 0) {
        if(errno = EEXIST) {
            printf("Memmory exist, connecting\n");
            mem_id2 = shmget(mem_key, sizeof(struct ip_info)*N, 0);
        }
        else {
            printf("Can't create int memory\n");
            print_error();
            exit(1);
        }
    }

    struct sockaddr_in info;
    info.sin_family = AF_INET;
    info.sin_port = htons(2256);
    info.sin_addr.s_addr = htonl(INADDR_ANY);

    int check = bind(main_socket, (struct sockaddr*)&info, sizeof(info));

    if(check < 0) {
        printf("Bind error\n");
        print_error();
        exit(1);
    }

    fork_ip = (struct ip_info*) shmat(mem_id, NULL, SHM_RND);
    int *serv_work;
    serv_work = (int*)shmat(mem_id2, NULL, SHM_RND);
    *serv_work = 1;

    int iter_init;
    for(iter_init = 0; iter_init < N; iter_init++) {
        fork_ip[iter_init].init = -1;
    }

    int fork_i;
    for(fork_i = 0; fork_i < N; fork_i++) {
        forks_pid[fork_i] = fork();
        if(forks_pid[fork_i] == 0) {
            printf("Fork %d stardet\n", fork_i);
            while(*serv_work) {

                printf("Fork %d listen\n", fork_i);
                int check_l = listen(main_socket, 3);
                if(check_l < 0) {
                    printf("Listen error\n");
                    print_error();
                    close(main_socket);
                    shmdt(mem_id);
                    exit(2);
                }

                int cl_socket = accept(main_socket, NULL, NULL);
                printf("Fork %d connect\n", fork_i);
                int resend_iter;
                for(resend_iter = 0; resend_iter < N; resend_iter++) {
                    if(resend_iter != fork_i && fork_ip[resend_iter].init != -1) {
                        char ri_ip[100];
                        get_ip(&fork_ip[resend_iter], ri_ip);
                        send(cl_socket, ri_ip, strlen(ri_ip)+1, 0);
                        printf("Sending %s from %d to %d\n", ri_ip, resend_iter, fork_i);
                        sleep(1);
                    }
                }

                int add_fork = fork();
                if(add_fork == 0){
                    while(*serv_work){
                        struct in_msg geting_msg;
                        msgrcv(msg_q[fork_i], &geting_msg, sizeof(geting_msg), 1, 0);
                        char ri_ip[100];
                        get_ip(&fork_ip[geting_msg.fork_n], ri_ip);
                        send(cl_socket, ri_ip, strlen(ri_ip)+1, 0);
                        printf("Sending %s from %d to %d\n", ri_ip, geting_msg.fork_n, fork_i);
                    }
                    return 0;
                }
                else{
                    if(add_fork == -1){
                        printf("Can't fork in fork %d", fork_i);
                        close(cl_socket);
                        close(main_socket);
                        *serv_work = 0;
                        return 0;
                    }
                }


                fork_ip[fork_i].init = 1;
                int sz_info;
                int check_pr = getpeername(cl_socket, &fork_ip[fork_i].info, &sz_info);
                if(check_pr < 0){
                    printf("Can't get name in fork %d\n", fork_i);
                    print_error();
                    close(cl_socket);
                    close(main_socket);
                    exit(2);
                }

                struct in_msg msg_to_oth;
                msg_to_oth.fork_n = fork_i;
                msg_to_oth.mtype = 1;
                msgsnd(msg_q[N], &msg_to_oth, sizeof(msg_to_oth), 0);


                char buff[100];
                recv(cl_socket, buff, sizeof(buff), 0);
                close(cl_socket);
                kill(add_fork, SIGTERM);
                fork_ip[fork_i].init = -1;
                printf("Fork %d close connection\n", fork_i);
            }

            return 0;
        }

        else {
            if(forks_pid[fork_i] == -1) {
                printf("Can't create fork %d\n", fork_i);
                close(main_socket);
                shmdt(mem_id);
                exit(2);
            }
        }
    }
    int msg_fork = fork();
    if(msg_fork == 0){
        while(*serv_work){
            struct in_msg tmp_msg;
            msgrcv(msg_q[N], &tmp_msg, sizeof(tmp_msg), 1, 0);

            int res_iter;
            for(res_iter = 0; res_iter < N; res_iter++){
                if(fork_ip[res_iter].init != -1 && res_iter != tmp_msg.fork_n)
                    msgsnd(msg_q[res_iter], &tmp_msg, sizeof(tmp_msg), 0);
            }
        }

        return 0;
    }
    else{
        if(msg_fork == -1){
            printf("Can't create fork for msg\n");
            *serv_work = 0;
        }
    }

    while(*serv_work){
        char tmp[100];
        scanf("%s", &tmp);
        if(strcmp(tmp, "!end!") == 0)
            *serv_work = 0;
    }

    printf("Shootind down server\n");
    sleep(1);
    close(main_socket);
    shmdt(fork_ip);

    int kill_iter;
    for(kill_iter = 0; kill_iter < N; kill_iter++){
        kill(forks_pid[kill_iter], SIGTERM);
        msgctl(msg_q[kill_iter], IPC_RMID, NULL);
    }
    kill(msg_fork, SIGTERM);
    msgctl(msg_q[N], IPC_RMID, NULL);

    shmdt(serv_work);

    printf("Server offline\n");


    return 0;
}
