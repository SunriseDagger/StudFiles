#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
int factor(int n)
{
	int res=1;	
	while (n>0)
		{res*=n;n--;}
	return res;
};
int main()
{	int status,pid,n,r,i,j;
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
	cout<<"narr: ";
	for (i=0;i<r;i++)
		cout<<narr[i]<<" ";
	cout<<'\n';
	for(i=0;i<=r;i++)
	{
		if ((pid=fork()) == 0)
		{
		tmp[0]=i;
//		write(fd1[1],tmp,4);
		tmp[1]=factor(narr[i]);
//		cout<<"PRC:"<<i<<"narr: "<<narr[i]<<"fact: "<<tmp[0]<<'\n';
		write(fd1[1],tmp,8);
		exit(0);
		}
	}
//	for (i=0;i<=r;i++)
//		if (pid==0)	exit(0);
//	cin>>j;
	int k=0,nom;
	while ( !prz)
		{
	        read(fd1[0],tmp,8); nom=tmp[0];//cout<<"nom="<<nom<<'\n';
//		read(fd1[0],tmp,4);
		res[nom]=tmp[1];
	//	sleep(3);
		k++;
		if ( k == r+1)
			prz=true;
		}
	for (i=0;i<=r;i++)
		wait(&status);
	int rzr=res[r];
	for (i=0;i<r;i++)
		rzr/=res[i];
	cout<<'\n'<<"RESULT :"<<rzr<<'\n';
/*	if ((pid=fork()) == 0)
	{
	cout<<"I am child";
	}
	wait(&status);
*/
	return 0;
}
