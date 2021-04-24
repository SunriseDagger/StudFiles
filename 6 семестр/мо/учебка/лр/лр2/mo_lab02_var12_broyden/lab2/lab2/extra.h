#pragma once

#include <math.h>

//Двухмерный вектор и матрица

struct vect_2d{
	double x, y;
	vect_2d(){
	}
	vect_2d(double s_x, double s_y){
		x = s_x; y = s_y;
	}
	vect_2d operator+ (vect_2d op2){
		return vect_2d(x+op2.x, y+op2.y);
	}

	vect_2d operator- (vect_2d op2){
		return vect_2d(x-op2.x, y-op2.y);
	}

	double operator* (vect_2d op2){
		return x*op2.x + y*op2.y;
	}
		
	vect_2d operator* (double op2){
		return vect_2d(op2*x, op2*y);
	}


	double norm(){
		return sqrt(x*x + y*y);
	}
};

struct matrix_2d{
	vect_2d col1, col2;
	matrix_2d(){
	};
	matrix_2d(vect_2d s_c1, vect_2d s_c2){
		col1 = s_c1; col2 = s_c2;
	}
	matrix_2d(double a11, double a12, double a21, double a22){
		col1 = vect_2d(a11, a21); col2 = vect_2d(a12, a22);
	}
	
	vect_2d operator * (vect_2d op2){
		return vect_2d(col1.x*op2.x + col2.x*op2.y, col1.y*op2.x + col2.y*op2.y);
	}

	matrix_2d operator* (matrix_2d op2){
		vect_2d n_col1 = vect_2d(col1.x * op2.col1.x + col2.x*op2.col1.y, col1.x * op2.col2.x + col2.x*op2.col2.y);
		vect_2d n_col2 = vect_2d(col1.y * op2.col1.x + col2.y*op2.col1.y, col1.y * op2.col2.x + col2.y*op2.col2.y);
		return matrix_2d(n_col1, n_col2);
	}

	matrix_2d operator* (double op2){
		return matrix_2d(col1*op2, col2*op2);
	}

	matrix_2d operator+(matrix_2d op2){
		return matrix_2d(col1+op2.col1, col2+op2.col2);
	}
};