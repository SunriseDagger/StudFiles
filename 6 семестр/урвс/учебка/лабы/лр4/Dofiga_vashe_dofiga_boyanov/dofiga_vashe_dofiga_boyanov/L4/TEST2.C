#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
//#include<sys/dirent.h>
#include<stdio.h>
#include<string.h>

typedef
  struct {
    ino_t inod;
    char  s[14];
  } direct;
  
main()
{
char buf[]={"lena"};
 direct  item;
//  memset(item.s,'$',14);
  strncpy(item.s,buf,14);
  int dir = open(".",0);
  read(dir, item, sizeof(direct));
  printf("%s\n",item.s);
  close(dir);
  return (0);
}