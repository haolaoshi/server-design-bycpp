/*
 * Block && nonblock
 *
 */
// client.



#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

#define SERVER_ADDRESS  "127.0.0.1"
#define SERVER_PORT     3000
#define SEND_DATA       "hello World"


int main(int argc,char* argv[])
{
  int clientfd = socket(AF_INET,SOCK_STREAM,0);
  if(clientfd == -1)
  {
    std::cout <<"error when create socket."<<std::endl;
    return -1;
  }

  struct sockaddr_in bindaddr;
  bindaddr.sin_family = AF_INET;
  bindaddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  bindaddr.sin_port = htons(3000);

  if(connect(clientfd,(struct sockaddr*)&bindaddr,sizeof(bindaddr)) == -1)
  {
    std::cout<<"error when connect to sever"<<std::endl;

    close(clientfd);
    return -1;
  }

  int count = 0;
  while(true)
  {
    int ret = send(clientfd,SEND_DATA,strlen(SEND_DATA),0);
    if(ret != strlen(SEND_DATA))
    {
      std::cout<<"error when send data"<<std::endl;
      break;
    }
    else
    {
      count++;
      std::cout<<"count:"<< count <<std::endl;
    }
  }

  close(clientfd);

  return 0;
}
