#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <cstdlib>
using namespace std;

void p(int semid, int semcount)
{
    struct sembuf p_buf;
    p_buf.sem_num = semcount;
    p_buf.sem_op = -1;
    p_buf.sem_flg = 0;
    if(semop(semid, &p_buf, 1) == -1)
    {
        cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
        exit(1);
    }
}

void v(int semid, int semcount)
{
    struct sembuf p_buf;
    p_buf.sem_num = semcount;
    p_buf.sem_op = 1;
    p_buf.sem_flg = SEM_UNDO;
    if(semop(semid, &p_buf, 1) == -1)
    {
        cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
        exit(1);
    }
}

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(int argc, char** argv)
{
    key_t semkey = 0x201;
    int semid = semget(semkey, 2, 0600 | IPC_CREAT | IPC_EXCL);
    union semun arg;
    if(semid == -1 && errno == EEXIST)
        semid = semget(semkey, 2, 0);
    if(semid == -1)
    {
        cerr << "Can`t create semaphore!" << endl;
        cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
        exit(1);
    }
    arg.val = 1;
    if(semctl(semid, 0, SETVAL, 1) == -1)
    {
        cerr << "Can`t initialize semaphore!" << endl;
        cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
        if(semctl(semid, 0, IPC_RMID, arg) == -1)
        {
            cerr << "Can`t delete semaphore!" << endl;
            cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
        }
        exit(1);
    }

    for(int i = 0; i < 10; i++)
    {
        if(fork() == 0)
        {
            int local_semid = semget(semkey, 2, 0);
            if(local_semid == -1)
            {
                cerr << "Can`t connect to semaphore!" << endl;
                cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
                exit(1);
            }
            p(local_semid, 0);

            cout << "Process N = " << i << ";  PID = " << getpid() << " here!" << endl;
            usleep(10000);

            v(local_semid, 0);
            exit(0);
        }
    }

    int r;
    for(int i = 0; i < 10; i++)
        wait(&r);

    if(semctl(semid, 0, IPC_RMID, arg) == -1)
    {
        cerr << "Can`t delete semaphore!" << endl;
        cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
        exit(1);
    }
    return 0;
}
