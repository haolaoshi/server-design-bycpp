#include <pthread.h>
#include <unistd.h>
#include <stdio.h>


void *threadfunc(void* arg)
{
	while(1)
	{

		sleep(1);
		printf("Iam a new Thread\n");
	}
	
	return NULL;
}

int main()
{
  pthread_t threadid;
  pthread_create(&threadid,NULL,threadfunc,NULL);
  while(1)
  {
  }

  return 0;

}
