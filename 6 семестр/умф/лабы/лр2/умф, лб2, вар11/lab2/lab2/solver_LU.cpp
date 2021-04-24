#include "solver_LU.h"

void solve_LU(double* dia_0, double* dia_p, double* dia_m, double* rp, int n){

	for(int j = 0; j < n-1 ; j++){
		dia_p[j] /= dia_0[j];
		dia_0[j+1] -= dia_m[j]*dia_p[j];
	}

   rp[0] /= dia_0[0];
	for(int i = 1; i < n; i++)
		rp[i] = (rp[i] - dia_m[i-1]*rp[i-1])/dia_0[i];
	for(int i = n - 2; i >=0 ; i--)
		rp[i] -= dia_p[i]*rp[i+1];
}