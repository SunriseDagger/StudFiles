#include "pv.h"

struct msg {
    long mtype;
    char mtext[1024];
};

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short array[2];
    struct seminfo  *__buf;
};

void printval(int sem_id) {
    union semun info;
    semctl(sem_id, 0, GETALL, info.array);
    printf("\t!!1: %d \t2: %d !!\n", info.array[0], info.array[1]);
}


void sem_def(int sem_id) {

   union semun info;
   info.array[0] = 1;
   info.array[1] = 1;

   int sem_check = semctl(sem_id, 0, SETALL, &info);


    if(sem_check < 0) {
        printf("Can't set defualt semafor\n");
        char error_text [100];
        perror(error_text);
        printf("Error:\t%s", error_text);
        exit(1);
    }
}


int main() {

    char error_text [100];
    key_t sem_key = 0X205; //ключ для семафора
    //Создать семафоры
    int sem_id = semget(sem_key, 2, IPC_CREAT | 0777);
    union semun info_dest;


    if(sem_id < 0) {
        if(errno == EEXIST){
            printf("Semafor allready exists - connection to it\n");
            sem_id = semget(sem_key, 2, 0);
        }
        else{
            printf("Can't create semafor\n");
            perror(error_text);
            printf("Error:\t%s\n", error_text);
            exit(1);
        }
    }

    sem_def(sem_id);
    printval(sem_id);

    FILE* text_file = fopen("text.txt", "r");

    if(text_file == NULL) {
        printf("Can't open file");
        exit(1);
    }

    int fork_pid[3]; //id форков
    key_t msgkey = 0x202;
    int msg_queue = msgget(msgkey, IPC_CREAT | 0777);
    if(msg_queue < 0) {
        char error_text [100];
        perror(error_text);
        printf("Error:\t%s\n", error_text);
        semctl(sem_id, 0, IPC_RMID, &info_dest);
        exit(1);
    }

    fork_pid[0] = fork();

    switch(fork_pid[0]) {
        case -1: {
            printf("Can't create fork 0\n");
            semctl(sem_id, 0, IPC_RMID, &info_dest);
            msgctl(msg_queue, IPC_RMID, NULL);
            exit(1);
        }
        break;

    case 0: {

        printf("\t!!Fork 0 started!!\n");
        char read_text[200]; //прочитанный текст
        struct msg sending_msg;

        while(!feof(text_file)) {
            sleep(1);
            printf("\t!!Fork 0 wake!!\n");

            p0(sem_id);
            printval(sem_id);


            printf("\t!!Fork 0 read!!\n");

            int read_iter;
            strcpy(sending_msg.mtext, "");
            for(read_iter = 0; read_iter < 4 && !feof(text_file); read_iter++) {
                fgets(read_text, 200, text_file);
                strcat(sending_msg.mtext, read_text);
            }
            for(read_iter = 0; read_iter < 4 && !feof(text_file); read_iter++) {
                fgets(read_text, 200, text_file);
            }

            printf("\t!!Fork 0 reading is over!!\n");
            sending_msg.mtype = 1; //от какого потока пришло сообщения

            printf("!!In 0: %s\n", sending_msg.mtext);

            int send_check = msgsnd(msg_queue, (void*)&sending_msg, sizeof(sending_msg), 0);
            if(send_check < 0) {
                printf("Can't send msg from fork 0, msg:\n%s\n\n", sending_msg.mtext);
                perror(error_text);
                printf("Error:\t%s\n", error_text);
                semctl(sem_id, 0, IPC_RMID, &info_dest);
                msgctl(msg_queue, IPC_RMID, NULL);
                exit(1);
            }
            v0(sem_id);
            printval(sem_id);


        }
        char end_msg[] = "!!end!!";
        strcpy(sending_msg.mtext, end_msg);
        sending_msg.mtype = 2;
        msgsnd(msg_queue, (void*)&sending_msg, sizeof(sending_msg), 0);
        printf("Fork 0 end\n");
        return 0;


    }
    break;

    default: {
        fork_pid[1] = fork();

        switch(fork_pid[1]) {
        case -1: {
            printf("Can't create fork 1\n");
            semctl(sem_id, 0, IPC_RMID, &info_dest);
            msgctl(msg_queue, IPC_RMID, NULL);
            exit(1);
        }
        break;

        case 0: {

            printf("\t!!Fork 1 started!!\n");

            char read_text[200]; //прочитанный текст
            struct msg sending_msg;

            while(!feof(text_file)) {
                sleep(2);
              //  printf("\t!!Fork 2 wake!!\n");

                printval(sem_id);
                p1(sem_id);
                printval(sem_id);

             //   printf("\t!!Fork 2 read!!\n");

                int read_iter;
                strcpy(sending_msg.mtext, "");
                //одно пропускаем, одно - читаем
                for(read_iter = 0; read_iter < 4 && !feof(text_file); read_iter++) {
                    fgets(read_text, 200, text_file);
                }
                if(!feof(text_file)){
                    for(read_iter = 0; read_iter < 4 && !feof(text_file); read_iter++) {
                        fgets(read_text, 200, text_file);
                        strcat(sending_msg.mtext, read_text);
                        printf("\t%s\n", read_text);
                    }
                    sending_msg.mtype = 2; //от какого потока пришло сообщения

                    // printf("!!In 2: %s\n", sending_msg.mtext);

                    int send_check = msgsnd(msg_queue, (void*)&sending_msg, sizeof(sending_msg), 0);
                    if(send_check < 0) {
                        printf("Can't send msg from fork 1, msg:\n%s", sending_msg.mtext);
                        perror(error_text);
                        printf("Error:\t%s\n", error_text);
                        semctl(sem_id, 0, IPC_RMID, &info_dest);
                        msgctl(msg_queue, IPC_RMID, NULL);
                        exit(1);
                    }
                }
                v1(sem_id);
                printval(sem_id);

            }

           char end_msg[] = "!!end!!";
           strcpy(sending_msg.mtext, end_msg);
           sending_msg.mtype = 2;
           msgsnd(msg_queue, (void*)&sending_msg, sizeof(sending_msg),0);
           printf("Fork 1 end\n");
           return 0;


        }
        break;

        default: {

            fork_pid[2] = fork();
            switch(fork_pid[2]) {
            case -1:{
                printf("Can't create fork 2\n");
                semctl(sem_id, 0, IPC_RMID, &info_dest);
                msgctl(msg_queue, IPC_RMID, NULL);
                exit(1);
            }break;

            case 0:{

            struct msg recv_msg;
            int forks_end = 0;
            char real_text[50][1024];
            int msg_gets[2] = {0, 0};

                while(forks_end < 2) {

                    int msg_check = 0;
                    while(msg_check <= 0)
                        msg_check = msgrcv(msg_queue, &recv_msg, sizeof(recv_msg), 1, 0);
                        if(strcmp(recv_msg.mtext, "!!end!!")){
                            forks_end++;
                            printf("Forks ended: %d\n", forks_end);
                        }
                        else{
                            printf("From 0:\n%s", recv_msg.mtext);
                            strcpy(real_text[msg_gets[0]*2], recv_msg.mtext);
                            msg_gets[0]++;
                        }


                    msg_check = 0;
                    while(msg_check <= 0)
                        msg_check = msgrcv(msg_queue, &recv_msg, sizeof(recv_msg), 2, 0);
                        if(strcmp(recv_msg.mtext, "!!end!!")){
                            forks_end++;
                            printf("Forks ended: %d\n", forks_end);
                        }
                        else{
                            printf("From 1:\n%s", recv_msg.mtext);
                            strcpy(real_text[msg_gets[1]*2 + 1], recv_msg.mtext);
                            msg_gets[1]++;
                        }
                 }

                printf("Output start\n");
                FILE* out_f = fopen("out.txt", "w");
                int iters_n = msg_gets[0] + msg_gets[1];
                printf("String: %d\n", iters_n);
                int out_iter;
                for(out_iter = 0; out_iter < iters_n; out_iter++)
                    fprintf(out_f, "%s\n", real_text[out_iter]);

                fclose(out_f);

                return 0;
            }break;

            default:{
                int fork_iter;
                for(fork_iter = 0; fork_iter < 2; fork_iter++)
                    wait();
            }break;
            }

        }
        break;
        }
    }
    break;
    }
    sleep(1);
    printf("The work is done!\n");
    msgctl(msg_queue, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID, &info_dest);

    return 0;
}
