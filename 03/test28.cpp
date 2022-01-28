/*
 * Non-block recv  client
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
//#include <stdio.h>



#define SERVER_ADDRESS    "127.0.0.1"
#define SERVER_PORT       3000
#define SEND_DATA         "helloworld"



int main(int argc,char** argv)
{
  int clientfd = socket(AF_INET,SOCK_STREAM,0);
  if(clientfd == -1)
  {
    std::cout<<"error when create socket."<<std::endl;
    return -1;
  }

  struct sockaddr_in   bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  bindaddr.sin_port= htons(SERVER_PORT);

  if(connect(clientfd,(struct sockaddr*)&bindaddr,sizeof(bindaddr)) == -1)
  {
    std::cout << "error when connect ot server."<<std::endl;
    close(clientfd);
    return -1;
  }

  int oldsocketflag = fcntl(clientfd,F_GETFL,0);
  int newsocketflag = oldsocketflag | O_NONBLOCK;
  if(fcntl(clientfd,F_SETFL, newsocketflag) == -1)
  {
    std::cout<<"error when set socket to non-block."<<std::endl;
    close(clientfd);
    return -1;
  }

  while(true)
  {
    char recvBuf[32];
    int len = recv(clientfd,recvBuf,32,0);
    if(len > 0)
    {
      std::cout<<" recv data : " << recvBuf << std::endl;

    }

    else if(len == 0)
    {
      std::cout <<" counter closed socket.."<<std::endl;
      break;
    }
    else if(len == -1)
    {
      if(errno == EWOULDBLOCK)
      {
        std::cout<<" thereis no data available now ." << std::endl;
      }
      else if(errno == EINTR)
      {
        std::cout<<" rev data interrupted by signal ." << std::endl;
      }
      else
      {
        break;
      }
    }
  }
  close(clientfd);

  return 0;
}
