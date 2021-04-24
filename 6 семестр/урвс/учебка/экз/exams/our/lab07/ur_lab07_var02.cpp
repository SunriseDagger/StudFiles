/**
 * Resource management, Lab 07, Variant 02.
 * Interprocess programs communication.
 * @author      Alexandrov Mikhail
 * @author      Zhigalov Peter
 * @version     0.3
 */
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <string>

using namespace std;

class poetry
{
public:
    poetry();               /**< Constructor */
    ~poetry();              /**< Destructor */
    int run(char* file);    /**< General */
private:
    enum
    {
        TEXT_SIZE = 1024,	/**< Shared memory size */
        SEM_SIZE = 2,		/**< Number of semaphores */
        RD = 0,				/**< Number of semaphore for read */
        WR = 1,				/**< Number of semaphore for write */
        KEY = 0x200, 		/**< Key */
    };
    union semun             /**< Union for setting values */
    {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } un;
    vector<string> strings1, strings2;  /**< Strings from children 1 and 2 */
    vector<string> lyrics;              /**< Text of poetry */
    struct sembuf buffer[SEM_SIZE];     /**< Buffer of semaphores */
    int sem;                            /**< Number set of semaphores */
    int shm;                            /**< Number shared memory */
    pid_t child1, child2;               /**< PID of children */
    char *text;                         /**< Pointer for shared memory */
    void child_func(int npr);           /**< Function for child process */
    void print();                       /**< Function for print strings */
    int set_semop(int nr, int nw);      /**< Function for setting semaphore values */
    int load(char* file);               /**< Function for loading text */
};

/**
 * @brief Constructor
 */
poetry::poetry()
{
    /** Create set of semaphores */
    if((sem = semget(KEY, SEM_SIZE, IPC_CREAT | 0600)) == -1)
        cerr << "semget error." << endl, exit(1);
    /** Create shared memory */
    if((shm = shmget(KEY, TEXT_SIZE, IPC_CREAT | 0600)) == -1)
        cerr << "shmget error." << endl, exit(1);
    /** Setting initial values of semaphores */
    un.val = 1;
    if (semctl(sem, WR, SETVAL, un) == -1)
        cerr << "semctl error." << endl, exit(1);
    un.val = 0;
    if (semctl(sem, RD, SETVAL, un) == -1)
        cerr << "semctl error." << endl, exit(1);
    text = NULL;
}

/**
 * @brief Destructor
 */
poetry::~poetry()
{
    /** Delete set of semaphores */
    semctl(sem, 0, IPC_RMID, 0);
    /** Delete shared memory */
    shmctl(shm, IPC_RMID, 0);
}

/**
 * @brief Function for loading text
 * @param file File with text
 * @return 1 if error, 0 - all correct
 */
int poetry::load(char* file)
{
    ifstream in;
    string str;
    in.open(file, ios::in);
    if(!in.is_open())
        cerr << "incorrect file." << endl, exit(1);
    while(!in.eof())
    {
        getline(in, str);
        lyrics.push_back(str);
    }
    in.close();
    return 0;
}

/**
 * @brief Function for child process
 * @param npr Child number
 */
void poetry::child_func(int npr)
{
    int count = lyrics.size();
    string tmpstr;
    /** Loop for 4 lines over 4 lines */
    for(int i = npr * 4; i < count && !set_semop(0, -1); i += 8)
    {   /** Associating text vith shared memory */
        if ((text = (char*)shmat(shm, 0, 0)) == (char*)-1)
            cerr << "shmat error." << endl, exit(1);
        /** Writing */
        tmpstr.clear();
        for (int j = i; j < i + 4 && j < count; j++)
            tmpstr += lyrics[j] + "\n";
        strcpy(text, tmpstr.c_str());
        if (shmdt(text) == -1)      /** Deassociating text */
            cerr << "shmdt error." << endl, exit(1);
        if (set_semop(1, 0) == -1)
            cerr << "semop error." << endl, exit(1);
    }
    if(!set_semop(0, -1))
    {   /** Write last empty line */
        if ((text = (char*)shmat(shm, 0, 0)) == (char*)-1)
            cerr << "shmat error." << endl, exit(1);
        strcpy(text, "");
        if (shmdt(text) == -1)
            cerr << "shmdt error." << endl, exit(1);
        if (set_semop(1, 0) == -1)
            cerr << "semop error." << endl, exit(1);
    }
}

/**
 * @brief Function for setting semaphore values
 * @param nr Number of semaphore for read
 * @param nw Number of semaphore for write
 * @return 1 if error, 0 - all correct
 */
int poetry::set_semop(int nr, int nw)
{
    buffer[0].sem_num = RD;
    buffer[0].sem_op = nr;
    buffer[0].sem_flg = 0;
    buffer[1].sem_num = WR;
    buffer[1].sem_op = nw;
    buffer[1].sem_flg = 0;
    return semop(sem, buffer, SEM_SIZE);
}

/**
 * @brief General function
 * @param file File with text
 * @return 1 if error, 0 - all correct
 */
int poetry::run(char* file)
{
    load(file);
    if ((child1 = fork()) == -1)
        cerr << "fork #1 error." << endl, exit(1);
    else if (!child1)
        child_func(0), exit(0);
    if ((child2 = fork()) == -1)
        cerr << "fork #2 error." << endl, exit(1);
    else if (!child2)
        child_func(1), exit(0);
    int count = 2;              /**< Counter of childs */
    struct shmid_ds shm_info;   /**< Struct for shared memory info */
    bool flag;                  /**< Flag of number child who send string */
    while (count && !set_semop(-1, 0))
    {
        shmctl(shm, IPC_STAT, &shm_info);   /** Shared memory info */
        if (shm_info.shm_lpid == child1)    /** Set flag of number child who send string */
            flag = false;
        else if (shm_info.shm_lpid == child2)
            flag = true;
        else
            cerr << "invasion error." << endl, exit(1);
        if ((text = (char*)shmat(shm, 0, 0)) == (char*)-1)
            cerr << "shmat error." << endl, exit(1);
        if (strcmp(text, ""))   /** If text not empty */
        {
            if (flag)
                strings2.push_back(text);
            else
                strings1.push_back(text);
        }
        else
            count--;
        if (shmdt(text) == -1)
            cerr << "shmdt error." << endl, exit(1);
        if (set_semop(0, 1) == -1)
            cerr << "semop error." << endl, exit(1);
    }
    int s1, s2;         /** Some waitings */
    waitpid(child1, &s1, 0);
    waitpid(child2, &s2, 0);
    if(s1 || s2)
        cerr << "child error." << endl, exit(1);
    print();    /** Output */
    return 0;
}

/**
 * @brief Function for print strings
 */
void poetry::print()
{
    int max_i = max(strings1.size(), strings2.size());
    for(int i = 0; i < max_i; i++)
    {
        if(i < strings1.size())
            cout << strings1[i];
        if(i < strings2.size())
            cout << strings2[i];
    }
}

/**
 * @brief Main function
 * @param argc Number of arguments
 * @param argv Arguments
 * @return 1 if error, 0 - all correct
 */
int main(int argc, char **argv)
{
    if(argc != 2)
        cerr << "incorrect number of parameters." << endl, exit(1);
    poetry p;
    return p.run(argv[1]);
}

