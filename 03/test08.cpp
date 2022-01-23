#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

pthread_mutex_t mymutex;
int resourceNo = 0;

void*  worker_thread(void* param)
{
  pthread_t threadid = pthread_self();
  printf("thread start , id = %d\n",threadid);

  while(true)
  {
    pthread_mutex_lock(&mymutex);
    printf("mutex lock , resourceNo : %d, threaid %d \n",resourceNo,threadid);
    resourceNo++;
    printf("mutext un-lock, resourceNo : %d, threadid %d\n",resourceNo,threadid);
    pthread_mutex_unlock(&mymutex);
    sleep(1);
  }

  return NULL;
}

int main()
{
  pthread_mutexattr_t mutex_attr;
  pthread_mutexattr_init(&mutex_attr);
  pthread_mutexattr_settype(&mutex_attr,PTHREAD_MUTEX_NORMAL);
  pthread_mutex_init(&mymutex,&mutex_attr);

  pthread_t threadids[5];
  for(int i = 0; i < 5; i++)
    pthread_create(&threadids[i],NULL,worker_thread,NULL);

  for(int i =0; i < 5; i++)
    pthread_join(threadids[i],NULL);

  pthread_mutex_destroy(&mymutex);
  pthread_mutexattr_destroy(&mutex_attr);

  return 0;
}
