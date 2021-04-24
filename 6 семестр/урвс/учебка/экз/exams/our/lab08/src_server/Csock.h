#ifndef CSOCK_H
#define CSOCK_H

#include <string>
#include <string.h>

using namespace std;

struct cl
{
    int sock;
    string Name;
    char ip[8]; /**< IPv4 address*/
    string host;
    int port;
};

class Csock
{
public:
    int total;  /**< Number of clients*/
    cl client[1024];    /**< Clients database*/
    Csock();    /**< Constructor*/

    void insert(int nsock, string name, char *ip, int port, string uhost);  /**< Insert client profile in database*/
    void clear();   /**< Clear all database*/
    int erase(int nsock);   /**< erase client*/
    int MaxSock;    /**< Maximal number of socket*/
};

#endif // CSOCK_H

