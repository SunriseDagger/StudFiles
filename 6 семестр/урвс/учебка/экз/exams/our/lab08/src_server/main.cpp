/**
 * Resource management, Lab 08, Variant 02.
 * Chat-server.
 * @author      Alexandrov Mikhail
 * @author      Zhigalov Peter
 * @version     1.3
 */
#include "cserverchat.h"

/**
 * @brief main
 * @param argc Number of arguments
 * @param argv Arguments
 * @return if succses then 0, else number of error
 */
int main(int argc,char **argv)
{
    int port = 0;
    if(argc == 2)
        sscanf(argv[1], "%i", &port);
    else
        port = 2525;
    CServerChat chat(port);
    chat.run();
    return 0;
}
