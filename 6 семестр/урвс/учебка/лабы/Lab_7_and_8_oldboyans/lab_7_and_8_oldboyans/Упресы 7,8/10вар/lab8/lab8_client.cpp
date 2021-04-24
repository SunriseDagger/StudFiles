#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
using namespace std;
ofstream out;

int main()
{	int sockid;
    sockaddr st_addr;
    in_addr addr;
    char mask[30], path[30], buf[1000]; 
    //create socket
    cout << "Client: creating socket." << endl;
    if((sockid = socket(AF_UNIX, SOCK_STREAM, 0))==-1)
   	cout << "Client: Can't create socket." << endl, exit(1);
    cout<<"Client: succsessfully created socket."<<endl;	
    //initialization of struct sockaddr & in_addr
    st_addr.sa_family = AF_UNIX;
    addr.s_addr = INADDR_ANY;
    sprintf(st_addr.sa_data,"%s", "INADDR_ANY");
    //connection to server
    cout << "Client: Connecting to server." << endl;
    if (connect(sockid, &st_addr, sizeof(struct sockaddr))==-1)
    {	
		cout << "Client: Error conecting server." << endl;
		close(sockid);
		exit(1);
    }
    cout << "Client: Sucsessfully connected to server." << endl;
    //ask for  file path & mask
in_path:    cout << "Input file path: " << endl;
			cin >> path;
    if(!(opendir(path))) {cout << "It's not directory" << endl; goto in_path;}
    cout << "Input file mask: " << endl;
    cin >> mask;
    //sending to server information
    cout << "Client: Sending to server search file info." << endl;
    if (send(sockid, path, 30*sizeof(char),MSG_WAITALL)==-1)		// send path
    {	
		cout << "Client: Error sending info." << endl;
		close(sockid);
		exit(1);
    }
    if (send(sockid, mask, 30*sizeof(char),MSG_WAITALL)==-1)		// send mask
    {	
		cout << "Client: Error sending info." << endl;
		close(sockid);
		exit(1);
    }
    cout << "Client: Send file info sucsessfully." << endl;
    if ((recv(sockid, buf, 1000*sizeof(char), 0))!=-1)				// receive file names
		cout << "Client: Received from server info. " << endl;
	out.open("output.txt");
	if(!out) cout << "Error in open file 'output.txt'" << endl;
	out << "Path is " << path << endl;
    out << "Mask is " << mask << endl;
    out << buf;
    close(sockid);
    cout << "Client: Closed socket." << endl;
}
