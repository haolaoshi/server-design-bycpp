#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <vector>

int main(int argc, char* argv[])
{
  int listenfd = socket(AF_INET,SOCK_STREAM, 0);
  if(listenfd == -1)
  {
    std::cout<<"error when create socket."<<std::endl;
    return -1;
  }

  struct sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bindaddr.sin_port = htons(3000);

  if(bind(listenfd, (struct sockaddr*)&bindaddr,sizeof(bindaddr)) == -1)
  {
    std::cout << "error when bind ."<<std::endl;
    return -1;
  }

  if(listen(listenfd , SOMAXCONN) == -1)
  {
    std::cout <<"error when listen."<< std::endl;
    return -1;
  }

  std::vector<int> clientfds;
  while(true)
  {

    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);

    int clientfd = accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
    if(clientfd != -1)
    {
      char recvBuf[32] = {0};
      int ret = recv(clientfd, recvBuf,32, 0);
      if(ret > 0)
      {
        std::cout <<" recv data from client : " << recvBuf<<std::endl;
        ret = send(clientfd,recvBuf,strlen(recvBuf), 0);
        if(ret != strlen(recvBuf))
        {
          std::cout << "error when send data . "<< std::endl;

        }
        else
        {
          std::cout <<" send data successfully."<<std::endl;
        }
      }
      else
      {
        std::cout <<" error when recv data.."<< std::endl;
      }

      //close(clientfd);

      clientfds.push_back(clientfd);
    }
  }

  close(listenfd);

  return 0;
}
