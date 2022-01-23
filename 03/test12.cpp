#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <list>
#include <semaphore.h>
#include <iostream>

class Task
{
public:
  Task(int taskID)
  {
    this->taskID = taskID;
  }
  void doTask()
  {
    std::cout<<"handle a task , taskID : " << taskID << ",THREAD-ID: "<<
      pthread_self() << std::endl;
  }

private:
  int taskID;
};

pthread_mutex_t   mymutex;
std::list<Task*>  tasks;
sem_t             mysemaphore;


void *tts_thread(void* param)
{
  Task* pTask = NULL;
  while(true)
  {
    if(sem_wait(&mysemaphore) != 0)
      continue;
    if(tasks.empty())
      continue;

    pthread_mutex_lock(&mymutex);
    pTask = tasks.front();
    tasks.pop_front();
    pthread_mutex_unlock(&mymutex);
    pTask->doTask();
    delete pTask;
  }
  return NULL;
}

void* db_thread(void* param)
{
  int taskID = 0;
  Task* pTask = NULL;

  while(true)
  {
    pTask = new Task(taskID);
    pthread_mutex_lock(&mymutex);
    tasks.push_back(pTask);
    std::cout<<"produce a task , taskID : " << taskID <<",thread-id: "<<
      pthread_self()<<std::endl;
    pthread_mutex_unlock(&mymutex);

    sem_post(&mysemaphore);
    taskID++;

    sleep(1);
  }

  return NULL;
}


int main()
{
  pthread_mutex_init(&mymutex,NULL);
  sem_init(&mysemaphore,0,0);

  pthread_t tts_thread_id[5];

  for(int i = 0 ; i < 5; i++)
    pthread_create(&tts_thread_id[i],NULL,tts_thread,NULL);

  pthread_t db_thread_id;
  pthread_create(&db_thread_id,NULL,db_thread,NULL);

  pthread_join(db_thread_id,NULL);
  for(int i = 0 ; i < 5; i++)
    pthread_join(tts_thread_id[i],NULL);

  sem_destroy(&mysemaphore);
  pthread_mutex_destroy(&mymutex);

  return 0;
}
