#include "csrs.h"

//Константы для параллелизма
const int ch_n = 4; //количество потоков

void Csrs::init_func(func2lf s_func)
{
    min_f = s_func;
}

void Csrs::init_rectangle(vector2lf s_x0, double s_hx, double s_hy){
    x0 = s_x0;
    left_bottom = x0 - vector2lf(s_hx/2, s_hy/2);
    right_top = x0 + vector2lf(s_hx/2, s_hy/2);
}

void Csrs::init_tests(double eps, double P){
    double lx = right_top.x - left_bottom.x;
    double ly = right_top.y - left_bottom.y;
    double P_eps = eps * eps / (lx * ly);
    unsigned long asdf = log(1 - P) / log(1 - P_eps);
    tests_N = /*log(1 - P) / log(1 - P_eps)*/ asdf;
    /*printf("Total iters:\t%d\n", tests_N);*/
    printf("Total iters:\t%lu\t%lf\n", asdf, log(1.0 - P) / log(1.0 - P_eps));
}

vector2lf Csrs::search(){
//	vector2lf xk; //генерируемая точка
//	double fk; //значение функции в точек xk

    min_f_val = min_f(x0); //находим минимальное значение

    long int iter = 0;
    long int p_iter = 0;
    long int thread_n = tests_N / ch_n + 1; //количество чисел на поток

    vector < vector2lf > points; //генерируемые точки
    vector < double > vals; //значения функции

    vector < vector2lf > rez_p;
    vector < double > rez_f;

    rez_p.resize(ch_n);
    rez_f.resize(ch_n);

    for(int i = 0 ; i < ch_n; i++){
        rez_p[i] = x0;
        rez_f[i] = min_f_val;
    }

    int num; //номер текущего потока

#pragma omp parallel default(shared)
        {
#pragma omp for private(points, vals, num)
    for(p_iter = 0; p_iter < ch_n; p_iter++){
        num = omp_get_thread_num(); //узнаём номер потока

        const int N = 10000; //количесво чисел, которые мы генерируем за раз
        points.resize(N);
        vals.resize(N);
#pragma omp parallel default(shared)
        {
#pragma omp for private(iter)
        for(iter = 0; iter < thread_n / N + 1; iter++){

            for(int i = 0; i < N; i++){
                points[i] = rand_gen.generate(left_bottom, right_top);
                vals[i] = min_f(points[i]);
            }

            int min_n = distance(vals.begin(), min_element(vals.begin(), vals.end())); //номер минимального элемента
            if(vals[min_n] < rez_f[num]){
                rez_p[num] = points[min_n];
                rez_f[num] = vals[min_n];
                printf("Min change in %d\n", num);
                printf("%d\t%.15lf\t%.15lf\t%.15lf\n", iter, rez_p[num].x, rez_p[num].y, rez_f[num]);
            }
        }
        }
    }
        }

        int glob_min_n = distance(rez_f.begin(), min_element(rez_f.begin(), rez_f.end()));
        x0 = rez_p[glob_min_n];
        min_f_val = rez_f[glob_min_n];



    /*
        for(iter = 0; iter < tests_N; iter++){
        xk = rand_gen.generate(left_bottom, right_top);
        fk = min_f(xk);


        fprintf(out_f,"%d\t%.7lf\t%.7lf\t%.15lf\n", iter, xk.x, xk.y, fk);

        if(fk < min_f_val){
            x0 = xk;
            min_f_val = fk;
            printf("Min change\n");
            printf("%d\t%.7lf\t%.7lf\t%.15lf\n", iter, xk.x, xk.y, fk);
        }
    }
    */
    FILE *out_f = fopen("easy_iters.txt", "w");
    fprintf(out_f, "Min is:\t%.6lf\t%.6lf\t%.6lf", x0.x, x0.y, min_f_val);
    fclose(out_f);

    return x0;
}
