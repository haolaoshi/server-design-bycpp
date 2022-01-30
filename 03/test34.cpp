/*
 *
 * Epoll  :  LT VS ET
 *
 * server.cpp
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

int main(int argc,char** argv)
{
  int listenfd =socket(AF_INET, SOCK_STREAM, 0);
  if(listenfd == -1)
  {
    std::cout<<"error when create socket.";
    return -1;
  }

  int on = 1;
  setsockopt(listenfd,SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
  setsockopt(listenfd,SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on));


  int oldsocketflag = fcntl(listenfd , F_GETFL, 0);
  int newsocketflag = oldsocketflag | O_NONBLOCK;
  if(fcntl(listenfd, F_SETFL, newsocketflag) == -1 )
  {
    std::cout <<"err when set non-block";
    close(listenfd);
    return -1;
  }

  struct sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  bindaddr.sin_port = htons(3000);

  if(bind(listenfd,(struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1)
  {
    std::cout <<"err when bind ";
    close(listenfd);
    return -1;
  }

  if(listen(listenfd, SOMAXCONN) == -1)
  {
    std::cout <<"err when listen";
    close(listenfd);
    return -1;
  }

  int epollfd = epoll_create(1);
  if(epollfd == -1)
  {
    std::cout <<"error when epollfd ";
    close(listenfd);
    return -1;
  }

  epoll_event  listen_fd_event;
  listen_fd_event.data.fd = listenfd;
  listen_fd_event.events = EPOLLIN;
  //listen_fd_event.events |= EPOLLET; //ET mode


  if(epoll_ctl(epollfd,EPOLL_CTL_ADD, listenfd, &listen_fd_event) == -1)
  {
    std::cout<<"error when epoll_ctl ";
    close(listenfd);
    return -1;
  }

  int n ;
  while(true)
  {
    epoll_event epoll_events[1024];
    n  = epoll_wait(epollfd,epoll_events,1024,1000);

    if(n < 0)
    {
      if(errno == EINTR)
        continue;
      break;
    }
    else if(n == 0)
    {
      continue;
    }

    for(size_t i = 0 ; i < n ; i++)
    {
      if(epoll_events[i].events & EPOLLIN)
      {
        if(epoll_events[i].data.fd == listenfd)
        {
          struct sockaddr_in clientaddr;

//          clientaddr.sin_family = AF_INET;
//          clientaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
//          clientaddr.sin_port = htons(3000);
//
          socklen_t len = sizeof(clientaddr);
          int clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
          if(clientfd != -1)
          {
            int oldsocketflag = fcntl(clientfd, F_GETFL, 0);
            int newsocketflag = oldsocketflag | O_NONBLOCK;
            if(fcntl(clientfd, F_SETFL, newsocketflag) == -1)
            {
              close(clientfd);
              std::cout<<" error set non-block  to clientfd";
            }
            else
            {
              epoll_event client_fd_event;
              client_fd_event.data.fd = clientfd;
              client_fd_event.events = EPOLLIN;
              //client_fd_event.events |= EPOLLET;
              if(epoll_ctl(epollfd,EPOLL_CTL_ADD, clientfd, &client_fd_event) != -1)
              {
                std::cout <<"new client accepted, fd:" << clientfd<<std::endl;
              }
              else
              {
                std::cout <<"add client fd to epollfd error";
                close(clientfd);
              }
            }
          }
        }
        else
        {
          std::cout<<" client fd : " << epoll_events[i].data.fd <<" recv data" << std::endl;
          char ch;
          int m = recv(epoll_events[i].data.fd , &ch, 1,0);
          if(m == 0)
          {
            if(epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd,NULL) != -1)
            {
              std::cout<<" client disconnected, fd:"<< epoll_events[i].data.fd << std::endl;
            }

            close(epoll_events[i].data.fd);
          }
          else if(m < 0)
          {
            if(errno != EWOULDBLOCK && errno != EINTR)
            {
              if(epoll_ctl(epollfd,EPOLL_CTL_DEL,epoll_events[i].data.fd, NULL) != -1)
              {
                std::cout<<" clietn disconnected ,fd :" << epoll_events[i].data.fd << std::endl;

              }
              close(epoll_events[i].data.fd);
            }
          }
          else
          {
            std::cout<<" revv from client:"<< epoll_events[i].data.fd << "," <<ch<<std::endl;
          }
        }
      }
      else if(epoll_events[i].events & EPOLLERR)
      {
        std::cout<<"epollerr"<<std::endl;
      }
    }
  }

  close(listenfd);

  return 0;
}
