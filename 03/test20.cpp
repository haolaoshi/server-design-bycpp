#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT    3000
#define SEND_DATA      "helloworld"

int main(int argc, char* argv[])
{
  //1. create socket 
  int clientfd = socket(AF_INET, SOCK_STREAM,0);
  if(clientfd == -1)
  {
    std::cout <<" error create socket. " << std::endl;
    return -1;
  }

  //2. init sockaddr 
  struct sockaddr_in serveraddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  serveraddr.sin_port = htons(SERVER_PORT);

  if(connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
  {
    std::cout << " error when cnnect server. " << std::endl;
    return -1;
  }


  //3. send dat a 
  int ret = send(clientfd, SEND_DATA, strlen(SEND_DATA),0);
  if(ret != strlen(SEND_DATA))
  {
    std::cout << " error when send data. " << std::endl;
    return -1;
  }

  std::cout << " send data successfully . " << std::endl;

  char recvBuf[32] = {0};
  //4. recv data 
  ret = recv(clientfd, recvBuf, 32, 0);
  if(ret > 0)
    std::cout << "recv data " << recvBuf << std::endl;
  else
    std::cout << " recv error. " << std::endl;

  //5. close 
  close(clientfd);

  return 0;
}
