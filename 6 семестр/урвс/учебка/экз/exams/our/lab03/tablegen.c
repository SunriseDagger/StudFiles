/**
 * The process created a new file and reassign it to stdout.
 * This program demonstrate dynamics of creating tables associated with this event.
 *
 * @author      Alexandrov Mikhail
 * @author      Zhigalov Peter
 * @version     0.3
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * Structure for tables
 */
struct tab
{
    int descr;
    struct stat p;
};

/**
 * Main program
 *
 * @brief main
 * @return "0" generally
 */
int main()
{
    char *name[2];
    int i, index;
    struct tab mas[6];

    // Print tables after implicit open standard input, output and output error
    printf("Implicit opening of standard files...\n");
    printf("descr\tname\t\tperm\tinode\tnlink\tUID\tGID\tsize\n");
    printf("---------------------------------------------------------------------\n");
    for(i = 0; i < 3; i++)
    {
        mas[i].descr = i;
        fstat(i , &mas[i].p);
        printf("%i\t\t\t\t%li\t%li\n", mas[i].descr, mas[i].p.st_ino, mas[i].p.st_nlink);
    }

    printf("\nOpening of the user file...\n");
    // Opening user files
    name[0] = (char*)"file.txt";
    name[1] = (char*)"file1.txt";
    mas[3].descr = creat(name[0], 0666);
    mas[4].descr = creat(name[1], 0666);
    stat(name[0], &mas[3].p);
    stat(name[1], &mas[4].p);
    // Print tables after opening user files
    printf("descr\tname\t\tperm\tinode\tnlink\tUID\tGID\tsize\n");
    printf("---------------------------------------------------------------------\n");
    for(i = 0; i < 3; i++)
        printf("%i\t\t\t\t%li\t%li\n", mas[i].descr, mas[i].p.st_ino, mas[i].p.st_nlink);
    printf("%i\t%s\t%o\t%li\t%li\t%u\t%u\t%li\n", mas[3].descr, name[0], mas[3].p.st_mode,
           mas[3].p.st_ino, mas[3].p.st_nlink, mas[3].p.st_uid, mas[3].p.st_gid, mas[3].p.st_size);

    printf("\nClosing of a standart file of input...\n");
    // Closing the standard input file
    close(0);
    fstat(0,&mas[0].p);
    // Print tables after closing the standard input file
    printf("descr\tname\t\tperm\tinode\tnlink\tUID\tGID\tsize\n");
    printf("---------------------------------------------------------------------\n");
    for(i = 1; i < 3; i++)
        printf("%i\t\t\t\t%li\t%li\n", mas[i].descr, mas[i].p.st_ino, mas[i].p.st_nlink);
    for(i = 3; i < 5; i++)
        printf("%i\t%s\t%o\t%li\t%li\t%u\t%u\t%li\n", mas[i].descr, name[i-3], mas[i].p.st_mode,
               mas[i].p.st_ino, mas[i].p.st_nlink, mas[i].p.st_uid, mas[i].p.st_gid, mas[i].p.st_size);

    printf("\nReception of a copy of a descriptor of the user file...\n");
    // Reception of a copy of the user file descriptor
    mas[5].descr=dup(mas[3].descr);//copy descriptor
    fstat(mas[5].descr,&mas[5].p);
    // Print tables after reception of a copy of the user file descriptor
    printf("descr\tname\t\tperm\tinode\tnlink\tUID\tGID\tsize\n");
    printf("---------------------------------------------------------------------\n");
    for(i = 1; i < 3; i++)
        printf("%i\t\t\t\t%li\t%li\n", mas[i].descr, mas[i].p.st_ino, mas[i].p.st_nlink);
    for(i = 3; i < 6; i++)
    {
        if(i == 4)
            index=1;
        else
            index=0;
        printf("%i\t%s\t%o\t%li\t%li\t%u\t%u\t%li\n", mas[i].descr, name[index], mas[i].p.st_mode,
               mas[i].p.st_ino, mas[i].p.st_nlink, mas[i].p.st_uid,mas[i].p.st_gid,mas[i].p.st_size);
    }

    printf("\nClose file1.txt\n");
    printf("descr\tname\t\tperm\tinode\tnlink\tUID\tGID\tsize\n");
    printf("---------------------------------------------------------------------\n");
    // Closing the user file
    close(mas[4].descr);
    // Print tables after closing the user file
    for(i = 1; i < 3; i++)
        printf("%i\t\t\t\t%li\t%li\n", mas[i].descr, mas[i].p.st_ino, mas[i].p.st_nlink);
    for(i = 3; i < 6; i++)
    {
        if(i != 4)
            printf("%i\t%s\t%o\t%li\t%li\t%u\t%u\t%li\n",mas[i].descr,name[0],mas[i].p.st_mode,
                   mas[i].p.st_ino, mas[i].p.st_nlink, mas[i].p.st_uid, mas[i].p.st_gid, mas[i].p.st_size);
    }
    return 0;
}
