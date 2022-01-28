/*
 * getsockopt
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <fcntl.h>


#define SERVER_ADDRESS    "127.0.0.1"
#define SERVER_PORT       3000
#define SEND_DATA         "helloworld"


int main(int argc, char** argv)
{
  int clientfd = socket(AF_INET, SOCK_STREAM,0);
  if(clientfd == -1)
  {
    std::cout<<"error when creae scoke."<<std::endl;
    return -1;
  }

  int oldsocketflag = fcntl(clientfd,F_GETFL,0);
  int newsocketflag = oldsocketflag | O_NONBLOCK;
  if(fcntl(clientfd,F_SETFL,newsocketflag) == -1)
  {
    std::cout<<"error when set non-block to socket."<<std::endl;
    close(clientfd);
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
      std::cout <<"connect ot setverv succes.."<<std::endl;
      close(clientfd);
      return 0;
    }
    else if(ret == -1)
    {
      if(errno == EINTR)
      {
        std::cout <<"connectin interrupted by signal, try again."<<std::endl;
        continue;
      }

      else if(errno == EINPROGRESS)
      {
        break;
      }
      else
      {
        close(clientfd);
        return -1;
      }
    }
  }

  fd_set writter;
  FD_ZERO(&writter);
  FD_SET(clientfd,&writter);
  struct timeval tv;
  tv.tv_sec = 3;
  tv.tv_usec = 0;

  if(select(clientfd + 1, NULL, &writter, NULL, &tv) != 1)
  {
    std::cout <<" [select]  to server error."<<std::endl;
    close(clientfd);
    return -1;
  }

  int err;
  socklen_t len = static_cast<socklen_t>(sizeof err);
  if(::getsockopt(clientfd,SOL_SOCKET, SO_ERROR, &err,&len) < 0)
  {
    close(clientfd);
    return -1;
  }

  if(err == 0)
  {
    std::cout <<" connect o server success."<<std::endl;
  }
  else
  {
    std::cout<<"connect o server faile.d"<<std::endl;
  }

  close(clientfd);

  return 0;
}
