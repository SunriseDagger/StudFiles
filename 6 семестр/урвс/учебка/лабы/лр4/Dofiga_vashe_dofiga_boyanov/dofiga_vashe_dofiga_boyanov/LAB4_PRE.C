#include <machine/types.h>
#include <iomanip.h>
#include <sys/dir.h>
#include <sys/stat.h>
//#include <dirent.h>

const MAX_LENGTH_OF_DIR_NAME  = 255;
const TEST_DIR                = 16384;

int main(int argc, char *argv[])
{
  struct stat stbuf;              // structure for system call stat
  struct dirent **NameList;       // name files in dir
  int d_flag, nEntr;              // test on dir & num of entries in dir
  
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << "  <Listing dir>" << endl;
    exit(-1);
  };

  if (open(argv[1], 0) < 0) {
    cerr << "Error directory" << endl;
    exit(-2);
  };

  if (stat(argv[1], &stbuf)<0){
    cerr << "Error in system call stat()" << endl;
    exit(-3);
  };

  if ((d_flag = stbuf.st_mode & 16384) == 0 ) { 
    cerr << "This not directory" << endl;
    exit(-4);  
  };
    /* Scanning directory */
  if ((nEntr=scandir(argv[1], &NameList, 0, alphasort)) < 0) {
    cerr << "Error scanning directory" << endl;
    exit(-5);
  };
    /* Change directory to work dir*/
  if (chdir(argv[1]) < 0) {
    cerr << "Enter permission denied or bad dir" << endl;
    exit(-6);
  };
   /* Basic program */
  for (int i=0; i<=nEntr-1; i++) {
    if (stat(NameList[i]->d_name, &stbuf) < 0) {     // info about entries
     cerr << "Error in system call stat() in listing dir" << endl;
     exit(-7);
    };
    if ((d_flag = stbuf.st_mode & TEST_DIR) == 0 )  // if not dir -> cout
      cout << NameList[i]->d_name << endl;
  };
  return 0;                               // Normal end
}
  
