#include "one_d_search.h"


namespace one_d_search{

	int iter_dich;
	int iter_gold;
	int iter_Fib;
	FILE* f_out;
	void find_area(func f, double delta, double x0, double& a, double& b){
		double f0 = f(x0);
		double x1 = x0+delta;
		double f1 = f(x1);
		double h = delta;
		int k = 1;
	
		fprintf(f_out, "===================================\n");
		fprintf(f_out, "Find area:\n");

		if(f0 < f1){
			x1 = x0 - delta;
			h *= -1;
		} 

		bool end_cycle = false;

		while(!end_cycle){
			h *= 2;
			x0 = x1 + h;
			f0 = f(x0);

			fprintf(f_out, "%d\t%25.15lf\t%25.15lf\n", k, x0, x1);	
			k++;

			if(f1 > f0){
				x1 = x0;
				f1 = f0;
			}
			else{
				end_cycle = true;
				x1 = x0;
				x0 -= h + h/2;
			}

		};

		fprintf(f_out, "%d\t%25.15lf\t%25.15lf\n", k, x1, x0);	
		a = x1;
		b = x0;
	}

	double dichotomy(func f, double eps, double a, double b){
		fprintf(f_out, "===================================\n");
		fprintf(f_out, "Dich:\n");
		double x1;;
		double x2;
		double f1;
		double f2;
		iter_dich = 0;
		double l1 = b-a, l2;
		while(fabs(a-b)>2*eps){
		

			x1 = (a + b - eps)/2;
			x2 = (a + b + eps)/2;
			f1 = f(x1);
			f2 = f(x2);

			iter_dich += 2;


			if(f1 > f2){
				a = x1;
			}
			else
				b = x2;
			l2 = b - a;
			fprintf(f_out, "%d\t%25.15lf\t%25.15lf\t%25.15lf\n",iter_dich, a, b, l1/l2);
			l1 = l2;
		};
		return (a+b)/2;

	}

	double golden(func f, double eps, double a, double b){
		double x1, x2, f1, f2;
		const double mul = (3-sqrt((double)5.0))/2;
		int point_num;

		//fprintf(f_out, "===================================\n");
	//	fprintf(f_out, "Golden:\n");

		iter_gold = 2;

		x1 = a + mul*(b-a);
		x2 = b - mul*(b-a);
		f1 = f(x1);
		f2 = f(x2);

		double l1 = b-a, l2;

		if(f1 < f2){
			b = x2;
			x2 = x1;
			f2 = f1;
			point_num = 1;
		}
		else{
			a = x1;
			x1 = x2;
			f1 = f2;
			point_num = 2;
		}

		
		l2 = b - a;
		//fprintf(f_out, "%d\t%25.15lf\t%25.15lf\t%25.15lf\n",iter_gold , a, b, l1/l2);
		l1 = l2;

		while(fabs(a-b)>eps){
			iter_gold++;
			switch(point_num){
				case 1:{
					x1 = a + mul*(b-a);
					f1 = f(x1);
					   }break;
				case 2:{
					x2 = b - mul*(b-a);
					f2 = f(x2);
					   }break;
			};

			if(f1 < f2){
				b = x2;
				x2 = x1;
				f2 = f1;
				point_num = 1;
			}
			else{
				a = x1;
				x1 = x2;
				f1 = f2;
				point_num = 2;
			}

			l2 = b - a;

		//fprintf(f_out, "%d\t%25.15lf\t%25.15lf\t%25.15lf\n",iter_gold , a, b, l1/l2);
		l1 = l2;
		}
		return (a+b)/2;

	}

	double Fib(func f, double eps, double a, double b){
		double x1, x2, f1, f2;
		double fib_max = (b-a)/eps;

		int add_fib;
		int n = 2;

		int point_num;

		fprintf(f_out, "===================================\n");
		fprintf(f_out, "Fib:\n");


		vector<long long int> fib_numbers;
		fib_numbers.push_back(1);
		fib_numbers.push_back(1);

		do{
			add_fib = fib_numbers[n-1] + fib_numbers[n-2];
			fib_numbers.push_back(add_fib);
			n++;
		}while(fib_max > add_fib);

		n = fib_numbers.size() - 3;

		double l1 = b-a, l2;
		iter_Fib = 1;


		x1 = a + fib_numbers[n]*(b-a)/fib_numbers[n+2];
		x2 = a + b - x1;
		f1 = f(x1);
		f2 = f(x2);

		if(f1 < f2){
			b = x2;
			x2 = x1;
			f2 = f1;
			point_num = 1;
		}
		else{
			a = x1;
			x1 = x2;
			f1 = f2;
			point_num = 2;
		}

		l2 = b - a;
		fprintf(f_out, "%d\t%25.15lf\t%25.15lf\t%25.15lf\n",iter_Fib , a, b, l1/l2);
		l1 = l2;



		const int true_iters = n;

		for(int k = 1; k < true_iters; k++){
			iter_Fib++;
			switch(point_num){
				case 1:{
					x1 = a + fib_numbers[n-k+1]*(b-a)/fib_numbers[n-k+3];
					f1 = f(x1);
					  }break;
				case 2:{
					x2 = a + fib_numbers[n-k+2]*(b-a)/fib_numbers[n-k+3];
					f2 = f(x2);
					  }break;
			};

			if(f1 < f2){
				b = x2;
				x2 = x1;
				f2 = f1;
				point_num = 1;
			}
			else{
				a = x1;
				x1 = x2;
				f1 = f2;
				point_num = 2;
			}

			l2 = b - a;
			fprintf(f_out, "%d\t%25.15lf\t%25.15lf\t%25.15lf\n",iter_Fib, a, b, l1/l2);
			l1 = l2;
		}

		return (a+b)/2 ;
	}
};