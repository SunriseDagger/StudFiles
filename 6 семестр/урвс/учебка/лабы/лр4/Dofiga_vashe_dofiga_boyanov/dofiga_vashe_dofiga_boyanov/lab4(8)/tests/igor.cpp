


double integrate( int N, double* f(double), double MAX)
{
    int count =0;
    double a1, a2;
    for(int i=0;i<N;i++)
    {
	a1 = ((double)rand())/RAND_MAX;
	a2 = ((double)rand()*MAX)/RAND_MAX;
	if(a2<f(a1)) count++;
    }
    return ((double)count)/N;
}

int main()
{

}