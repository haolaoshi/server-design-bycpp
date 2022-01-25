#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

int main(int argc,char* argv[])
{

  //1.create a socket
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if(listenfd == -1)
  {
    std::cout <<" create lisen socket error ."<< std::endl;
    return -1;
  }

  //2. init sockaddr 
  struct sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bindaddr.sin_port = htons(3000);

  if(bind(listenfd,(struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1)
  {
    std::cout <<" bind listen socket error. " << std::endl;
    return -1;
  }

  //3. listen
  if(listen(listenfd,SOMAXCONN) == -1)
  {
    std::cout << " listen socket failed."<<std::endl;
    return -1;
  }

  while(true)
  {
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);

    //4. accept client 
    int clientfd = accept(listenfd, (struct sockaddr*)&clientaddr,&clientlen);
    if(clientfd != -1)
    {
      char recvBuf[32] = {0};
      //5. recv data 
      int ret = recv(clientfd, recvBuf, 32, 0);

      if(ret > 0)
      {
        std::cout << " recv data from client , data : " << recvBuf << std::endl;
        //6. send data 
        ret = send(clientfd,recvBuf, strlen(recvBuf),0);
        if(ret != strlen(recvBuf))
          std::cout<< " send data error. " << std::endl;
        else
          std::cout << " send data successfully . (" << recvBuf <<")" << std::endl;
      }

      close(clientfd);
    }
  }
  //7. cose listen
  close(listenfd);
}
