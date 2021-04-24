#include "cbss.h"

//Константы для параллелизма
const int ch_n = 4; //количество потоков


void Cbss::init_func(func2lf s_func)
{
    min_f = s_func;
}

void Cbss::init_rectangle(vector2lf s_x0, double s_hx, double s_hy){
    x0 = s_x0;
    hx = s_hx;
    hy = s_hy;
    change_rect();
}

void Cbss::init_tests(double s_eps, double s_P){
    eps = s_eps;
    P = s_P;
}

void Cbss::init_max_iters(int s_max_iters){
    max_iters = s_max_iters;
}
void Cbss::init_eps_grow(double s_eps_grow){
    eps_grow = s_eps_grow;
}

void Cbss::change_rect(){
    left_bottom = x0 - vector2lf(hx/2, hy/2);
    right_top = x0 + vector2lf(hx/2, hy/2);
}

bool Cbss::check_bounds(vector2lf ch_p){
    /*if((3*ch_p.x + ch_p.y) <= 3)
        return true;
    else
        return false;*/return true;
}

vector2lf Cbss::search(){

//	vector2lf xk; //Очередное приближение
//	double fk; //Значение в приближении

    unsigned long int teaste = 0; //общее количество проб

    min_f_val = min_f(x0);

    //Проверяем содержит ли квадрат точки области, если не содержит - увеличиваем квадрат, пока не начнёт содержать
    if(!check_bounds(x0)){
        while(!(check_bounds(left_bottom) || check_bounds(right_top) || check_bounds(x0+vector2lf(hx/2,0)) || check_bounds(x0+vector2lf(-hx/2,0)) || check_bounds(x0+vector2lf(0,hy/2)) || check_bounds(x0+vector2lf(0,-hy/2)))){
            hx *= 2.0;
            hy *= 2.0;
            change_rect();
        }
    }

    FILE *out_f = fopen("iters_info.txt", "w");

    //Собственно поиск
    bool cycle_continue = true; //дополнительные условия выхода из цикла
    for(int iter = 0; iter < max_iters && cycle_continue; iter++){

        printf("%d\t%.7lf\t%.7lf\t%.15lf\t%.7lf\t%.7lf\n", iter, x0.x, x0.y, min_f_val, hx, hy);
        fprintf(out_f, "%d\t%.7lf\t%.7lf\t%.15lf\t%.7lf\t%.7lf\n", iter, x0.x, x0.y, min_f_val, hx, hy);
        fflush(out_f);

        //Поиск минимума в текущем прямоугольнике
        double min_val_st = min_f_val; //минимальное занчение с предыдущей итерации
        double P_eps = eps*eps / (hx * hy);
        tests_N = log(1 - P) / log(1 - P_eps);
        printf("%d\t%d\n", iter, tests_N);
        teaste += tests_N;

        //============ Начало паралелльной версии =============

        int p_iter; //интераро по потокам
        int it;
        long int thread_n = tests_N / ch_n + 1; //количество чисел на поток

        vector<vector2lf> points; //генерируемые точки
        vector<double> vals; //значения функции

        vector<vector2lf> rez_p;
        vector<double> rez_f;

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
                            do{
                                points[i] = rand_gen.generate(left_bottom, right_top);
                            }while(!check_bounds(points[i]));

                            vals[i] = min_f(points[i]);
                        }

                        int min_n = distance(vals.begin(), min_element(vals.begin(), vals.end())); //номер минимального элемента
                        if(vals[min_n] < rez_f[num]){
                            rez_p[num] = points[min_n];
                            rez_f[num] = vals[min_n];
                        }
                    }
                }
            }
        }

        int glob_min_n = distance(rez_f.begin(), min_element(rez_f.begin(), rez_f.end()));
        x0 = rez_p[glob_min_n];
        min_f_val = rez_f[glob_min_n];

            //Последовательная версия
            /*
            xk = rand_gen.generate(left_bottom, right_top);
            while(!check_bounds(xk)){
                xk = rand_gen.generate(left_bottom, right_top);
            }

            //printf("%.8lf\t%.8lf\n", xk.x, xk.y);

            fk = min_f(xk);

            if(fk < min_f_val){
                x0 = xk;
                min_f_val = fk;
            }
        }*/

        double alpha = min_val_st / min_f_val; //коэффциент, на который уменьшается прямоугольник

        //Двигаем прямоугольник
        double diff = fabs(min_val_st - min_f_val);
        if(diff > eps_grow){
            hx /= alpha;
            hy /= alpha;
        }
        else{
            hx *= alpha;
            hy *= alpha;
        }


        change_rect();

        cycle_continue = (hx > eps || hy > eps) && diff > eps; //пока длина сторон большАя и значения функции меняются - продолжаем считать
    }

    fprintf(out_f, "Min is:\t%.6lf\t%.6lf\t%.6lf\n%lu", x0.x, x0.y, min_f_val, teaste);

    fclose(out_f);

    return x0;
}
