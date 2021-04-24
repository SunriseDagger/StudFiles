#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

#define N 100

//bubble
void info_sort(struct stat* finfo, struct dirent **info, int n) {
    int i, j;
    for(i = 0; i < n; i++) {
        for(j = 0; j < i; j++)
            if(finfo[i].st_nlink < finfo[j].st_nlink) {
                struct stat c = finfo[i];
                finfo[i] = finfo[j];
                finfo[j] = c;

                struct dirent* d = info[i];
                info[i] = info[j];
                info[j] = d;

            }
    }

}

int main() {
    DIR  *our_dir = 0;
    struct dirent *info[N];
    struct stat finfo[N];
    int dir_n = 0;
    int end_flag = 1;

    do {
        our_dir = opendir(".");
        struct stat end_info[2];
        stat(".", &end_info[0]);
        stat("..", &end_info[1]);
        if(end_info[0].st_ino == end_info[1].st_ino)
            end_flag = 0;
        if(our_dir != NULL) {
            int read_flag = 1;
            int read_iter;
            int real_n = 0;
            for(read_iter = 0; read_iter < N && read_flag; read_iter++) {
                info[read_iter] = readdir(our_dir);
                if(info[read_iter] == NULL) {
                    read_flag = 0;
                }
                else {
                    char full_path[100];
                    strcpy(full_path, "./");
                    strcat(full_path, info[read_iter]->d_name);

                    int check = stat(full_path, &finfo[read_iter]);

                    if(check < 0) {

                        printf("Stat problem %d %s\n", read_iter, full_path);
                        char err_msg[100];
                        perror(err_msg);
                        printf("Error:\t%s\n", err_msg);
                        closedir(our_dir);
                        exit(1);

                    }
                    real_n ++;
                }

            }



            closedir(our_dir);
            info_sort(finfo, info, real_n);
            int out_iter;

            printf("====\tDir %d\t===\n", dir_n);
            for(out_iter = 0; out_iter < real_n; out_iter++) {
                printf("%d\t", out_iter);
                printf("%s\t", info[out_iter]->d_name);
                printf("%d\n", finfo[out_iter].st_nlink);
                if(out_iter != real_n-1 && finfo[out_iter].st_nlink != finfo[out_iter+1].st_nlink)
                    printf("\n");
            }

            printf("\n\n");

            dir_n++;
            chdir("..");
        }

    } while(end_flag);
}
