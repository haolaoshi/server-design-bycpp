/*
 *
 * Non - Block client.cpp
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>


#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#define SERVER_ADDRESS  "127.0.0.1"
#define SERVER_PORT     3000
#define SEND_DATA       "helloworld"

int main(int argc,char **argv)
{
  int clientfd = socket(AF_INET,SOCK_STREAM,0);
  if(clientfd == -1)
  {
    std::cout <<"error when create socket."<<std::endl;
    return -1;
  }


  struct sockaddr_in  bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  bindaddr.sin_port= htons(SERVER_PORT);
  if(connect(clientfd,(struct sockaddr*)&bindaddr,sizeof(bindaddr)) == -1)
  {
    std::cout<<"error when connect to server."<<std::endl;
    close(clientfd);
    return -1;
  }

  int oldsocketflag = fcntl(clientfd, F_GETFL, 0);
  int newsocketflag = oldsocketflag | O_NONBLOCK;
  if(fcntl(clientfd,F_SETFL,newsocketflag) == -1)
  {
    close(clientfd);
    std::cout <<"error when set socket to non-block failed."<<std::endl;
    return -1;
  }

  int count = 0;
  while(true)
  {
    int ret = send(clientfd,SEND_DATA, strlen(SEND_DATA),0);
    if(ret == -1)
    {
      if(errno == EWOULDBLOCK)
      {
        std::cout <<"send data error as TCP WINDOW is too small."<<std::endl;
        continue;
      }
      else if(errno = EINTR)
      {
        std::cout <<"sending data interrupted by signal."<<std::endl;
        continue;
      }
      else
      {
        std::cout <<"send data errror"<<std::endl;
        break;
      }
    }
    else if(ret == 0)
    {
      std::cout <<" send data error , counter is closed."<<std::endl;
      close(clientfd);
      break;
    }
    else
    {
      count ++;
      std::cout <<"send data success , count :" << count << std::endl;
    }
  }
  close(clientfd);

  return 0;
}
