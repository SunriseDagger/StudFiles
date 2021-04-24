#ifndef CSERVERCHAT_H
#define CSERVERCHAT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "Csock.h"
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

class CServerChat
{
private:
    int listener;
    struct sockaddr_in addr, clientAddress;
    socklen_t clientAddressLength;
    struct hostent *hst;
    Csock clients;
    int ServerPort;
    fd_set readset;
    timeval timeout;
    char buf[1024];

    void NewUser(); /**< Add new user*/
    void ReadAndTransfer(); /**< Retranslate data*/
    void cleararr(char *m, int n);  /**< Clear static array*/
    int sendall(int s, char *buf, int len, int flags); /**< Send all data*/


public:

    CServerChat(int _ServerPort);   /**< Constructor*/
    ~CServerChat(); /**< Destructor*/
    void run(); /**< Run the server*/
};

#endif // CSERVERCHAT_H
