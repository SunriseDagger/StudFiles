#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define N 100
#define ch_max  1024

int st_time;

struct table_el {
    char name[ch_max];
    struct stat info;
};

void print_error() {
    char err_msg[100];
    perror(err_msg);
    printf("Error:\t%s\n", err_msg);
}

void print_table(struct table_el* table, int n) {

    int i;
    char name_s[] = "Name";
    printf("N\t%20s\tLinks\tSize\tLast chenge\n", name_s);
    for(i = 0; i < n; i++) {
        printf("%d\t%20s\t%d\t%d\t%d\n",i, table[i].name, table[i].info.st_nlink, table[i].info.st_size, table[i].info.st_mtime - st_time);
    }

    printf("\n\n");
}

int inp_table(struct table_el* table) {

    st_time = time(0);
    struct dirent *tmp_info;
    int n = 0;
    DIR* our_dir = opendir(".");

    if(our_dir == NULL) {
        printf("Can't open dir");
        exit(1);
    }

    int read_iter;
    int read_flag = 1;
    for(read_iter = 0; read_iter < N && read_flag; read_iter++) {
        int errno_old = errno;
        tmp_info = readdir(our_dir);
        if(tmp_info == NULL) {
            if(errno_old == errno)
                read_flag = 0;
            else {
                printf("Read error on file %d\n", read_iter);
                print_error();
                closedir(our_dir);
                exit(1);
            }
        }
        else {
            strcpy(table[n].name, tmp_info->d_name);
            char full_path[ch_max] = "./";
            strcat(full_path, tmp_info->d_name);
            int check = stat(full_path, &table[n].info);
            if(check < 0) {
                printf("Can't get %s stat\n", tmp_info->d_name);
                print_error();
                closedir(our_dir);
                exit(1);
            }
            n++;
        }
    }
    closedir(our_dir);
    return n;
}

int find_in_table(struct table_el *table, int n, char* name){
    int i;
    for(i = 0; i < n; i++)
        if(strcmp(table[i].name, name) == 0)
            return i;
    return -1;
}

int main() {

    struct table_el table[N];
    int n = inp_table(table);
    print_table(table, n);

    printf("Open file1\n");
    int f[3];
    f[0] = open("file1", O_WRONLY);

    n = inp_table(table);
    print_table(table, n);

    printf("Open file2\n");
    f[1] = open("file2", O_RDONLY);

    n = inp_table(table);
    print_table(table, n);

    printf("Open file3\n");
    f[1] = open("file3", O_WRONLY);

    n = inp_table(table);
    print_table(table, n);


    printf("Empty file3\n");
    close(f[2]);
    f[2] = open("file3", O_CREAT | O_WRONLY);
    n = inp_table(table);
    print_table(table, n);

    printf("Copy from file2 to file3\n");
    char buf[ch_max];
    int f2_n = find_in_table(table, n, "file2");
    read(f[1], buf, table[f2_n].info.st_size);
    write(f[2], buf, table[f2_n].info.st_size);
    n = inp_table(table);
    print_table(table, n);

    printf("Create link file4 to file3\n");
    link("file3", "file4");
    n = inp_table(table);
    print_table(table, n);

    printf("Unlink\n");
    unlink("file4");
    n = inp_table(table);
    print_table(table, n);


    int cl_iter;
    for(cl_iter = 0 ; cl_iter < 3; cl_iter++)
        close(f[cl_iter]);


    return 0;
}
