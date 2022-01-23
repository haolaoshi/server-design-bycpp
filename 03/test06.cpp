//multi-thread  use c++11 std::thread 
//
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <thread>

#define TIME_FILENAME "time.txt"

void FileThreadProc()
{
  time_t now = time(NULL);
  struct tm* t = localtime(&now);
  char timeStr[32] = {0};
  sprintf(timeStr,"%04d/%02d/%02d %02d:%02d:%02d",
      t->tm_year + 1900,
      t->tm_mon + 1,
      t->tm_mday,
      t->tm_hour,
      t->tm_min,
      t->tm_sec);

  FILE* fp = fopen(TIME_FILENAME,"wb");
  if(fp == NULL)
  {
    printf("cannot open file \n");
    return ;
  }

  size_t sizeToWrite = strlen(timeStr) + 1;
  size_t ret = fwrite(timeStr,1,sizeToWrite,fp);
  if(ret != sizeToWrite)
  {
    printf("Write failed.\n");
  }

  fclose(fp);
}

int main()
{
  std::thread t(FileThreadProc);
  if(t.joinable())
    t.join();

  FILE* fp= fopen(TIME_FILENAME,"rb");
  if(fp == NULL)
  {
    printf("ope file failed\n");
    return 1;
  }

  char buf[32] = {0};
  size_t ret = fread(buf,1,32,fp);
  if(ret == 0)
  {
    printf("read file failed.");
    fclose(fp);
    return 3;
  }

  printf("%s\n",buf);
  fclose(fp);

  return 0;
}
