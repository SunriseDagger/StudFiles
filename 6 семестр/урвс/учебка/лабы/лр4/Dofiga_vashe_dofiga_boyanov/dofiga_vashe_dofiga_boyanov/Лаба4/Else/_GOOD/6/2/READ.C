/* read - program read numbers in binary format and
	prints them into the screen in text format */

#define INF 1e500 // stop number

int main()
{
    double f;
    int print;

    f = 0;
    for(;;)
    {
	print = 0;
	if(read(0, &f, sizeof(double)))
	    print = 1;
	if(f == INF)
	    break;
	if(print)
	    printf("f(x) = %lf\n", f);
    }
    close(0);
    return 0;
}
