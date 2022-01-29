/*
 *
 * poll  usage
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <vector>
#include <string.h>
#include <fcntl.h>
#include <poll.h>



#define INVALID_FD    -1
int main(int argc ,char** argv)
{


  int listenfd = socket(AF_INET,SOCK_STREAM,0);
  if(listenfd == -1)
  {
    std::cout<<"error when create socket.";
    return -1;
  }

  int oldsocketflag = fcntl(listenfd,F_GETFL,0);
  int newsocketflag = oldsocketflag | O_NONBLOCK;
  if(fcntl(listenfd,F_SETFL,newsocketflag) == -1)
  {
    std::cout <<"error when set non-block";
    close(listenfd);
    return -1;
  }

  int on = 1;
  setsockopt(listenfd,SOL_SOCKET, SO_REUSEADDR,(char*)&on, sizeof(on));
  setsockopt(listenfd,SOL_SOCKET, SO_REUSEPORT,(char*)&on, sizeof(on));

  struct sockaddr_in  bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  bindaddr.sin_port = htons(3000);

  if(bind(listenfd,(struct sockaddr*)&bindaddr, sizeof(bindaddr))== -1)
  {
    std::cout <<"bind socket error.";
    close(listenfd);
    return -1;
  }

  if(listen(listenfd,SOMAXCONN) == -1)
  {
    std::cout <<"error when listen";
    close(listenfd);
    return -1;
  }

  std::vector<pollfd> fds;
  pollfd  listen_fd_info;
  listen_fd_info.fd = listenfd;
  listen_fd_info.events = POLLIN;
  listen_fd_info.revents = 0;

  fds.push_back(listen_fd_info);

  bool exist_invalid_fd = false;
  int n ;

  while(true)
  {
    exist_invalid_fd = false;
    n = poll(&fds[0],fds.size(), 1000);
    if(n <0 )
    {
      if(errno == EINTR)
        continue;
      break;
    }
    else if(n == 0)
    {
      continue;
    }

    for(size_t i = 0 ; i < fds.size(); i++)
    {
      if(fds[i].revents & POLLIN)
      {
        if(fds[i].fd == listenfd)
        {
          struct sockaddr_in clientaddr;
          socklen_t clientaddrlen = sizeof(clientaddr);

          int clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientaddrlen);
          if(clientfd != -1)
          {
            int oldsocketflag = fcntl(clientfd, F_GETFL, 0);
            int newsocketflag = oldsocketflag | O_NONBLOCK;
            if(fcntl(clientfd, F_SETFL, newsocketflag) == -1)
            {
              std::cout <<"error when set non-block";
              close(clientfd);
            }
            else
            {
              struct pollfd client_fd_info;
              client_fd_info.fd = clientfd;
              client_fd_info.events = POLLIN;
              client_fd_info.revents = 0;
              fds.push_back(client_fd_info);
              std::cout<<" new client accepted . fd :" << clientfd << std::endl;
            }
          }
        }
        else
        {
          char buf[64] = {0};
          int m = recv(fds[i].fd, buf,64, 0);
          if(m <= 0)
          {
            if(errno != EINTR && errno != EWOULDBLOCK)
            {
              for(std::vector<pollfd>::iterator it = fds.begin(); it != fds.end();it++)
              {
                if(it->fd == fds[i].fd)
                {
                  std::cout << "client disconnected, clientfd:"<< fds[i].fd << std::endl;
                  close(fds[i].fd);
                  it->fd = INVALID_FD;
                  exist_invalid_fd = true;
                  break;
                }
              }
            }
          }
          else
          {
            std::cout <<"recv from client : " << buf << ", clientfd : " << fds[i].fd<<std::endl;
          }
        }
      }
      else if(fds[i].revents & POLLERR)
      {
        std::cout<<"POLLERR"<<std::endl;
      }
    }

    if(exist_invalid_fd)
    {
      for(std::vector<pollfd>::iterator it=fds.begin(); it!= fds.end();)
      {
        if(it->fd == INVALID_FD)
          it = fds.erase(it);
        else
          it++;
      }
    }
  }//end of while(true);

  for(std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++)
    close(it->fd);

  return 0;
}
