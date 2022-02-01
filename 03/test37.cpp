/*
 *
 * EPOLL with EPOLLONESHOT
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <poll.h>

int main()
{
  int listenfd = socket(AF_INET, SOCK_STREAM,0);
  if(listenfd == -1)
    return -1;

  int on = 1;
  setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on));
  setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT,(char*)&on,sizeof(on));

  int oldsockflag = fcntl(listenfd,F_GETFL,0);
  int newsockflag = oldsockflag | O_NONBLOCK;
  if(fcntl(listenfd,F_SETFL, newsockflag) == -1)
  {
    close(listenfd);
    return -1;
  }

  struct sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  bindaddr.sin_port = htons(3000);
  if(bind(listenfd,(struct sockaddr*)&bindaddr,sizeof(bindaddr)) == -1)
  {
    close(listenfd);
    return -1;
  }

  if(listen(listenfd,SOMAXCONN) == -1)
  {
    close(listenfd);
    return -1;
  }

  int epollfd = epoll_create(1);
  if(epollfd == -1)
  {
    close(listenfd);
    return -1;
  }

  epoll_event event_listen_fd;
  event_listen_fd.data.fd = listenfd;
  event_listen_fd.events = EPOLLIN | EPOLLOUT;

  if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event_listen_fd) == -1)
  {
    close(listenfd);
    return -1;
  }

  int n ;
  while(true)
  {
    epoll_event  array_events[1024] = {0};
    n = epoll_wait(epollfd,array_events,1024,1000);
    if(n < 0)
    {
      if(errno == EINTR) continue;
      break;
    }
    else if(n == 0)
    {
      continue;
    }

    for(size_t i = 0 ; i < n ; i++)
    {
      epoll_event ee = array_events[i];
      if(ee.events & EPOLLIN)
      {
        if(ee.data.fd == listenfd)
        {
          struct sockaddr_in clientaddr;
          socklen_t len = sizeof(clientaddr);
          int clientfd = accept(listenfd,(struct sockaddr*)&clientaddr,&len);
          if(clientfd != -1)
          {
            std::cout<<"accept fd:"<<clientfd<<std::endl;
            int oldsockflag  = fcntl(clientfd,F_GETFL,0);
            int newsockflag = oldsockflag | O_NONBLOCK;
            if(fcntl(clientfd, F_SETFL, newsockflag) == -1)
            {
              close(clientfd);
            }
            else
            {
              epoll_event client_event;
              client_event.data.fd = clientfd;
              client_event.events = EPOLLIN;
              client_event.events |= EPOLLONESHOT;
              if(epoll_ctl(epollfd, EPOLL_CTL_ADD,clientfd,&client_event) != -1)
                std::cout<<" new client accepted , fd:" << clientfd<<std::endl;
              else
                close(clientfd);
            }
          }
        }
        else
        {
          std::cout<<"client (old) fd : " << ee.data.fd <<", recvdata."<<std::endl;

          char ch;

          int m = read(ee.data.fd, &ch, 1);
          if(m == 0)
          {
            if(epoll_ctl(epollfd, EPOLL_CTL_DEL, ee.data.fd, NULL) != -1)
              std::cout<<" client disconm, fd:"<<ee.data.fd<<std::endl;
            close(ee.data.fd);
          }
          else if(m < 0)
          {
            if(errno != EWOULDBLOCK && errno != EINTR)
            {
              if(epoll_ctl(epollfd, EPOLL_CTL_DEL, ee.data.fd, NULL) != -1)
                std::cout <<"client disconn,fd:"<<ee.data.fd<<std::endl;
              close(ee.data.fd);
            }
          }
          else
          {
            std::cout<<"nomally recv " << ee.data.fd <<","<<ch<<std::endl;

            epoll_event fd_event_c;
            fd_event_c.data.fd = ee.data.fd;
            fd_event_c.events = EPOLLIN;
            if(epoll_ctl(epollfd, EPOLL_CTL_MOD, ee.data.fd, &fd_event_c) != -1)
              std::cout<<" re-claim EPOLLIN event to client fd:"<<ee.data.fd<<std::endl;
            else
            {
              if(epoll_ctl(epollfd,EPOLL_CTL_DEL,ee.data.fd,NULL) != -1)
                std::cout<<" remove fd from epoll fd , clientfd:"<<ee.data.fd<<std::endl;
              close(ee.data.fd);
            }
          }
        }
      }
      else if(ee.events & EPOLLOUT)
      {
        std::cout<<"EPOLLOUT"<<std::endl;
      }
      else if(ee.events & EPOLLERR)
      {
        std::cout<<"EPOLLERR"<<std::endl;
      }
    }//end for
  }//ehn while(true)
}
