/*
 * select usage 
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <vector>
#include <errno.h>

#define INVALID_FD   -1


int main(int argc,char* argv[])
{
  int listenfd = socket(AF_INET,SOCK_STREAM,0);
  if(listenfd == -1)
  {
    std::cout <<"error when create socket." <<std::endl;
    return -1;
  }

  struct sockaddr_in   bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bindaddr.sin_port = htons(3000);

  if(bind(listenfd,(struct sockaddr*)&bindaddr,sizeof(bindaddr)) == -1)
  {
    std::cout <<"bind socket error."<<std::endl;
    return -1;
  }

  if(listen(listenfd,SOMAXCONN) == -1)
  {
    std::cout <<" start listen failed."<< std::endl;
    return -1;
  }

  std::vector<int> client_fds;
  int maxfd;

  while(true)
  {
    fd_set readset;
    FD_ZERO(&readset);

    FD_SET(listenfd,&readset);
    maxfd = listenfd;

    int clientlen = client_fds.size();
    for(int i = 0 ; i < clientlen; i++)
    {
      if(client_fds[i] != INVALID_FD)
      {
        FD_SET(client_fds[i],&readset);
        if(maxfd < client_fds[i])
          maxfd = client_fds[i];
      }
    }

    timeval tm;
    tm.tv_sec = 0;
//tm.tv_sec = 1;
    tm.tv_usec = 0;

    int ret = select(maxfd + 1, &readset, NULL,NULL,NULL);
    //int ret = select(maxfd + 1, &readset, NULL,NULL,&tm);
    std::cout<<"tm.tv_sec:" << tm.tv_sec <<",tm.tv_usec:"<<tm.tv_usec<<std::endl;
    if(ret == -1)
    {
      if(errno != EINTR)
        break;
    }
    else if(ret == 0)
    {
      std::cout<<" no event in specific time int."<< std::endl;     
      continue;
    }
    else
    {
      if(FD_ISSET(listenfd,&readset))
      {
        struct sockaddr_in clientaddr;
        socklen_t clientaddrlen = sizeof(clientaddr);

        int client_fd = accept(listenfd,(struct sockaddr*)&clientaddr,&clientaddrlen);
        if(client_fd == INVALID_FD)
          break;
        std::cout <<"accept a client , fd:" <<client_fd<<std::endl;
        client_fds.push_back(client_fd);
      }
      else
      {
        char recvBuf[64];
        int clientfdslen = client_fds.size();
        for(int i = 0 ; i< clientfdslen; i++)
        {
          if(client_fds[i] != INVALID_FD&& FD_ISSET(client_fds[i],&readset))
          {
            memset(recvBuf, 0, sizeof(recvBuf));
            int len = recv(client_fds[i],recvBuf,64,0);
            if(len < 0)
            {
              std::cout <<" recv data fd : "<<client_fds[i]<<" error."<<std::endl;
              close(client_fds[i]);
              client_fds[i] = INVALID_FD;
              continue;
            }
            else if(len == 0)
            {
              std::cout<<" the opposite closed." <<std::endl;
              break;
            }

            std::cout <<"clientfd: "<< client_fds[i]<<",recv data: " <<recvBuf<<
              std::endl;

          }
          else
            std::cout<<" other fd event : " << std::endl;
        }
      }
    }
  }

  int clientfdslen = client_fds.size();
  for(int i = 0 ; i < clientfdslen; i++)
  {
    if(client_fds[i] != INVALID_FD)
      close(client_fds[i]);
  }

  close(listenfd);
  return 0;
}
