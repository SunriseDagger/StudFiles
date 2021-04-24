/**
 * Resource management, Lab 06, Variant 02.
 * Simulation of Shell-interpreter.
 * @author      Alexandrov Mikhail
 * @author      Zhigalov Peter
 * @version     0.2
 */
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Main function
 * @return 1 if error, 0 - all correct
 */
int main()
{
    int fd_wc_a, fd_wc_b, fd_wc_c;
    int pipe_cat_wc[2];
    int s_wc1 = 0, s_wc2 = 0, s_wc4 = 0, s_cat = 0;
    pid_t wc1, wc2, cat, wc4 = (pid_t)NULL;

    printf("#0 main: #1 wc for \"wc -c < a.txt\"\n");
    /** fork for execution "wc -c < a.txt" */
    switch(wc1 = fork())
    {
    case (pid_t)-1:
        fprintf(stderr, "#1 wc: Can`t fork wc\n");
        exit(1);
    case (pid_t)0:
        printf("#1 wc: Forked\n");
        /** Open a.txt */
        if ((fd_wc_a = open("./a.txt", O_RDONLY)) < 0)
        {
            fprintf(stderr, "#1 wc: Can`t open a.txt\n");
            exit(1);
        }
        /** Replace stdin with a.txt */
        if (dup2(fd_wc_a, 0) < 0)
        {
            fprintf(stderr, "#1 wc: Can`t dup2\n");
            exit(1);
        }
        close(fd_wc_a);
        printf("#1 wc: execlp wc -c\n");
        /** Execute "wc -c" */
        execlp("wc","wc","-c",NULL);
        fprintf(stderr, "#1 wc: Can`t execlp wc -c\n");
        exit(1);
    }

    printf("#0 main: #2 wc for \"wc -c < b.txt\"\n");
    /** fork for execution "wc -c < b.txt" */
    switch(wc2 = fork())
    {
    case (pid_t)-1:
        fprintf(stderr, "#2 wc: Can`t fork wc\n");
        exit(1);
    case (pid_t)0:
        printf("#2 wc: Forked\n");
        /** Open b.txt */
        if ((fd_wc_b = open("./b.txt", O_RDONLY)) < 0)
        {
            fprintf(stderr, "#2 wc: Can`t open b.txt\n");
            exit(1);
        }
        /** Replace stdin with b.txt */
        if (dup2(fd_wc_b, 0) < 0)
        {
            fprintf(stderr, "#2 wc: Can`t dup2\n");
            exit(1);
        }
        close(fd_wc_b);
        printf("#2 wc: execlp wc -c\n");
        /** Execute "wc -c" */
        execlp("wc","wc","-c",NULL);
        fprintf(stderr, "#2 wc: Can`t execlp wc -c\n");
        exit(1);
    }

    /** Create pipe */
    if (pipe(pipe_cat_wc) == -1)
    {
        fprintf(stderr, "#0 main: Can't create pipe\n");
        exit(1);
    }

    printf("#0 main: #3 cat for \"cat a.txt b.txt | wc -c > c.txt\"\n");
    /** fork for execution "cat a.txt b.txt" */
    switch(cat = fork())
    {
    case (pid_t)-1:
        fprintf(stderr, "#3 cat: Can`t fork cat\n");
        exit(1);
    case (pid_t)0:
        printf("#3 cat: Forked\n");
        printf("#3 cat: cat a.txt b.txt\n");
        /** Replace stdout with pipe */
        if (dup2(pipe_cat_wc[1], 1) < 0)
        {
            fprintf(stderr, "#3 cat: Can't dup2\n");
            exit(1);
        }
        execlp("cat","cat","./a.txt","./b.txt", NULL);
        fprintf(stderr, "#3 cat: Can't execlp cat\n");
        exit(1);
    }
    /** Wait stopping cat */
    waitpid(cat, &s_cat, WUNTRACED);
    /** Close pipe on write */
    close(pipe_cat_wc[1]);
    /** Some errors messages */
    if(s_cat)
    {
        if(WIFSIGNALED(s_cat))
            fprintf(stderr, "#3 cat: Received unexpected signal %i\n", WTERMSIG(s_cat));
        if(WIFEXITED(s_cat))
            fprintf(stderr, "#3 cat: Returned status %i\n", WEXITSTATUS(s_cat));
        if(WIFSTOPPED(s_cat) && WSTOPSIG(s_cat))
            fprintf(stderr, "#3 cat: Stopped with signal %i\n", WSTOPSIG(s_cat));
    }
    else
    {
        printf("#3 cat: Finished\n");
        printf("#0 main: #4 wc for \"cat a.txt b.txt | wc -c > c.txt\"\n");
        /** fork for execution "wc -c > c.txt" */
        switch(wc4 = fork())
        {
        case (pid_t)-1:
            fprintf(stderr, "#4 wc: Can`t fork wc #3\n");
            exit(1);
        case (pid_t)0:
            printf("#4 wc: Forked\n");
            /** Open c.txt */
            if ((fd_wc_c = open("./c.txt", O_WRONLY | O_CREAT, 0664)) < 0)
            {
                fprintf(stderr, "#4 wc: Can`t open c.txt\n");
                exit(1);
            }
            /** Replace stdin with pipe */
            if (dup2(pipe_cat_wc[0], 0) < 0)
            {
                fprintf(stderr, "#4 wc: Can`t dup2\n");
                exit(1);
            }
            printf("#4 wc: execlp wc -c\n");
            /** Replace stdout with c.txt */
            if (dup2(fd_wc_c, 1) < 0)
            {
                fprintf(stderr, "#4 wc: Can`t dup2\n");
                exit(1);
            }
            close(fd_wc_c);
            close(pipe_cat_wc[1]);
            /** Execute "wc -c" */
            execlp("wc","wc","-c", NULL);
            fprintf(stderr, "#4 wc: Can`t execlp wc -c\n");
            exit(1);
        }
    }

    /** Wait stopping all wc`s */
    waitpid(wc2, &s_wc2, WUNTRACED);
    waitpid(wc1, &s_wc1, WUNTRACED);
    if(wc4)
        waitpid(wc4, &s_wc4, WUNTRACED);

    /** Some errors messages */
    if(!s_wc1)
        printf("#1 wc: Finished\n");
    else
    {
        if(WIFSIGNALED(s_wc1))
            fprintf(stderr, "#1 wc: Received unexpected signal %i\n", WTERMSIG(s_wc1));
        if(WIFEXITED(s_wc1))
            fprintf(stderr, "#1 wc: Returned status %i\n", WEXITSTATUS(s_wc1));
        if(WIFSTOPPED(s_wc1) && WSTOPSIG(s_wc1))
            fprintf(stderr, "#1 wc: Stopped with signal %i\n", WSTOPSIG(s_wc1));
    }
    if(!s_wc2)
        printf("#2 wc: Finished\n");
    else
    {
        if(WIFSIGNALED(s_wc2))
            fprintf(stderr, "#2 wc: Received unexpected signal %i\n", WTERMSIG(s_wc2));
        if(WIFEXITED(s_wc2))
            fprintf(stderr, "#2 wc: Returned status %i\n", WEXITSTATUS(s_wc2));
        if(WIFSTOPPED(s_wc2) && WSTOPSIG(s_wc2))
            fprintf(stderr, "#2 wc: Stopped with signal %i\n", WSTOPSIG(s_wc2));
    }
    if(wc4 && !s_wc4)
        printf("#4 wc: Finished\n");
    else if(wc4)
    {
        if(WIFSIGNALED(s_wc4))
            fprintf(stderr, "#4 wc: Received unexpected signal %i\n", WTERMSIG(s_wc4));
        if(WIFEXITED(s_wc4))
            fprintf(stderr, "#4 wc: Returned status %i\n", WEXITSTATUS(s_wc4));
        if(WIFSTOPPED(s_wc4) && WSTOPSIG(s_wc4))
            fprintf(stderr, "#4 wc: Stopped with signal %i\n", WSTOPSIG(s_wc4));
    }
    close(pipe_cat_wc[0]);
    if(!s_wc1 && !s_wc2 && !s_wc4 && !s_cat)
        printf("#0 main: Complete\n");
    else
    {
        printf("#0 main: Error\n");
        exit(1);
    }
    return 0;
}
