#pragma once

#include <math.h>

void prepared(int n);
void solve_Gauss(double **A, double *x, int n); //решение СЛАУ, метод Гаусса

void trianglematrix1(double **A, double *x, int n); //Приведение матрицы к верхнему треугольному виду
void transf1(double **A, double *x, int i,int n); // перестановка строк