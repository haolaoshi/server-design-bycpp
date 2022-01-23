#include <pthread.h>
#include <unistd.h>
#include <iostream>


int resourceID = 0;
pthread_rwlock_t myrwlock;

void* read_thread(void* param)
{
  while(true)
  {
    pthread_rwlock_rdlock(&myrwlock);
    std::cout <<"read thread id : " << pthread_self() << ",resourceID : " << resourceID << std::endl;

    sleep(1);
    pthread_rwlock_unlock(&myrwlock);
  }

  return NULL;
}

void* write_thread(void* param)
{
  while(true)
  {
    pthread_rwlock_wrlock(&myrwlock);
    ++resourceID;

    std::cout<<" write thread id : " << pthread_self() <<",resourceID : " << resourceID <<std::endl;

    sleep(1);
    pthread_rwlock_unlock(&myrwlock);

  }

  return NULL;
}

int main()
{
  pthread_rwlockattr_t attr;
  pthread_rwlockattr_init(&attr);
  pthread_rwlockattr_setkind_np(&attr,PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
//PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP  
  pthread_rwlock_init(&myrwlock,&attr);

  pthread_t readThread[5];
  for(int i = 0 ; i < 5 ; i++)
    pthread_create(&readThread[i],NULL,read_thread,NULL);

  pthread_t writeThread;
  pthread_create(&writeThread,NULL,write_thread,NULL);
  pthread_join(writeThread,NULL);

  for(int i = 0 ; i < 5; i++)
    pthread_join(readThread[i],NULL);

  pthread_rwlock_destroy(&myrwlock);

  return 0;
}
