/*
 *
 * get buffer available 
 *
 *
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <poll.h>

#define INVALID_FD  -1


int main(int argc,char** argv)
{
  //1.建立socket
  int listenfd = socket(AF_INET, SOCK_STREAM,0);
  if(listenfd == -1)
  {
    std::cout<<"error when create socket. " <<std::endl;
    return -1;
  }

  //设置socket为非阻塞
  int oldsocketflag = fcntl(listenfd,F_GETFL,0);
  int newsocketflag = oldsocketflag | O_NONBLOCK;
  if(fcntl(listenfd,F_SETFL,newsocketflag) == -1)
  {
    std::cout<<"error when set non-block "<<std::endl;
    close(listenfd);
    return -1;
  }

  //利用地址和端口
  int on = 1;
  setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on));
  setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT,(char*)&on,sizeof(on));

  //初始化服务器地址
  struct sockaddr_in  bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  bindaddr.sin_port = htons(3000);

  //绑定
  if(bind(listenfd,(struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1)
  {
    std::cout <<"error when bind to addr."<<std::endl;
    close(listenfd);
    return -1;
  }

  ///启动监听
  if(listen(listenfd,SOMAXCONN) == -1)
  {
    std::cout<<"error when listne ."<<std::endl;
    close(listenfd);

    return -1;
  }

  std::vector<pollfd> fds;
  pollfd  listen_fd_info;
  listen_fd_info.fd = listenfd;
  listen_fd_info.events = POLLIN;
  listen_fd_info.revents = 0;
  fds.push_back(listen_fd_info);


  bool exist_invalid_fd;
  int n;

  while(true)
  {
    exist_invalid_fd = false;
    n = poll(&fds[0],fds.size(), 1000);
    if(n < 0)
    {
      //是被信号中断
      if(errno == EINTR)
        continue;
      //其它的是其它是出错
      break;
    }
    else if(n == 0)
    {
      //超时，继续
      continue;
    }

    int size = fds.size();
    for(size_t i = 0 ; i < size; i++)
    {
      //事件可读
      if(fds[i].revents & POLLIN)
      {
        if(fds[i].fd == listenfd)
        {
          //监听socket接受新连接
          struct sockaddr_in clientaddr;
          socklen_t clientaddrlen = sizeof(clientaddr);
          //接受客户端的连接并将产生的fd加入到数组 中
          int clientfd = accept(listenfd , (struct sockaddr*)&clientfd, &clientaddrlen);
          if(clientfd != -1)
          {
            //把客户端也设置为非阻塞
            int oldsocketflag = fcntl(clientfd,F_GETFL,0);
            int newsocketflag = oldsocketflag | O_NONBLOCK;
            if(fcntl(clientfd, F_SETFL,newsocketflag) == -1)
            {
              std::cout<<"error when set non-block "<<std::endl;
              close(clientfd);
            }
            else
            {
              struct pollfd client_fd_info;
              client_fd_info.fd = clientfd;
              client_fd_info.events = POLLIN;
              client_fd_info.revents = 0;
              fds.push_back(client_fd_info);
              std::cout<<"new client accept.  fd:"<<clientfd << std::endl;
            }
          }
        }
        else
        {
          //在socket 可读时获取 可读字节数
          ulong byteToRecv = 0;
          if(ioctl(fds[i].fd , FIONREAD, &byteToRecv) == 0)
          {
            std::cout <<" byteToRecv : " << byteToRecv <<std::endl;
          }

          //普通
          char buf[64] = {0};
          int m = recv(fds[i].fd, buf, 64,0);
          if(m <= 0)
          {
            if(errno != EINTR && errno != EWOULDBLOCK)
            {
              std::cout <<"client read error. fd:"<<fds[i].fd<<std::endl;
              close(fds[i].fd);
              fds[i].fd = INVALID_FD;
              exist_invalid_fd = true;
            }
        }
        else
        {
          std::cout <<"recv from client : " << buf << ", clientfd:"<<fds[i].fd << std::endl;

        }
      }
    }
    else if(fds[i].revents & POLLERR)
    {
      std::cout <<"pollerr"<<std::endl;
    }
  }//end out-for-loop

    if(exist_invalid_fd)
    {
      for(std::vector<pollfd>::iterator iter = fds.begin(); iter != fds.end(); )
      {
        if(iter->fd == INVALID_FD)
        {
          iter = fds.erase(iter);
        }
        else
          iter++;
      }
    }
  }//end-while loop

  for(std::vector<pollfd>::iterator  it = fds.begin(); it != fds.end(); it++)
    close(it->fd);

  return 0;
}
