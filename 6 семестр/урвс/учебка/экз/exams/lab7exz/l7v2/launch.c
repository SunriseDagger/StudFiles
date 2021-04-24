#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>


union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  array[4];  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};

void print_error() {
    char error_text[100];
    perror(error_text);
    printf("Error: %s\n", error_text);
}

void print_sem(int sem_id){
    union semun info;
    semctl(sem_id, 0, GETALL, &info);
    int info_iter;
    for(info_iter = 0; info_iter < 4; info_iter++)
        printf("%d: %d\t", info_iter, info.array[info_iter]);
    printf("\n");
}


void sem_def(int sem_id) {
    union semun info;

    int oper_iter;
    for(oper_iter = 0; oper_iter < 4; oper_iter++){
        info.array[oper_iter] = 1;
    }

    int check = semctl(sem_id, 0, SETALL, &info);
    if(check < 0){
        printf("Can't set semafor default\n");
        print_error();
        semctl(sem_id, 0, IPC_RMID, &info);
        exit(2);
    }
}

void p_fork(int sem_id, int fork_n){
        union semun info;
        struct sembuf operation[4];

    int oper_iter;
    for(oper_iter = 0; oper_iter < 4; oper_iter++){
        operation[oper_iter].sem_num = oper_iter;
        if(oper_iter == fork_n)
            operation[oper_iter].sem_op = -1;
        else
            operation[oper_iter].sem_op = 1;
        operation[oper_iter].sem_flg = 0;
    }

    int check = semop(sem_id, operation, 4);
    if(check < 0){
        printf("Can't p %d\n", fork_n);
        print_error();
        semctl(sem_id, 0, IPC_RMID, &info);
        exit(3);
    }
}

void v_fork(int sem_id, int fork_n){
        union semun info;
        struct sembuf operation[4];

    int oper_iter;
    for(oper_iter = 0; oper_iter < 4; oper_iter++){
        operation[oper_iter].sem_num = oper_iter;
        if(oper_iter == fork_n)
            operation[oper_iter].sem_op = 1;
        else
            operation[oper_iter].sem_op = -1;
        operation[oper_iter].sem_flg = SEM_UNDO;
    }

    int check = semop(sem_id, operation, 4);
    if(check < 0){
        printf("Can't p %d\n", fork_n);
        print_error();
        semctl(sem_id, 0, IPC_RMID, &info);
        exit(3);
    }
}

int main() {

    union semun info;
    char empty_str = "";

    key_t sh_key = 0X212;
    key_t sem_key = 0X223;
    char* sh_mes2;

    int sh_mem = shmget(sh_key, sizeof(char[200]), IPC_EXCL | IPC_CREAT | 0777);
    int sem_id = semget(sem_key, 4, IPC_EXCL | IPC_CREAT | 0777);

    if(sh_mem < 0) {
        if(errno == EEXIST){ //если память сушествовала, то пытаемся выделить заного
            printf("Memory exists - connecting to it\n");
            sh_mem = shmget(sh_key, sizeof(char[200]), 0);
        }
        else{
            printf("Can't get memory\n");
            print_error();
            exit(1);
        }
    }
    if(sem_id < 0){

        if(errno == EEXIST){
            printf("Semafors exist - connection to them\n");
            sem_id = semget(sem_key, 4, 0);
        }
        else{
            printf("Can't get semafor\n");
            print_error();
            exit(1);
        }
    }

    sem_def(sem_id);
    print_sem(sem_id);

    FILE* text_file = fopen("text.txt", "r");
    int err_f = open("err.txt", O_WRONLY | O_CREAT);

    if(text_file == NULL) {
        printf("Can't open file\n");
        print_error();
        semctl(sem_id, 0, IPC_RMID);
        exit(1);
    }

    sh_mes2 = (char*)shmat(sh_mem, NULL, SHM_RND);

    if(sh_mes2 < 0) {
        printf("Can't connect memory\n");
        print_error();
        semctl(sem_id, 0, IPC_RMID);
        exit(1);
    }
    strcpy(sh_mes2 , "");

    int forks_pid[4];
    int main_pid = getpid();

    int fork_iter;
    for(fork_iter = 0; fork_iter < 4; fork_iter++) {
        forks_pid[fork_iter] = fork();
        if(forks_pid[fork_iter] == 0) {
            int in_fork_iter;
            printf("Fork %d created\n", fork_iter);
            for(in_fork_iter = 0; in_fork_iter < 10; in_fork_iter++) {
                //printf("Fork %d sleep\n", fork_iter);
                sleep(rand()%2);

                p_fork(sem_id, fork_iter);
               // print_sem(sem_id);

               // printf("Fork %d wake\n", fork_iter);
                char fork_mes[100];
                if(!feof(text_file)){
                    fscanf(text_file, "%s", &fork_mes);
                    char send_msg[104];
                    send_msg[0] = 'M';
                    send_msg[1] = fork_iter + '0';
                    send_msg[2] = '\0';
                    strcat(send_msg, fork_mes);
                    strcpy(sh_mes2, "");
                    strcpy(sh_mes2 , send_msg);
                    //printf("Fork %d send: %s\n", fork_iter, send_msg);

                    fscanf(text_file, "%s", &fork_mes);
                    fscanf(text_file, "%s", &fork_mes);
                    fscanf(text_file, "%s", &fork_mes);
                } else {
                    printf("Fork %d EOF\n", fork_iter);
                    v_fork(sem_id, fork_iter);
                    break;
                }
                v_fork(sem_id, fork_iter);
                //print_sem(sem_id);
                close(text_file);
            }
            printf("Fork %d cycle end\n", fork_iter);
            p_fork(sem_id, fork_iter);
            sh_mes2[0] = 'E';
            v_fork(sem_id, fork_iter);
            return 0;
        } else if(forks_pid[fork_iter] < 0) {
            printf("Can't create fork %d\n", fork_iter);
            print_error();
            shmdt(sh_mes2);
            semctl(sem_id, 0, IPC_RMID, &info);
            close(text_file);
            exit(2);
        }
    }


    char msgs[200][100];
    int msggets[4] = {0,0,0,0};
    int fork_ended = 0;
    printf("Here?\n");
    while(fork_ended < 4){
        if(sh_mes2[0] == 'M') {
            int fork_n = sh_mes2[1] - '0';
            if(fork_n > 3){
                printf("We have a problem, sir!\n");
                printf("\tMsg recv:\t%s\n", sh_mes2);
                printf("\tFirst letter:\t%c\n", sh_mes2[0]);
            }
            strcpy(msgs[msggets[fork_n]*4 + fork_n], sh_mes2);
            printf("We put in %d: %s\n", msggets[fork_n]*4 + fork_n, sh_mes2);
            msggets[fork_n]++;
            strcpy(sh_mes2 , "");
            //v_fork(sem_id, fork_n);
        } else {
            if(sh_mes2[0] == 'E'){
                fork_ended++;
                strcpy(sh_mes2 , "");
                printf("\tForks endes: %d\n", fork_ended);
            }

        }
    }
    FILE* out_file = fopen("out.txt", "w");

    int total_mes = msggets[0] + msggets[1] + msggets[2] + msggets[3];
    int outp_iter;
    for(outp_iter = 0; outp_iter < total_mes; outp_iter++)
        fprintf(out_file, "%s\n", msgs[outp_iter]);

    shmdt(sh_mes2);
    semctl(sem_id, 0, IPC_RMID);
    fclose(text_file);
    fclose(out_file);

    return 0;
}
