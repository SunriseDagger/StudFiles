#include <windows.h>

#include "Broyden.h"

//Квадратичная функция
double func(vect_2d arg){
	return arg.x * arg.x - arg.x * arg.y + 3 * arg.y  *arg.y;
}

vect_2d grad_func(vect_2d arg){
	return vect_2d(2*arg.x - arg.y, 6*arg.y - arg.x); 
}

//Функция Розенброка
double func1(vect_2d arg){
	return 100*(arg.y - arg.x*arg.x)*(arg.y - arg.x*arg.x) + (1-arg.x)*(1-arg.x);
}

vect_2d grad_func1(vect_2d arg){
	return vect_2d(400*arg.x*arg.x*arg.x - 400*arg.x*arg.y + 2*arg.x - 2, 200*(arg.y - arg.x*arg.x)); 
}

//Функция из варианта: -3/(1 + ((x-3)/2)^2 + (y-2)^2) - 3/(1 + (x-1)^2 + ((y-2)/3)^2)
double func2(vect_2d arg){
	return (-3.0/(1.0 + (arg.x-3)*(arg.x-3)/4.0 + (arg.y-2)*(arg.y-2)) - 3.0/(1.0 + (arg.x-1)*(arg.x-1) + (arg.y-2)*(arg.y-2)/9.0));
}

vect_2d grad_func2(vect_2d arg){
	double x = arg.x, y = arg.y;
	double denom1 = (1.0 + (x-1)*(x-1)+(y-2)*(y-2)/9.0)*(1.0 + (x-1)*(x-1)+(y-2)*(y-2)/9.0);
	double denom2 = 2*(1.0 + (x-3)*(x-3)/4.0 + (y-2)*(y-2))*(1.0 + (x-3)*(x-3)/4.0 + (y-2)*(y-2));
	double grad_x = 6.0*(x-1)/denom1 + 3.0*(x-3)/denom2;
	double grad_y = 2.0*(y-2)/3.0 * (1/denom1 + 9*denom2);
	return vect_2d(grad_x, grad_y);
}


int main(){

	Broyden our_meth;
	our_meth.init(func2, grad_func2, 1E-6, 40000);
	vect_2d start(10, -8);
	vect_2d min;

	our_meth.minimization(start, min);

	printf("%.15lf\t%.15lf\n%.15lf\n",min.x, min.y, func2(min));
	system("pause");

}