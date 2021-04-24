#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
using namespace std;

int main()
{
	int sockid, sockid1;			// socked identificator
	struct sockaddr st_addr;
	char path[30], mask[30];
	char data[1000];
	int pipeFd[2];
	remove("INADDR_ANY");
	if ((sockid = socket(AF_UNIX,SOCK_STREAM,0)) == -1)		// creating socket
	cout << "Server: Error in creating socket." <<	endl, exit(1);
	//initialization of struct sockaddr
    st_addr.sa_family = AF_UNIX;
    sprintf(st_addr.sa_data,"%s", "INADDR_ANY");
    // binding socket
    if(bind(sockid, &st_addr, sizeof(struct sockaddr))==-1)
    {	
		cout << "Server: Error binding." << endl;
		close(sockid);
		exit(1);
    }
    // turn on TCP-connections
    cout << "Server: is trying to listen socket with id = " << sockid << endl;
    if(listen(sockid, 10)==-1)
    {	
		cout<<"Error listening for server1!"<<endl;
		close(sockid);
		exit(1);
    }
    cout << "Server: Listening." << endl;
    cout << "Server: Creating another socket for receiving messages." << endl;
    sockid1 = accept(sockid, 0, 0);
    cout << "Server: Created another socket with id = " << sockid1 << endl;
    cout << "Server: Waiting for a message from client." << endl;
	if(recv(sockid1, path, 30*sizeof(char), MSG_DONTROUTE))
		cout << "Server: Received from client info: " << path << endl;
	if(recv(sockid1, mask, 30*sizeof(char), MSG_DONTROUTE))
		cout<<"Server: Received from client info: " << mask << endl;   
	pipe(pipeFd);
	if (!fork())
	{	
		close(pipeFd[0]);
		close(1);
		dup(pipeFd[1]);
		close(pipeFd[1]);
		execlp("find", "find",path,"-name", mask, "-print",NULL);
		exit(0);
	}
	close(pipeFd[1]);
	for (int i = 0; ; i++)
	{	
		if (read(pipeFd[0], &data[i], 1) == 0)
		{	
			data[i] = '\0';		
	    	break;
		}
	}
	if(send(sockid1,data,sizeof(data),MSG_WAITALL)!=-1) 
		cout << "Server: Send data." << endl;            
    cout << "Server: Closing socket." << endl;
    close(sockid1);
    close(sockid);
    cout << "Server: Closed socket." << endl;
    return 0;
}
