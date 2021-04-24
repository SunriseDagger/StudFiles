#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dir.h>      
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>

double myFunc(double x)
{
    return 4 / (1 + x * x);
}

main(int argc,char *argv[])
{ 
    int pid,pid1;
	int fp;
    double rez;
    int N = 0;
  

  if(argc != 2)  { printf("You should specify N argument - quantity of random points.\n");
                   exit(1);}

  N = atoi(argv[1]);       
  
  if(N == 0) {printf("Too small number of points specified: %d\n",N);
              exit(1);}

  printf("Open file.\n");

  // Открываем файл
  fp = open(".tmpfile", O_CREAT | O_TRUNC | O_RDWR, 0660);
  if (fp < 0) {printf("Can't open file"); exit(1);}
                        
    printf("Starting child process.\n");
    pid = fork();   
  
    if(pid == -1) {printf("Can't start child process");
        exit(1);}
                  
    if(pid == 0) 
    {
        srand48(N);
        
        double* Vals;
        Vals = (double*)malloc(N * sizeof(double));
        
        long ind;
        for(ind = 0; ind < N; ++ind)
        {
            Vals[ind] = drand48();
        }
        
        int changed = 1;
        
        while(changed == 1)
        {
            changed = 0;
            for(ind = 0; ind < N - 1; ++ind)
                if(Vals[ind] > Vals[ind + 1])
                {
                    double val;
                    val = Vals[ind];
                    Vals[ind] = Vals[ind + 1];
                    Vals[ind + 1] = val;
                    changed = 1;
                }
        }
        for(ind = 0; ind < N; ++ind)
        {
            printf("%.6f\n", Vals[ind]);
        }
        
        write(fp, Vals, sizeof(double) * N);
        free(Vals);
        printf("Child process stopped.\n");
        exit(1);
    }

    // Ожидаем завершение потомка
    wait();

    lseek(fp, 0, SEEK_SET);

    int ind;
    double* Vals;
    Vals = (double*)malloc(N * sizeof(double));
        
    read(fp, Vals, sizeof(double) * N);
        
    double integral = 0;
    for(ind = 0;ind < N - 1; ind++)
    {
        integral += (myFunc(Vals[ind]) + myFunc(Vals[ind])) * (Vals[ind + 1] - Vals[ind]) / 2;
    }
   
    free(Vals);
    close(fp);
    unlink(".tmpfile");
    printf("Integral = %f\n", integral);
}  
