#include "csrs.h"
#include "cbss.h"
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>

double func1(vector2lf arg){
    return (arg.x+5) * (arg.x+5) + (arg.y+1)*(arg.y+1);
}

double func2(vector2lf arg){
    double x = arg.x;
    double y = arg.y;
    double pi = 4*atan(1.0);
    return -20*exp(-0.2*sqrt(0.5*(x*x+y*y))) - exp(0.5*(cos(2*pi*x)+cos(2*pi*y))) + 20 + exp(1.0);
}

double func3(vector2lf arg){
    double x = arg.x;
    double y = arg.y;
    return -exp(-fabs(x)-y*y*y*y)*cos(2*x*x*x)*cos(10*y*y);
}

double myfunk(vector2lf arg)
{
    double x = arg.x;
    double y = arg.y;
    return -2.0/(1+(x-1)*(x-1)/4.0+(y-2)*(y-2)) - 1/(1+(x-3)*(x-3)/9.0+(y-1)*(y-1)/9.0);
}

int main()
{
    pid_t ch1;
    Csrs srs;

    /*if((ch1 = fork()) == 0) {*/
        /*srs.init_func(myfunk);
        srs.init_rectangle(vector2lf(0.5, 2.0), 1, 1);
        srs.init_tests(1E-5, 0.9);

        vector2lf min = srs.search();
        printf("Minimum_in:\t %.7lf\t%.7lf\nValue is:\t%.15lf\n", min.x, min.y, myfunk(min));
        return 0;*/
    /*}*/

    Cbss bss;

    bss.init_func(myfunk);
    bss.init_rectangle(vector2lf(0.5, 2.0), 1E-2, 1E-2);
    bss.init_max_iters(500000);
    bss.init_tests(1E-5, 0.9);
    bss.init_eps_grow(1E-2);

    vector2lf min = bss.search();
    printf("Minimun in:\t %.7lf\t%.7lf\nValue is:\t%.15lf\n", min.x, min.y, myfunk(min));

    /*waitpid(ch1,0,0);*/
    return 0;
}
