/*
 *
 * gethostbyname
 *//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

//extern int h_errno;
//

bool connect_to_server(const char* server, short port)
{
  int hsocket = socket(AF_INET, SOCK_STREAM, 0);
  if(hsocket == -1)
    return false;
  struct sockaddr_in addrsrv = {0};
  struct hostent* phostent = NULL;
  //unsigned int addr = 0;

  if(addrsrv.sin_addr.s_addr = inet_addr(server) == INADDR_NONE)
  {
    phostent = gethostbyname(server);
    if(phostent == NULL)
      return false;
    addrsrv.sin_addr.s_addr = *((unsigned long*))
  }
}
