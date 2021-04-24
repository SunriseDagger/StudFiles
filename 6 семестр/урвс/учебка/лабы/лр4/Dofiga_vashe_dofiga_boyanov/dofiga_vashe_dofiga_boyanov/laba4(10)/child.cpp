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
    int pipeFd[2];
    double rez;
    int outp, N;
  

  if(argc != 3)  { printf("You should specify 2 arguments.\n");
                   exit(1);}

  N = atoi(argv[1]);
  outp = atoi(argv[2]); // Читаем вторым параметром номер дескриптора, для вывода
  
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
        
        write(outp, Vals, sizeof(double) * N);
        free(Vals);
        printf("Child process stopped.\n");
        exit(1);
}  
