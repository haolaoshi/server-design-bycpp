/*
 *
 * BigEndian
 */
#include <iostream>

bool isNetByOrder()
{
  unsigned short mode = 0x1234;
  char* pmode = (char*)&mode;

  if(*pmode == 0x34)
    return false;
  return true;
}

int main()
{
  if(isNetByOrder()) 
    std::cout<<" BigEndian!!!";
  else
    std::cout<<" Little Endian!!!";

  return 0;
}
