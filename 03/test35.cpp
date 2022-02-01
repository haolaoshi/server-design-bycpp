/*
 *
 * Epoll with event LT
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <string.h>
#include <poll.h>

int main()
{
  int listenfd = socket(AF_INET, SOCK_STREAM,0);
  if(listenfd == -1)
  {
    std::cout<<"error1";
    return -1;
  }

  int on = 1;
  setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on));
  setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT,(char*)&on,sizeof(on));

  int oldsockflag = fcntl(listenfd,F_GETFL,0);
  int newsockflag = oldsockflag | O_NONBLOCK;
  if(fcntl(listenfd,F_SETFL,newsockflag) == -1)
  {
    std::cout<<"error2";
    close(listenfd);
    return -1;
  }

  struct sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  bindaddr.sin_port = htons(3000);

  if(bind(listenfd,(struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1)
  {
    std::cout<<"error 3";
    close(listenfd);
    return -1;
  }

  if(listen(listenfd, SOMAXCONN) == -1)
  {
    std::cout <<"error 4";
    close(listenfd);
    return -1;
  }

  int epollfd = epoll_create(1);
  if(epollfd == -1)
  {
    std::cout <<"error 5";
    close(listenfd);
    return -1;
  }

  epoll_event  listen_epoll_event;
  listen_epoll_event.data.fd = listenfd;
  listen_epoll_event.events = EPOLLIN;
  //listen_epoll_event.events |= EPOLLET;

  if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &listen_epoll_event) == -1)
  {
    std::cout<<"error 6";
    close(listenfd);
    return -1;
  }

  int n ;
  while(true)
  {
    epoll_event  epoll_event_array[1024];
    n = epoll_wait(epollfd , epoll_event_array, 1024,1000);
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

    for(size_t i = 0 ; i < n; i++)
    {
      if(epoll_event_array[i].events & EPOLLIN)
      {
        if(epoll_event_array[i].data.fd == listenfd)
        {
          struct sockaddr_in clientaddr;
          socklen_t len = sizeof(clientaddr);
          int clientfd = accept(listenfd, (struct sockaddr*)&clientfd, &len);
          if(clientfd != -1)
          {
            int oldsockflag = fcntl(clientfd, F_GETFL, 0);
            int newsockflag = oldsockflag | O_NONBLOCK;
            if(fcntl(clientfd, F_SETFL, newsockflag)  == -1)
            {
              close(clientfd);
              return -1;
            }
            epoll_event client_epoll_event;
            client_epoll_event.data.fd = clientfd;
            client_epoll_event.events = EPOLLIN |EPOLLOUT;
            //client_epoll_event.event  |= EPOLLET;
            if(epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&client_epoll_event) != -1)
            {
              std::cout<<"new client accepted , fd:"<<clientfd<<std::endl;
            }
            else
            {
              std::cout<<"add client fd to epollfd error"<<std::endl;
              close(clientfd);
            }
          }
          else
            std::cout<<"accept error."<<std::endl;
        }
        else //old socket
        {
          std::cout<<"client fd : " <<epoll_event_array[i].data.fd <<std::endl;
          char recvbuf[1024] = {0};
          //int m = recv(epoll_event_array[i].data.fd , recvbuf, 1024,0);
          int m = read(epoll_event_array[i].data.fd , recvbuf, 1024);
          if(m == 0)
          {
            if(epoll_ctl(epollfd,EPOLL_CTL_DEL, epoll_event_array[i].data.fd,NULL) != -1)
              std::cout<<"the opposite close socket."<<std::endl;
            close(epoll_event_array[i].data.fd);
          }
          else if(m < 0)
          {
            if(errno != EWOULDBLOCK && errno != EINTR)
            {
              if(epoll_ctl(epollfd, EPOLL_CTL_DEL,epoll_event_array[i].data.fd , NULL)!= -1)
                std::cout<<"client disconnect fd:"<<epoll_event_array[i].data.fd<<std::endl;
              close(epoll_event_array[i].data.fd);
            }
          }
          else
          {
            std::cout <<"nomally recv from client : " << epoll_event_array[i].data.fd
              <<">"<<recvbuf<<std::endl;
          }
        }//old connection 
      }
      else if(epoll_event_array[i].events  & EPOLLOUT)
      {
        if(epoll_event_array[i].data.fd != listenfd)
          std::cout<<"EPOLLOUT triggered, fd:" << epoll_event_array[i].data.fd<<std::endl;
        else
          std::cout<<"EPOLLOUT"<<std::endl;
      }
      else if(epoll_event_array[i].events & EPOLLERR)
      {
        std::cout<<"EPOLLERR"<<std::endl;
      }
    }//for n of  epoll_wait 
  }//while(true)

  close(listenfd);
  return 0;
}
