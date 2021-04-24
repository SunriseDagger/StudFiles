#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/dirent.h>
#include<stdio.h>
#include<dirent.h>
 main()
{
  struct  DIR *dirp;
  struct  dirent *dp;
  dirp=opendir(".");
  dp=readdir(dirp);
  printf("%s\t%d",dp->d_name,dirp->d_size);
  closedir(dirp);
  return (0);
}