#include "cserverchat.h"

/**
 * @brief CServerChat::CServerChat
 * @param _ServerPort Number of server port
 */
CServerChat::CServerChat(int _ServerPort)
{
    cout << "Starting server..." << endl;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }

    fcntl(listener, F_SETFL, O_NONBLOCK);

    addr.sin_family = AF_INET;
    ServerPort = _ServerPort;
    addr.sin_port = htons(ServerPort);
    addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }
    /** Queue of users = 5 */
    listen(listener, 5);
    clients.clear();
    /** Timeout*/
    timeout.tv_sec = 15;
    timeout.tv_usec = 0;
}

/**
 * @brief CServerChat::cleararr Clear static array
 * @param m Pointer to static array
 * @param n Number elements of array
 */
void CServerChat::cleararr(char *m, int n)
{
    for(int i = 0; i < n; i++)
        m[i] ='\0';
}

/**
 * @brief CServerChat::sendall Send all data
 * @param s Socket descriptor
 * @param buf Pointer for data buffer
 * @param len Number of read elements
 * @param flags Flags for function send
 * @return If error -1, else number of read bytes
 */
int CServerChat::sendall(int s, char *buf, int len, int flags)
{
    int total = 0;
    int n;

    while(total < len)
    {
        n = send(s, buf+total, len-total, flags);
        if(n == -1)
        {
            break;
        }
        total += n;
    }

    return (n==-1 ? -1 : total);
}

/**
 * @brief CServerChat::NewUser Add new user
 */
void CServerChat::NewUser()
{
    /** New request for connection, use accept*/
    clientAddressLength = sizeof(clientAddress);
    int sock = accept(listener, (struct sockaddr *) &clientAddress, &clientAddressLength);
    if(sock < 0)
    {
        perror("accept");
        exit(3);
    }
    hst = gethostbyaddr((char *) &clientAddress.sin_addr.s_addr,4, AF_INET);
    /** Show clients information*/
    printf("+%s [%s] new connect!\n", (hst) ? hst->h_name : "", inet_ntoa(clientAddress.sin_addr));
    fcntl(sock, F_SETFL, O_NONBLOCK);
    cleararr(buf, sizeof(buf));
    int bytes_read = 0;
    while(bytes_read <= 0)
        bytes_read = recv(sock, buf, sizeof(buf), 0);   /** Read Nickname*/
    char _ip[4];
    memcpy(_ip, &(clientAddress.sin_addr.s_addr), 4);
    clients.insert(sock,buf, _ip, clientAddress.sin_port, (hst)?hst->h_name:""); /** Information of host*/
    cout << "New user: " << buf << endl;
    for(int it1 = 0; it1 < clients.total; it1++)
    {
        if(clients.client[it1].sock != sock)
        {
            sendall(clients.client[it1].sock, "Join:", 5, 0);
            sendall(clients.client[it1].sock, buf, sizeof(buf), 0);
            cout << "+ " << buf << endl;
        }
    }
    buf[0] = '\0';
}

/**
 * @brief CServerChat::ReadAndTransfer Retranslate data
 */
void CServerChat::ReadAndTransfer()
{
    for(int it1 = 0; it1 < clients.total; it1++)
    {
        if(FD_ISSET(clients.client[it1].sock, &readset))
        {
            cleararr(buf, sizeof(buf));
            /** New clients data, read them*/
            int bytes_read = recv(clients.client[it1].sock, buf, sizeof(buf), 0);
            if(bytes_read <= 0)
            {
                /** Connection refused, delete socket*/
                buf[0] = '\0';
                close(clients.client[it1].sock);
                strcpy(buf, clients.client[it1].Name.c_str());
                clients.erase(clients.client[it1].sock);
                for(int it = 0; it < clients.total; it++)
                {
                    sendall(clients.client[it].sock, "Left:", 5, 0);
                    sendall(clients.client[it].sock, buf, sizeof(buf), 0);
                    cout << "- " << buf << endl;
                }
                cleararr(buf, sizeof(buf));
            }
            else
            {
                /** Send data to clients*/
                cout << buf << endl;
                char k[15],j(0);
                while(clients.client[it1].Name[j] != '\0')
                {
                    k[j] = clients.client[it1].Name[j];
                    j++;
                }
                k[j-1] = ':';
                k[j] = '\0';
                string answer = buf;
                int a = 0;
                while((a = answer.find('\n', a)) >= 0 && answer[a] != answer[answer.size()-1])
                {
                    a++;
                    answer.insert(a,k);
                }
                for(int it = 0; it < clients.total; it++)
                {
                    sendall(clients.client[it].sock, k, strlen(k), 0);
                    sendall(clients.client[it].sock, (char*)answer.c_str(), sizeof(char) * answer.size(), 0);
                    cout << "= " << buf << endl;
                }
                answer.clear();
                cleararr(buf, sizeof(buf));
            }
        }
    }
}

/**
 * @brief CServerChat::run Run the server
 */
void CServerChat::run()
{
    cout << "Server start at port:" << ServerPort <<endl;
    while(1)
    {
        /** Fill socket set*/
        FD_ZERO(&readset);
        FD_SET(listener, &readset);

        for(int it = 0; it < clients.total; it++)
            FD_SET(clients.client[it].sock, &readset);

        /** Wait socket event*/
        int mx = max(listener,clients.MaxSock);
        if(select(mx+1, &readset, NULL, NULL, &timeout) < 0)
        {
            perror("select");
            exit(3);
        }

        /** Determinate type of event and act*/
        if(FD_ISSET(listener, &readset))
            NewUser();
        ReadAndTransfer();
    }
}

/**
 * @brief CServerChat::~CServerChat
 */
CServerChat::~CServerChat()
{
    for(int it = 0; it < clients.total; it++)
        close(clients.client[it].sock);
    close(listener);
}
