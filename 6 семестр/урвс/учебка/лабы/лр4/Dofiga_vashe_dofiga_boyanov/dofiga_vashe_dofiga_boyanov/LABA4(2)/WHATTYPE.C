#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
typeOf( name ) char *name;
{  int type; struct stat st;
   if( stat( name, &st ) < 0 ){
           printf( "File %s does not exist\n", name );
           return 0;
   }
   printf("File has %d names\n", st.st_nlink);
   switch(type = (st.st_mode & S_IFMT)){
   case S_IFREG:
        printf( "Normal file of %ld bytes\n",
                   st.st_size ); break;
   case S_IFDIR:
           printf( "Directory\n" );      break;
   case S_IFCHR:   /* byte-oriented */
   case S_IFBLK:   /* block-oriented */
           printf( "device" );   break;
   case S_IFIFO:
           printf( "FIFO-file" );    break;
   default:
           printf( "Other type\n" );   break;
   }       return type;
 }
int main(int num,char *str[])
{ if (num==2) typeOf(str[1]); 
         else puts("1 parameter required");
  return 0;
}
     