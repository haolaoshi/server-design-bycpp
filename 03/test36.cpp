/*
 * epoll with ET mode
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <errno.h>
#include <iostream>

int main()
{
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if(listenfd == -1)
  {
    return -1;
  }

  int on = 1;
  setsockopt(listenfd,SOL_SOCKET, SO_REUSEADDR,(char*)&on, sizeof(on));
  setsockopt(listenfd,SOL_SOCKET, SO_REUSEPORT,(char*)&on, sizeof(on));

  int oldsockflag = fcntl(listenfd,F_GETFL,0);
  int newsockflag = oldsockflag | O_NONBLOCK;
  if(fcntl(listenfd,F_SETFL,newsockflag) == -1)
  {
    close(listenfd);
    return -1;
  }

  struct sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  bindaddr.sin_port = htons(3000);

  if(bind(listenfd,(struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1)
  {
    close(listenfd);
    return -1;
  }

  if(listen(listenfd, SOMAXCONN) ==  -1)
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

  epoll_event listen_fd_event;
  listen_fd_event.data.fd = listenfd;
  listen_fd_event.events = EPOLLIN;
  //listen_fd_event.events |= EPOLLET;

  if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &listen_fd_event) == -1)
  {
    close(listenfd);
    return -1;
  }

  int n ;
  while(true)
  {
    epoll_event  array_epoll_events[1024];
    n = epoll_wait(epollfd,array_epoll_events, 1024,1000);
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
      epoll_event ee = array_epoll_events[i];
      if(ee.events & EPOLLIN)
      {
        if(ee.data.fd == listenfd)
        {
          struct sockaddr_in clientaddr;
          socklen_t len = sizeof(clientaddr);
          int clientfd = accept(listenfd, (struct sockaddr*)&clientfd, &len);
          if(clientfd != -1)
          {
            int oldsockflag = fcntl(clientfd, F_GETFL, 0);
            int newsockflag = oldsockflag | O_NONBLOCK;
            if(fcntl(clientfd, F_SETFL, newsockflag) == -1)
            {
              close(clientfd);
              std::cout<<"set non-block to clientfd error";
            }
            else
            {
              epoll_event client_fd_event;
              client_fd_event.data.fd = clientfd;
              client_fd_event.events = EPOLLIN | EPOLLOUT;
              client_fd_event.events |= EPOLLET;

              if(epoll_ctl(epollfd, EPOLL_CTL_ADD,clientfd,&client_fd_event) != -1)
                std::cout<<"new client accept.fd:" << clientfd<<std::endl;
              else
              {
                std::cout<<"add client fd to epollfd error"<<std::endl;
                close(clientfd);
              }
            }
          }
        }
        else
        {
          std::cout<<"client fd " << ee.data.fd <<" recv data." << std::endl;
          char recvbuf[1024] = {0};
          int m = read(ee.data.fd, recvbuf, 1024);
          if(m == 0)
          {
            if(epoll_ctl(epollfd,EPOLL_CTL_DEL,ee.data.fd,NULL) != -1)
            {
              std::cout<<"client disconnect, fd:"<<ee.data.fd<<std::endl;
            }
            close(ee.data.fd);
          }
          else if(m < 0)
          {
            if(errno != EWOULDBLOCK && errno != EINTR)
            {
              if(epoll_ctl(epollfd,EPOLL_CTL_DEL,ee.data.fd, NULL) != -1)
                std::cout<<"client disconnect, fd:"<<ee.data.fd<<std::endl;
              close(ee.data.fd);
            }
          }
          else
          {
            std::cout<<"nomally recv data:"<<ee.data.fd<<","<<recvbuf<<std::endl;


            epoll_event  client_fd_event;
            client_fd_event.data.fd = ee.data.fd;
            //again
            client_fd_event.events = EPOLLIN | EPOLLOUT | EPOLLET;
            if(epoll_ctl(epollfd,EPOLL_CTL_MOD, ee.data.fd,&client_fd_event) != -1)
            {
              std::cout<<"epoll_ctl successfully mode : EPOLL_CTL_MOD, fd:"
                <<ee.data.fd<<std::endl;
            }
          }
        }//old socket 
      }
      else if(ee.events & EPOLLOUT)
      {
        if(ee.data.fd != listenfd)
          std::cout<<"EPOLLOUT triggered,fd:"<<ee.data.fd<<std::endl;
      }
      else if(ee.events & EPOLLERR)
      {
        std::cout<<"EPOLLERR"<<std::endl;
      }
    }//end for i = 0 to n 
  }//while(true);
}
