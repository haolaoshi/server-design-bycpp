/*
 * Block && Unblock
 *
 */

//Block server
//

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  int listenfd = socket(AF_INET,SOCK_STREAM,0);
  if(listenfd == -1)
  {
    std::cout <<"error when create a socket."<< std::endl;
    return -1;
  }

  struct sockaddr_in  bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bindaddr.sin_port= htons(3000);

  if(bind(listenfd,(struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1)
  {
    std::cout <<"error when bind socket."<<std::endl;
    return -1;
  }

  if(listen(listenfd,SOMAXCONN) == -1)
  {
    std::cout <<"error when listen fd."<< std::endl;
    return -1;
  }

  while(true)
  {
    struct sockaddr_in clientaddr;
    socklen_t  clientlen = sizeof(clientaddr);

    int clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
    if(clientfd != -1)
    {
      std::cout <<"accv a client connection."<<std::endl;
    }
  }

  close(listenfd);

  return 0;
}
