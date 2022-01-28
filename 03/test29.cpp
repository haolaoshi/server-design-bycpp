/*
 * nonblocking connect 
 *
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#define SERVER_ADDRESS    "127.0.0.1"
#define SERVER_PORT       3000
#define SEND_DATA         "helloworld"


int main(int argc,char** argv)
{

  int clientfd = socket(AF_INET,SOCK_STREAM,0);
  if(clientfd == -1)
  {
    std::cout <<"errorn when create socket."<<std::endl;
    return -1;
  }

  int oldsocketflag = fcntl(clientfd,F_GETFL,0);
  int newsocketflag = oldsocketflag | O_NONBLOCK;
  if (fcntl(clientfd,F_SETFL,newsocketflag) == -1)
  {
    close(clientfd);
    std::cout <<"error when set non - block ." << std::endl;
    return -1;
  }

  struct sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  bindaddr.sin_port = htons(SERVER_PORT);

  for(;;)
  {
    int ret = connect(clientfd,(struct sockaddr*)&bindaddr,sizeof(bindaddr));
    if(ret == 0)
    {
      std::cout << "connect to server successfuly"<<std::endl;
      close(clientfd);
      return 0;
    }
    else if(ret == -1)
    {
      if(errno == EINTR)
      {
        std::cout <<" connectin interrupted by signal, try again," << std::endl;
        continue;
      }
      else if(errno == EINPROGRESS)
      {
        std::cout<<"ing..."<<std::endl;
        break;
      }
      else
      {
        std::cout<<"Maybe sth wrong."<<std::endl;
        close(clientfd);
        return -1;
      }
    }
  }

  fd_set  witseter;
  FD_ZERO(&witseter);
  FD_SET(clientfd,&witseter);
  struct timeval tv;
  tv.tv_sec = 3;
  tv.tv_usec = 0;

  if(select(clientfd + 1, NULL, &witseter, NULL, &tv) == 1)
  {
    std::cout<<" [select] connect oserver sucess . " << std::endl;
  }
  else
  {
    std::cout <<" select - connecto server error."<< std::endl;
  }

  close(clientfd);
  return 0;
}
