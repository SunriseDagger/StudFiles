int main()
{
    	double f = 0;
    	int stop = 0;
	while(!stop)
    	{
		if(read(0, &f, sizeof(double)))
	    	{
			if (f != -1) printf("f(x) = %lf\n", f);
			else stop=1;
	   	}
    	}
	close(0);
    	exit(0);
}
