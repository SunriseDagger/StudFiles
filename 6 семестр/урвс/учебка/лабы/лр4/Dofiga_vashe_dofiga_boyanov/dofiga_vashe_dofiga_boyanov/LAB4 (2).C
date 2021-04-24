#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
//#include <strings.h>
#include <string.h>
#include <stdlib.h>

DIR *dire,*dirp;
char *new_name;
struct stat st,st1;
struct dirent *dr,*dr1;
int f=2;

//*********************************************************
/*** Формирование полного имени очередного подкаталога ***/
void filename(char * str)
{
new_name = (char *)malloc(256);
strcpy(new_name,str);
strcat(new_name,"/");
strcat(new_name,dr->d_name);
strcat(new_name,"/");
strcat(new_name,dr1->d_name);
}
//*******************************************************

void main(int argc,char* argv[])
{
if (argc != 2)
    {
       printf("\n ERROR! Не указан начальный каталог ! ");
       exit(1);
    };

dire=opendir(argv[1]); //открываем начальный каталог

while((dr=readdir(dire))!=NULL) //пока он не пуст
  {
   f=2;
   if ((strcmp(dr->d_name,".")==0) || (strcmp(dr->d_name,"..")==0)) continue;
   if(stat(dr->d_name,&st)<0) continue;

   if((st.st_mode&S_IFMT)==S_IFDIR) //это каталог
     {
      f=1;
      dirp=opendir(dr->d_name);  //открыть его

      while(((dr1=readdir(dirp))!=NULL) && (f==1))
// пока каталог не пуст и в нем не встретили подкаталога
	 {
	  filename(argv[1]);
	  if((strcmp(dr1->d_name,".")==0) || (strcmp(dr1->d_name,"..")==0))
	       continue;

	  if(stat(new_name,&st1)<0) continue;

	  if((st1.st_mode&S_IFMT)==S_IFDIR) f=0;

	 }; // end while2
      closedir(dirp);
      free(new_name);
     };  // end if

   if(f==1)  printf("\n %s\n",dr->d_name);
  }; //end while1
  closedir(dire);
}