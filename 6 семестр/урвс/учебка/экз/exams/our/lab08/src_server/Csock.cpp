#include "Csock.h"

/**
 * @brief Csock::Csock
 */
Csock::Csock()
{
    total = 0;
    MaxSock = 0;
}

/**
 * @brief Csock::insert Insert client profile in database
 * @param nsock Socket descriptor
 * @param name User nickname
 * @param ip User ip address
 * @param port User port
 * @param uhost Name of user host
 */
void Csock::insert(int nsock,string name, char* ip, int port,string uhost)
{
    client[total].sock = nsock;
    client[total].Name = name;
    memcpy(client[total].ip, ip, 4 * sizeof(char));
    client[total].host = uhost;
    client[total].port = port;
    total++;
    if(nsock > MaxSock)
        MaxSock = nsock;
}

/**
 * @brief Csock::clear Clear all database
 */
void Csock::clear()
{
    total = 0;
    MaxSock = 0;
}

/**
 * @brief Csock::erase Erase client
 * @param nsock Socket descriptor
 * @return If succses 1, else 0
 */
int Csock::erase(int nsock)
{
    if(!total)
        return 0;
    int i(0);
    while(client[i].sock != nsock && i < total+1)
        i++;
    if(i == total + 1)
        return 0;
    total--;
    if(i != total)
    {
        client[i].sock = client[total].sock;
        client[i].Name = client[total].Name;
        memcpy(client[i].ip, client[total].ip, 4 * sizeof(char));
        client[i].host = client[total].host;
        client[i].port = client[total].port;
    }
    if(nsock == MaxSock)
    {
        MaxSock = 0;
        for(int j = 0; j < total; j++)
            if(client[j].sock > MaxSock)
                MaxSock = client[j].sock;
    }
    return 1;
}

