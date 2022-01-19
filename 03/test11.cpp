#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>


pthread_mutex_t mymutex;

void* worker_thread(void* param)
{
  pthread_t thread_id = pthread_self();

  printf("thread start , id = %d\n",thread_id);


  while(true)
  {
    int ret = pthread_mutex_lock(&mymutex);
    if(ret == EDEADLK)
      printf("EDEADLK , ID=%d\n",thread_id);
    else
      printf("ret = %d, id = %d\n",ret ,thread_id);
    sleep(1);
  }

  return NULL;
}


int main()
{
  pthread_mutexattr_t mutex_attr;
  pthread_mutexattr_init(&mutex_attr);
  pthread_mutexattr_settype(&mutex_attr,PTHREAD_MUTEX_ERRORCHECK);
  pthread_mutex_init(&mymutex,&mutex_attr);

  int ret = pthread_mutex_lock(&mymutex);
  printf("main ret = %d\n",ret);

  pthread_t id[5];

  for(int i = 0 ; i < 5; i++)
    pthread_create(&id[i],NULL,worker_thread,NULL);

  for(int i = 0 ; i < 5; i++)
    pthread_join(id[i],NULL);

  pthread_mutex_destroy(&mymutex);
  pthread_mutexattr_destroy(&mutex_attr);

  return 0;
}
