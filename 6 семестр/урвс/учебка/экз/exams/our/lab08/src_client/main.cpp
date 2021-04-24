/**
 * Resource management, Lab 08, Variant 02.
 * Chat-client.
 * @author      Alexandrov Mikhail
 * @author      Zhigalov Peter
 * @version     1.3
 */
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <thread>
#include <string.h>
#include <unistd.h>

using namespace std;

/**
 * @brief sendmessage Send message to server
 * @param sock Socket descriptor
 */
void sendmessage(const int& sock)
{
    string message;
    while(1)
    {
        getline(cin, message);
        if(message.size())
        {
            message.append("\n\0");
            if(message == "\\cmd exit\n\0")
                exit(0);
            send(sock, message.c_str(), sizeof(char) * message.size(), 0);
        }
    }
}

/**
 * @brief main
 * @param argc Number of arguments
 * @param argv Arguments
 * @return if succses then 0, else number of error
 */
int main(int argc, char **argv)
{
    int sock;   /**< Socket descriptor*/
    struct sockaddr_in addr;
    struct hostent *hostInfo;   /**< Information about server*/
    unsigned short int serverPort;  /**< Server port*/
    string buf, Nick;

    if(argc == 4)
    {
        buf = argv[1];
        sscanf(argv[2], " %i", &serverPort);
        Nick = argv[3];
    }
    else
    {
        buf = "localhost";
        serverPort = 2525;
        Nick = "Test";
    }
    hostInfo = gethostbyname(buf.c_str());
    Nick.append("\n\0");

    sock = socket(AF_INET, SOCK_STREAM, 0); /** Create socket*/
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = hostInfo->h_addrtype; /** AF_INET family*/
    addr.sin_port = htons(serverPort);
    memcpy((char *) &addr.sin_addr.s_addr,
           hostInfo->h_addr_list[0], hostInfo->h_length);

    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)   /** Connect to server*/
    {
        perror("connect");
        exit(2);
    }

    send(sock, Nick.c_str(), sizeof(char) * Nick.size(), 0);    /** Send Nickname to server*/
    buf.clear();
    std::thread th(sendmessage,sock);   /** Create new thread for send messages*/
    char message[1024];
    int len = 0;
    bool lost = false;
    while(!lost)    /** Loop for recive message*/
    {
        buf.clear();
        len = recv(sock, message, sizeof(message), 0);
        if(len <= 0)
        {
            cout << endl <<"Connection lost...";
            lost = true;
        }
        else
        {
            message[len] = '\0';
            buf.append(message);
            if(buf.size())
                cout << buf;
        }
    }
    close(sock);
    return 0;
}
