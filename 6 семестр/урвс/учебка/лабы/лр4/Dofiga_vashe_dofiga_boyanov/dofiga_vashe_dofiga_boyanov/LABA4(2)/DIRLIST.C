/*  Here's a simple program that prints the names of the entries in the
    current working directory (alphabetically sorted):
*/
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include<string.h>
#define MAXNUM 100
int main()
{ DIR *dp;
  struct dirent *ep;
  char *list[MAXNUM];
  int i,count,num[MAXNUM];
  void sort(char *list[],int num[],int count);
  dp = opendir ("./");
  if (dp != NULL)
  { count=0;
    while (ep = readdir (dp))
    { list[count]=strdup(ep->d_name);
      count++;      
    }
    closedir (dp);
    for (i=0;i<count;i++) num[i]=i;
    sort(list,num,count);
    for (i=0;i<count;i++) puts(list[num[i]]);
    for (i=0;i<count;i++) free(list[i]);
  }
  else puts ("Couldn't open the directory.");
  return 0;
}
void sort(char *list[],int num[],int count)
{ int i,j,best,tmp;
  for (i=count-1;i>0;i--)
  { best=0;
    for (j=1;j<=i;j++)
      if (strcmp(list[num[j]],list[num[best]])>0) best=j;
    tmp=num[i]; num[i]=num[best]; num[best]=tmp;
  }
}
