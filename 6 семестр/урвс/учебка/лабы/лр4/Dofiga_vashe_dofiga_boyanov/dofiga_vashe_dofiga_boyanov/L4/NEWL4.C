#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dir.h>

#define bool int
#define true 1
#define false 0

main()
{
struct stat stbuf;
struct direct drbuf;
int i,fd,Kol,Nfile,Help;
off_t drsize;
bool Test;

if(stat(".",&stbuf)<0)
   {
   fprintf(stderr,"Stat error\n");
   return (-1);
   }
if((stbuf.st_mode & S_IFMT)!=S_IFDIR)
   {
   fprintf(stderr,"Strange mistake\n");
   return (-1);
   }

if((fd=open(".",O_RDONLY))<0)
  {
  fprintf(stderr,"Error in opening a current directory\n");
  return(-1);
  }
drsize=stbuf.st_size;
Kol=0;
Nfile=0;
Help=drsize/sizeof(struct direct);
do
{
Test=false;
Kol++;
for(i=0;i<Help;i++)
   {
    if((read(fd,&drbuf,sizeof(struct direct)))!=sizeof(struct direct))
      {
       fprintf(stderr,"Error in reading a current directory\n");
       return (-1);
      }
    if(drbuf.d_ino)
      {
       if(stat(drbuf.d_name ,&stbuf)<0)
	 {
	  fprintf(stderr,"Strange stat error\n");
	  return (-1);
	 }
       if(stbuf.st_nlink==Kol)
	 {
	  if(!Test)
	     {
	     printf("Number of references to files:%d\n",Kol);
	     Test=true;
	     }
	   printf("\t%s\n",drbuf.d_name);
	   Nfile++;
	 }
      }

   }
}while(Test||(Nfile!=Help));
close(fd);
return(0);
}