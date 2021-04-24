/**
 * Program for view files list rom current directory to root directory
 * 
 * @author      Alexandrov Mikhail
 * @author      Zhigalov Peter
 * @version     0.6
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

/**
 * Check name of file on extension
 * 
 * @param  file  name of file
 * @param  ext   extension
 * @return       1 - if file contains extension, 0 - not contains
 */
int isExt(char* file, char* ext)
{
    int i = strlen(file) - 1;
    int j = strlen(ext) - 1;
    while(j != -1) {
        if (file[i] != ext[j])
            return 0;
        i--;
        j--;
    }
    if(file[i] == '.' && file[i-1] != '/')
        return 1;
    else
        return 0;
}

/**
 * Main function
 * 
 * @param  argc  number of program arguments
 * @param  argv  program arguments
 * @return       1, 2 or 3 - incorrect parameters, 0 - all correct
 */
int main(int argc,char **argv)
{
    if(argc != 2) {
        fprintf(stderr,"Incorrect number of arguments! Must be 1.\n");
        return 1;
    }
    char* ext;  // name of extension
    ext = argv[1];
    DIR *CurrDir, *PrevDir = NULL;
    CurrDir = opendir("./"); // open current directory
    if(CurrDir == NULL) { // directory not exist
        fprintf(stderr,"Directory not exist.\n");
        return 2;
    }
    struct dirent *CurrFile, *Next = NULL, *Prev = NULL;
    do { // while not root directory
        while ((CurrFile = readdir(CurrDir)) != 0) { // read all files in directory
            if(isExt(CurrFile->d_name, ext))    // if file contains extension
                printf("%s\n", CurrFile->d_name);
            if(CurrFile->d_name[0] == '.') {
                if(CurrFile->d_name[1] == '\0')
                    Prev = CurrFile;
                if(CurrFile->d_name[1] == '.')
                    Next = CurrFile;
            }
        }
        // change current directory
        closedir(PrevDir);
        PrevDir = CurrDir;
        chdir("..");
        CurrDir = opendir("./");
        if(CurrDir == NULL) { // directory not exist
            fprintf(stderr,"Directory not exist.\n");
            return 3;
        }
    }
    while( Prev->d_ino != Next->d_ino);
    closedir(CurrDir);
    return 0;
}

