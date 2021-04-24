#include <iostream.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
int factor(int n)
{
	int res=1;	
	while (n>0)
		{res*=n;n--;}
	return res;
};
int main()
{	int status,pid,n,r,i,j;
	char c,b,nmb;
	int fd1[2];
	int narr[10],res[10]={0},tmp[2];
	bool prz=false,ok;
	int fd;
	cout<<"Input n: ";cin>>n;
	cout<<"Input r: ";cin>>r;
	int ost=n;
	for (i=0;i<r;i++)
		{
		ok=false;
//		if (i==r-1) {narr[i]=ost;ok=true;}	
		while (!ok)
			{
			cout<<"Input n("<<i<<") , 0<=n("<<i<<")<="<<ost<<'\n';
			cin>>narr[i];
			if ((narr[i]>-1)&&(narr[i]<ost+1)) ok=true;
			}
			ost-=narr[i];
		}
	narr[r]=n;
	pipe(fd1);

	cout<<'\n';
	for(i=0;i<=r;i++)
	{
		if ((pid=fork()) == 0)
		{
	 	 c=narr[i];
		 b=fd1[1];
		 nmb=i;
		char *const  argv[]={"fa.cpp.exe",&c,&b,&nmb,(char *)0};
//		execl("fa.cpp.exe","fa.cpp.exe",&c,&b,nmb,(char*) 0);
		execv("fa.cpp.exe",argv);
		exit(0);
		}
		else wait(&status);
	}

 if (pid!=0)
 {	
	int k=0,nom;
	while ( !prz)
		{
	        read(fd1[0],tmp,8); nom=tmp[0];
		res[nom]=tmp[1];
		k++;
		if ( k == r+1)
			prz=true;
		}
        int rzr=res[r];
	for (i=0;i<r;i++)
		rzr/=res[i];
	cout<<'\n'<<"RESULT :"<<rzr<<'\n';

}	
	return 0;
}
