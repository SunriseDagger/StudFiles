/**
 * Program for lab05
 *
 * @author      Malev Kirill
 * @author      Pavlov Alexey
 * @version     1.0
 */


#include <signal.h>
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
#include <stdlib.h>
#include <string.h>

using namespace std;


main (int argc, char *argv[]) {
  cout << "Server for lab8 by pwnz_teh_win_team"<<endl;

  int sockfd, newsockfd;	//Basic initialization
  socklen_t cli_len;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;

  //socket initializing
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    cout << "Server: Error in socket creating"<<endl; 
    return (1);
  }
  else cout << "Server: Aquired sockfd = "<<sockfd <<endl;

  //Deleting everything from server address
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;	//Dealing not with local net
  serv_addr.sin_addr.s_addr = INADDR_ANY;	//Server can have any ip
  serv_addr.sin_port = htons(51717);	//Setting port

  //Binding socket
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    cout << "Server: Error binding the socket"<<endl;
    cout << errno<<endl;
    return (1);
  }
  
  listen(sockfd,5); //Starting socket listening

  //Prepare for accepting connections
  cli_len = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
  if (newsockfd < 0) {
     cout<<"Server: Error in socket accepting"<<endl;
     return (1);
  }

  //if connection accepted, we start password checks
  int i=0;
  while (i!=5) {
    for (i=0;i<3;i++) {	//Grant user 3 attempts
      bzero(buffer,256);
      n = read(newsockfd,buffer,255); //Reading password from socket
      if (n < 0) cout <<("Server: Error reading from socket");
      cout << "Server: Password recieved: "<<buffer<<endl;	//Print to verify it
      if (strcmp (buffer,"password")==0) {	//Password is "password"
        i=5; 
        n = write(newsockfd,"correct",7);	//We notify client that it is correct
        if (n < 0) cout << ("Server: Error writing to socket");
      }
      else {
        n = write(newsockfd,"Server: Password is incorrect",21);	//Otherwise, notify that not
        if (n < 0) cout << ("Server: Error writing to socket");
      }  
      if (i==2) { 
        cout << "Server: Blocking enter for 10 secs"<<endl;	//After 3 attempts, user get blocked
        n = write(newsockfd,"Server: You are blocked for some time",30);
        if (n < 0) cout << ("Server: Error writing to socket");
        sleep (10);
      }
    }
  }
  return 0;   

}
