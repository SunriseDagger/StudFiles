/**
 * Program for compute value of f (x) as sum of series from k = 0 to k = N
 * on expression x ^ (2k +1) / (2k +1)! for values ​​of x, uniformly distributed
 * on interval [0; Pi].
 *
 * @author      Alexandrov Mikhail
 * @author      Zhigalov Peter
 * @version     0.3
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <math.h>

/**
 * Function for calculate sum of series
 *
 * @param x argument of function
 * @param N number elements on series
 * @return sum of series
 */
double f(double x, int N)
{
    double sum = x;
    int tmp;
    for (int i = 1; i < N; i++)
    {
        tmp = i * 2;
        sum += sum * x * x / tmp / (tmp + 1);
    }
    return sum;
}

/**
 * Main function
 *
 * @param argc number of program arguments
 * @param argv program arguments
 * @return     1 - incorrect parameters, 0 - all correct
 */
int main(int argc, char* argv[])
{
    /** Check arguments number */
    if (argc != 2)
    {
        fprintf(stderr, "1 arguments expected.\n");
        exit(1);
    }
    /** Config file loading */
    FILE *file = fopen("./config.txt", "r");
    if (!file)
    {
        fprintf(stderr, "Can't open configuration file.\n");
        exit(1);
    }
    int N = -1;
    if (!fscanf(file, "%i", &N) || N == -1)
    {
        fprintf(stderr, "Can't read configuration file.\n");
        fclose(file);
        exit(1);
    }
    fclose(file);
    /** Arguments loading */
    int num_steps = 1;
    if (!sscanf(argv[1], "%i", &num_steps))
    {
        fprintf(stderr, "Can't read arguments.\n");
        exit(1);
    }
    if (num_steps <= 0)
    {
        fprintf(stderr, "Bad arguments.\n");
        exit(1);
    }
    double h = M_PI / num_steps;

    pid_t child_pid;
    struct flock lock, lock_info;
    double data[2];
    int flag = 0, temp;

    const char *temp_file = "/tmp/summator_msg"; /**< Temp file */
    int fd;
    fd = open(temp_file, O_RDWR | O_TRUNC | O_CREAT, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "Can't open temp file.\n");
        exit(1);
    }
    close(fd);

    switch(child_pid = fork())
    {
    /** Can`t create fork */
    case (pid_t)-1:
        fprintf(stderr, "Error creatinfg fork.\n");
        break;

    /** Code for forked process */
    case (pid_t)0:
    {
        child_pid = getpid();
        /** Some problems with temp file */
        if ((fd = open(temp_file, O_RDWR)) == -1)
        {
            fprintf(stderr, "Can't open temp file.\n");
            exit(1);
        }
        /** While not STOP flag */
        while (flag != -1)
        {
            lock.l_len = 0;
            lock.l_start = 0;
            lock.l_whence = SEEK_SET;
            lock.l_type = F_WRLCK;
            /** Waiting unlocking and do lock */
            while (fcntl(fd, F_SETLK, &lock) == -1)
            {
                fcntl(fd, F_GETLK, &lock_info);
            }
            lseek(fd, 0, SEEK_SET);
            read(fd, &flag, sizeof(int));
            /** Print result */
            if (flag == 1)
            {
                read(fd, data, 2 * sizeof(double));
                printf("f(%lf) = %lf\n", data[0], data[1]);
                lseek(fd, 0 ,SEEK_SET);
                temp = 0;
                write(fd, &temp, sizeof(int));
            }
            lock.l_len = 0;
            lock.l_start = 0;
            lock.l_whence = SEEK_SET;
            lock.l_type = F_UNLCK;
            /** Unlocking file */
            fcntl(fd, F_SETLK, &lock);
            usleep(10000);
        }
        break;
    }

    /** Code for parent process */
    default:
    {
        int fd = open(temp_file, O_RDWR);

        for(int i = 0; i <= num_steps; i++)
        {
            double x = h * i;
            usleep(10000);
            lock.l_len = 0;
            lock.l_start = 0;
            lock.l_whence = SEEK_SET;
            lock.l_type = F_WRLCK;
            /** Waiting unlocking and do lock */
            while (fcntl(fd, F_SETLK, &lock) == -1)
            {
                fcntl(fd, F_GETLK, &lock_info);
            }
            lseek(fd, 0, SEEK_SET);
            read(fd, &flag, sizeof(int));
            if (flag == 0)
            {
                lseek(fd, 0, SEEK_SET);
                temp = 1;
                write(fd, &temp, sizeof(int));
                data[0] = x;
                data[1] = f(x, N);
                write(fd, data, 2 * sizeof(double));
            }
            lock.l_len = 0;
            lock.l_start = 0;
            lock.l_whence = SEEK_SET;
            lock.l_type = F_UNLCK;
            /** Unlocking file */
            fcntl(fd, F_SETLK, &lock);
        }

        /** Ending work */
        usleep(10000);
        lock.l_len = 0;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_type = F_WRLCK;
        /** Waiting unlocking and do lock */
        while (fcntl(fd, F_SETLK, &lock) == -1)
        {
            fcntl(fd, F_GETLK, &lock_info);
        }
        lseek(fd, 0, SEEK_SET);
        read(fd, &flag, sizeof(int));
        /** Write STOP flag */
        if (flag == 0)
        {
            lseek(fd, 0, SEEK_SET);
            temp = -1;
            write(fd, &temp, sizeof(int));
        }
        lock.l_len = 0;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_type = F_UNLCK;
        /** Unlocking file */
        fcntl(fd, F_SETLK, &lock);
        wait(&temp);
        break;
    }
    }
    close(fd);
    return 0;
}
