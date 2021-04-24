#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include<stdio.h>
struct stat buf;
int fd,t,k=0,h,dl,i;
int main()
{
 DIR *uk; 
 struct dirent *st;
 char *ima,ch1[1],ch2[]="/";
 char put[]="/export/home/stud/pm7204/tst2";
 
// f=open("result",2);
t:
 dl=strlen(put);
printf("\n%s\n",put);
uk=opendir(put);
if (uk!=NULL)

{
while(st=readdir(uk))
 {
 ima=st->d_name;
 stat(ima,&buf);
 if( (buf.st_mode&S_IFMT)== S_IFREG)
 {printf("\n %s",ima);}
 
 };
 closedir(uk);
};

i=dl-1;
ch1[0]=put[i];
while(ch1[0]!=ch2[0])
{i=i-1;
 ch1[0]=put[i];
};
put[i]='\0';

if(strcmp(put,"")>0) {goto t;}
else{ if(k==0) {put[i]='/';put[i+1]='\0'; k=1; goto t;}  }

return 0;
}