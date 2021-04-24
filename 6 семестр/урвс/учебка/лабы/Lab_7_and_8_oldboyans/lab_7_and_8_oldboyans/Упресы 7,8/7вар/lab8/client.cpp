/**
 * Program for lab05
 *
 * @author      Malev Kirill
 * @author      Pavlov Alexey
 * @version     1.0
 */

#include <signal.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tcpd.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h> 

using namespace std;


main (int argc, char * argv[]) {
  cout << "Client for lab8 by pwnz_teh_win_team"<<endl;

  int sockfd, n;	//Basic initialization
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer[256];

  //socket initializing	
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    cout << "Client: Error in socket creating"<<endl;
    return (1);
  }
  else cout << "Client: Aquired sockfd = "<<sockfd <<endl;

  server = gethostbyname("crios");	//We should connect to host named "crios"
  if (server == NULL) {
    cout << "Client: Error, no such host"<<endl;
    exit(0);
  }
  
  //clean structure to set new
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;	//Deal with inet family
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);	//Setting address for hostname "crios"
  serv_addr.sin_port = htons(51717);	//Yup, we use port #51717

  //Connecting to socket set 2 lines above
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
    cout<< "Client: Error in connecting"<<endl;
    return (1);
  }

  //Starting password check
  while (strcmp (buffer,"correct")!=0) {	//Leave if get message "correct"
    cout<<"Client: Enter password:" <<endl;
    bzero(buffer,256);
    cin >> buffer;	
    n = write(sockfd,buffer,strlen(buffer));	//Sending password to server
    if (n < 0) {
      cout<< "Client: Error writing to socket"<<endl;
    }
 
    bzero(buffer,256);	//Cleaning buffer
    n = read(sockfd,buffer,255);	//Recieving message from server
    if (n < 0) {
      cout<< "Client: Error reading from socket"<<endl;
    }
    cout << buffer <<endl;
    if (strcmp (buffer,"You are blocked for some time")==0) sleep (10);	//If server blocked us, we hibernate
  }
  return 0;
}

