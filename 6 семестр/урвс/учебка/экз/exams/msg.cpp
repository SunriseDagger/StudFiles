#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <cstdlib>
using namespace std;

struct msg
{
    long mtype;
    char mtext[255];
};

int main(int argc, char** argv)
{
    key_t msgqkey = 0x201;
    int msgqid = msgget(msgqkey, 0666 | IPC_CREAT | IPC_EXCL);
    if(msgqid == -1 && errno == EEXIST)
        msgqid = msgget(msgqkey, 0);
    if(msgqid == -1)
    {
        cerr << "Can`t create message queue!" << endl;
        cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
        exit(1);
    }

    if(fork() == 0)
    {
        int msgqid_loc = msgget(msgqkey, 0);
        if(msgqid_loc == -1)
        {
            cerr << "Can`t connect to message queue!" << endl;
            cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
            exit(1);
        }

        int fd[2];
        pipe(fd);

        if(fork() == 0)
        {
            close(1);
            close(fd[0]);
            dup2(fd[1], 1);
            if(execlp("ls", "ls", "-lAh", NULL) < 0)
            {
                cerr << "Can`t run ls!" << endl;
                cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
                exit(1);
            }
            exit(0);
        }
        int r;
        wait(&r);

        close(fd[1]);
        close(0);
        dup2(fd[0], 0);

        while(!cin.eof())
        {
            string a;
            getline(cin, a);
            struct msg m;
            m.mtype = 1;
            strcpy(m.mtext, a.c_str());
            msgsnd(msgqid_loc, &m, sizeof(msg), IPC_NOWAIT);
        }

        close(fd[0]);
        exit(0);
    }

    int r;
    wait(&r);

    struct msg m;
    while(msgrcv(msgqid, &m, sizeof(msg), 1, IPC_NOWAIT) > 0)
    {
        cout << m.mtext << endl;
    }

    struct msqid_ds msg_stat;
    if(msgctl(msgqid, IPC_RMID, &msg_stat) == -1)
    {
        cerr << "Can`t delete message queue!" << endl;
        cerr << "Errno = " << errno << " (" << strerror(errno) << ")" << endl;
        exit(1);
    }
    return 0;
}
