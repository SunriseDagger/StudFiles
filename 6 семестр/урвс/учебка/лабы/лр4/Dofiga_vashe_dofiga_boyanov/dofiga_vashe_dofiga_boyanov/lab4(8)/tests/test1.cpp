#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[])
{
 if(!fork())
 {
    cout<<"child process"<<endl;
 }else
    {
    cout<<"main process"<<endl;
    }
    return 0;
}
