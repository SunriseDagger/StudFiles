#include "pv.h"

void p0(int sem_id){
    struct sembuf oper[2];
    oper[0].sem_num = 0;
    oper[0].sem_op = -1;
    oper[0].sem_flg = 0;

    oper[1].sem_num = 1;
    oper[1].sem_op = 1;
    oper[1].sem_flg = 0;

    int sem_check = semop(sem_id, oper, 2);


   if(sem_check < 0){
        printf("Can't p0 semafor\n");
        char error_text [100];
        perror(error_text);
        printf("Error:\t%s", error_text);
        exit(2);
    }

    printf("\t!! p0 !!\n");
}

void v0(int sem_id){
    struct sembuf oper[2];
    oper[0].sem_num = 0;
    oper[0].sem_op = 1;
    oper[0].sem_flg = 0;

    oper[1].sem_num = 1;
    oper[1].sem_op = -1;
    oper[1].sem_flg = 0;

    int sem_check = semop(sem_id, oper, 2);


   if(sem_check < 0){
        printf("Can't v0 semafor\n");
        char error_text [100];
        perror(error_text);
        printf("Error:\t%s", error_text);
        exit(2);
    }

    printf("\t!! v0 !!\n");
}


void p1(int sem_id){
    struct sembuf oper[2];
    oper[0].sem_num = 0;
    oper[0].sem_op = 1;
    oper[0].sem_flg = 0;

    oper[1].sem_num = 1;
    oper[1].sem_op = -1;
    oper[1].sem_flg = 0;

    int sem_check = semop(sem_id, oper, 2);


   if(sem_check < 0){
        printf("Can't p1 semafor\n");
        char error_text [100];
        perror(error_text);
        printf("Error:\t%s", error_text);
        exit(2);
    }
    printf("\t!! p1 !!\n");
}

void v1(int sem_id){
    struct sembuf oper[2];
    oper[0].sem_num = 0;
    oper[0].sem_op = -1;
    oper[0].sem_flg = 0;

    oper[1].sem_num = 1;
    oper[1].sem_op = 1;
    oper[1].sem_flg = 0;

    int sem_check = semop(sem_id, oper, 2);


   if(sem_check < 0){
        printf("Can't v1 semafor\n");
        char error_text [100];
        perror(error_text);
        printf("Error:\t%s", error_text);
        exit(2);
    }
    printf("\t!! v1 !!\n");
}
