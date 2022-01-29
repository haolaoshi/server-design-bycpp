/*
 * use poll  implemet asyn-connect
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>

#define SERVER_ADDRESS    "127.0.0.1"
#define SERVER_PORT       3000
#define SEND_DATA         "helloworld"

int main(int argc ,char** argv)
{
  int clientfd = socket(AF_INET, SOCK_STREAM, 0) ;
  if(clientfd == -1)
  {
    std::cout <<"error";
    return -1;
  }

  int oldsocketflag = fcntl(clientfd, F_GETFL, 0);
  int newsocketflag = oldsocketflag | O_NONBLOCK;
  if(fcntl(clientfd , F_SETFL, newsocketflag) == -1)
  {
    std::cout <<"error set non-block";
    close(clientfd);
    return -1;
  }

  struct sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  bindaddr.sin_port = htons(SERVER_PORT);


  for(;;)
  {
    int ret = connect(clientfd, (struct sockaddr*)&bindaddr, sizeof(bindaddr));
    if(ret == 0)
    {
      std::cout <<"the server disconn";
      close(clientfd);
      return -1;
    }
    else if(ret == -1)
    {
      if(errno == EINTR)
      {
        std::cout <<"connecting interrupted.";
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

  pollfd event;
  event.fd = clientfd;
  event.events = POLLOUT;
  int timeout = 3000;

  if(poll(&event, 1, timeout) != 1)
  {
    close(clientfd);
    std::cout<<" [poll] connect to server error";
    return -1;
  }

  if(!(event.revents & POLLOUT))
  {
    close(clientfd);
    std::cout <<"[POLLOUT] connecto server error";
    return -1;
  }

  int err;
  socklen_t len = static_cast<socklen_t>(sizeof(err));
  if(::getsockopt(clientfd,SOL_SOCKET, SO_ERROR, &err, &len) < 0)
    return -1;

  if(err == 0)
    std::cout <<"connect to server succ.";
  else
    std::cout <<"connect to server failed.";

  close(clientfd);
  return 0;
}
