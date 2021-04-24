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
#include <string.h>
using namespace std;
ofstream out;

int main()
{	int sokett;
    sockaddr st_addr;
    in_addr addr;
	string key;
    char  buf[1000]; 
    //create socket
    cout << "Client: creating socket." << endl;
    if((sokett = socket(AF_UNIX, SOCK_STREAM, 0))==-1)
   	cout << "Client: Can't create socket." << endl, exit(1);
    cout<<"Client: succsessfully created socket."<<endl;	
    //initialization of struct sockaddr & in_addr
    st_addr.sa_family = AF_UNIX;
    addr.s_addr = INADDR_ANY;
    sprintf(st_addr.sa_data,"%s", "INADDR_ANY");
    //connection to server
    cout << "Client: Connecting to server." << endl;
    if (connect(sokett, &st_addr, sizeof(struct sockaddr))==-1)
    {	
		cout << "Client: Error conecting server." << endl;
		close(sokett);
		exit(1);
    }
    cout << "Client: Sucsessfully connected to server." << endl;
    //ask for  file path & mask
    cout << "Input key: " << endl;
			cin >> key;
    
    //sending to server information
    cout << "Client: Sending to server search file info." << endl;
    if (send(sokett, key, sizeof(char)*strlen(key),MSG_WAITALL)==-1)		// send path
    {	
		cout << "Client: Error sending info." << endl;
		close(sokett);
		exit(1);
    }
    
    cout << "Client: Send file info sucsessfully." << endl;
    if ((recv(sockid, buf, 1000*sizeof(char), 0))!=-1)				// receive file names
		cout << "Client: Received from server info. " << endl;
	out.open("output.txt");
	if(!out) cout << "Error in open file 'output.txt'" << endl;
	out << "key is " <<key << endl;
    out << buf;
    close(sokett);
    cout << "Client: Closed socket." << endl;
}
