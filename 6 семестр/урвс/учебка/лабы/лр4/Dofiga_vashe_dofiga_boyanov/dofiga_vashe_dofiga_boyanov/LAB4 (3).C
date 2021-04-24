#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#define max 100

main()
{int fd=0,fg,fd1;
 struct stat stbuf,stbuf2,stbuf3,stbuf4;
 char c;
 fstat(0,&stbuf);
 printf("\n \n \n Неявное открытие стандартного файла ввода и вывода \n");
 fstat(1,&stbuf2);
 printf(" таблица файлов процесса \n");
 printf("                 1            2 \n");
 printf("file name    com. input   com. output \n");
 printf("number inode     %d           %d \n",stbuf.st_ino,stbuf2.st_ino);
 printf("access mode    %X         %X \n",stbuf.st_mode,stbuf2.st_mode);
 printf("locate point     %d           %d \n",stbuf.st_dev,stbuf2.st_dev);
 printf(" \n таблица открытых файлов \n");
 printf("file name    com. input   com. output \n");
 printf("point to PTF    1              2 \n");
 fg=open("ddd.epl",O_CREAT);
 fstat(fg,&stbuf3);
 printf("\n ОТКРЫТИЕ ПОЛЬЗОВАТЕЛЬСКОГО ФАЙЛА \n");
 printf(" таблица файлов процесса \n");
 printf("                 1            2               3\n");
 printf("file name    com. input   com. output    ddd.epl\n");
 printf("number inode     %d           %d              %d\n",stbuf.st_ino,
  stbuf2.st_ino,stbuf3.st_ino);
 printf("access mode    %X         %X           %X\n",stbuf.st_mode,
  stbuf2.st_mode,stbuf3.st_mode);
 printf("locate point     %d           %d           %d\n",stbuf.st_dev,
  stbuf2.st_dev,stbuf3.st_mode);
 printf(" \n таблица открытых файлов \n");
 printf("file name    com. input   com. output   ddd.epl\n");
 printf("point to PTF    1              2            3\n");
 scanf("%c \n \n",&c);
 printf("ЗАКРЫТИЕ СТАНДАРТНОГО ВВОДА \n");
 printf(" таблица файлов процесса \n");
 printf("                   1            2\n");
 printf("file name     com. output    ddd.epl\n");
 printf("number inode       %d           %d\n",stbuf2.st_ino,stbuf3.st_ino);
 printf("access mode      %X         %X\n",stbuf2.st_mode,stbuf3.st_mode);
 printf("locate point       %d           %d\n",stbuf2.st_dev,stbuf3.st_mode);
 printf("\n таблица открытых файлов \n");
 printf("file name     com. output   ddd.epl\n");
 printf("point to PTF      1            2    \n \n");
 scanf("%c \n \n",&c);
 printf("МОДЕЛИРОВАНИЕ  dup \n");
 printf(" таблица файлов процесса \n");
 printf("                   1            2\n");
 printf("file name     com. output    ddd.epl\n");
 printf("number inode       %d           %d\n",stbuf2.st_ino,stbuf3.st_ino);
 printf("access mode      %X         %X\n",stbuf2.st_mode,stbuf3.st_mode);
 printf("locate point       %d           %d\n",stbuf2.st_dev,stbuf3.st_mode);
 printf("\n таблица открытых файлов \n");
 printf("file name     com. output   ddd.epl  copy_of_ddd.epl\n");
 printf("point to PTF      1            2           2\n \n");
 printf("ЗАКРЫТИЕ ПОЛЬЗОВАТЕЛЬСКОГО ФАЙЛА \n");
 close(fg);
 printf(" таблица файлов процесса \n");
 printf("                   1            2\n");
 printf("file name     com. output    ddd.epl\n");
 printf("number inode       %d           %d\n",stbuf2.st_ino,stbuf3.st_ino);
 printf("access mode      %X         %X\n",stbuf2.st_mode,stbuf3.st_mode);
 printf("locate point       %d           %d\n",stbuf2.st_dev,stbuf3.st_mode);
 printf("\n таблица открытых файлов \n");
 printf("file name     com. output   copy_of_ddd.epl\n");
 printf("point to PTF      1                2\n \n");
}