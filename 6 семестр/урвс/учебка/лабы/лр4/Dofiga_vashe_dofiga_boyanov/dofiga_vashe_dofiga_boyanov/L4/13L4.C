#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dirent.h>
#include<dirent.h>
#include<stdio.h>
#include<string.h>
#define MaxLink 8

char *malloc();
typedef struct{
int NLink;
char *Tmp;
//char *InAdd;
int Counter;
}Work;
Work WorkList[MaxLink];
char *Linefeed = "\n";

ini()
{
int i;
for(i=0;i<MaxLink;i++)
   {
    WorkList[i].NLink=0;
    WorkList[i].Tmp=NULL;
    WorkList[i].Counter=16*(16-2*i);
   }
return (0);
}
prt()
{
 int i,j;
 for(i=0;i<MaxLink;i++)
   if(WorkList[i].Tmp!=NULL)
     {
     printf("Amount of link :%d\n",i);
     for(j=0;j<16*(16-2*i)-WorkList[i].Counter;j++)
       printf("%c",WorkList[i].Tmp[j]);
     }  
 return (0);
}
int H;
add(char *Name,int Lnk)
{
if(WorkList[Lnk].NLink==0)
  {
  WorkList[Lnk].NLink=Lnk;
  if((WorkList[Lnk].Tmp=malloc(16*(16-2*Lnk)))==NULL)
    {
     fprintf(stderr,"Not enough memory to allocatr buffer\n");
     return(-1);
    }
 }
H=strlen(Name);
if(H>WorkList[Lnk].Counter)
  {
  fprintf(stderr,"Ceteloge too much %d\n",Lnk);
  return (-1);
  } 
WorkList[Lnk].Counter-=H+1;
strcat(WorkList[Lnk].Tmp,Name);
strcat(WorkList[Lnk].Tmp,Linefeed);
return (1);
}


main(argc,argv)
int argc;
char **argv;
{
struct stat stbuf;
struct  DIR *dirp;
struct  dirent *dp;

int i;
char *Name;
    if(argc!=2)
      Name=".";
    else Name=argv++;
if(stat(Name,&stbuf)<0)
   {
   fprintf(stderr,"Stat error\n");
   return (-1);
   }
if((stbuf.st_mode & S_IFMT)!=S_IFDIR)
   {
   fprintf(stderr,"Strange mistake\n");
   return (-1);
   }
dirp=opendir(Name);
ini();


while((dp=readdir(dirp))!=NULL)
   {
     if(dp->d_ino)
       {
       if(stat(dp->d_name ,&stbuf)<0)
	     {
	       fprintf(stderr,"Strange stat error\n");
	       return (-1);
	     }
//       printf("%d\t%s\n",stbuf.st_nlink,dp->d_name);	     
       if(add(dp->d_name,stbuf.st_nlink)<0)
	  return (-1);   
       }

   }
closedir(dirp);
prt();
return (0);
}
