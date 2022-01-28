/*
 * Non-Block  client recv 
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <errno.h>


#define SERVER_ADDRESS  "127.0.0.1"
#define SERVER_PORT     3000



int main(int argc, char** argv)
{
  int clientfd = socket(AF_INET, SOCK_STREAM,0);
  if(clientfd == -1)
  {
    std::cout <<"error when create socket." << std::endl;
    return -1;
  }

  struct sockaddr_in   bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  bindaddr.sin_port = htons(SERVER_PORT);

  if(connect(clientfd,(struct sockaddr*)&bindaddr,sizeof(bindaddr)) == -1)
  {
    std::cout<<"error when connect to server."<<std::endl;
    close(clientfd);
    return -1;
  }

  while(true)
  {
    char recvBuf[32] = {0};

    int len = recv(clientfd,recvBuf, 32, 0);
    if(len < 0)
    {
      std::cout<<"error when recv data"<<std::endl;
      break;
    }
    else if(len == 0)
    {
      std::cout<<"the server disconnect ."<< std::endl;
      break;
    }
    else
    {
      std::cout<<"recv data : " << recvBuf<<std::endl;
    }
  }

  close(clientfd);

  return 0;
}
