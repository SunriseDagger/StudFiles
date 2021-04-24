#include "LU_full_matrix.h"

void LU_dec(double **A, int n){
	for(int i = 0; i < n; i++){
		for(int j = i; j < n; j++){
			double sum_l = 0;

			for(int k = 0; k < i; k++)
				sum_l += A[j][k]*A[k][i];

			A[j][i] -= sum_l;
		}

		for(int j = i+1; j < n; j++){
			double sum_u = 0;

			for(int k = 0; k < i; k++)
				sum_u += A[i][k]*A[k][j];

			A[i][j] = (A[i][j] - sum_u)/A[i][i];
		}
	}
}

void solve_LU(double **LU_m, double *x, int n){
	for(int i = 0; i < n; i++){
		double sum = 0;
		for(int j = 0; j < i; j++)
			sum += LU_m[i][j]*x[j];
		x[i] = (x[i] - sum)/LU_m[i][i];
	}

	for(int i = n-1; i >=0; i--){
		double sum = 0;
		for(int j = i+1; j < n; j++)
			sum += LU_m[i][j]*x[j];
		x[i] -= sum;
	}
}