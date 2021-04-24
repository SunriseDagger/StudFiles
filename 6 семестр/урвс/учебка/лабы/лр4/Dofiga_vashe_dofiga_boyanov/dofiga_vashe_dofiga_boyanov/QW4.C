#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define COLF 50
#define NUMBERS 20
#define NUMB1 20
#define MAX 225

 int fd[2];
 struct stat st[2];
 char fn0[5]="pr0",fn1[5]="pr1",fn2[5]="pr2";
 char *a0="n ",*a1="n ",*a2="n ";
 char h[255];

void viv(void)
{
 printf("File name		%s		%s		%s\n",fn0,fn1,fn2);
 printf("Descript		%d		%d		%d\n",fd[0],fd[1],fd[2]);
 printf("Access			%s		%s		%s\n",a0,a1,a2);
 printf("Tabl opisatel   	     	     \n");
 printf("  	       		*______*	*______*	*______*\n");
 printf(" |device file  		%d   	%d   	%d\n",st[0].st_dev,st[1].st_dev,st[2].st_dev);
 printf(" |serial inode		%d   	%d   	%d\n",st[0].st_ino,st[1].st_ino,st[2].st_ino);
 printf(" |file MODE   		%d   	%d   	%d\n",st[0].st_mode,st[1].st_mode,st[2].st_mode);
 printf(" |links			%d		%d		%d\n",st[0].st_nlink,st[1].st_nlink,st[2].st_nlink);
 printf(" |user  ID		%d		%d		%d\n",st[0].st_uid,st[1].st_uid,st[2].st_uid);
 printf(" |group ID		%d		%d		%d\n",st[0].st_gid,st[1].st_gid,st[2].st_gid);
 printf(" |device ident		%d		%d		%d\n",st[0].st_rdev,st[1].st_rdev,st[2].st_rdev);
 printf(" |SIZE of file		%d		%d		%d\n",st[0].st_size,st[1].st_size,st[2].st_size);
 printf(" |last accessT		%d   	%d   	%d\n",st[0].st_atime,st[1].st_atime,st[2].st_atime);
 printf(" |last modifyT		%d   	%d   	%d\n",st[0].st_mtime,st[1].st_mtime,st[2].st_mtime);
 printf(" |last statusC		%d   	%d   	%d\n",st[0].st_ctime,st[1].st_ctime,st[2].st_ctime);
 printf("			_________	_________	_________\n\n\n\n\n\n");
 scanf("%s",h);
 printf("\n\n");
};

int main()
{
 int i;
 char c;
 unsigned int j;
 stat(fn0,&st[0]);
 stat(fn1,&st[1]);
 stat(fn2,&st[2]);
 fd[0]=-1;fd[1]=-1;fd[2]=-1;
 viv();
  {
   fd[0]=open(fn0,0);
   fstat(fd[0],&st[0]);
  a0="W";
   viv();
  };
  {
   fd[1]=open(fn1,0);
   fstat(fd[1],&st[1]);
   a1="R";
   viv();
  };
  {
   fd[2]=open(fn2,2);
   fstat(fd[2],&st[2]);
  a2="RW";
   viv();
  };

 close(fd[2]);
 fd[2]=creat(fn2,0);
 for (i=0;i<3;i++)
  fstat(fd[i],&st[i]);
 viv();
 for (j=0;j<st[1].st_size;j++)
  {
   read(fd[1],&c,1);
   write(fd[2],&c,1);
  }
 for (i=0;i<3;i++)
  fstat(fd[i],&st[i]);
 viv();
 return 0;
}
