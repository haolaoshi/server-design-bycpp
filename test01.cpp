#include <process.h>
#include <stdio.h>

unsigned int __stdcall threadfun(void* args)
{
	while (true)
	{
		printf("i am a new thread\n");
	}
}

int main()
{
	unsigned int thread_id;
	_beginthreadex(0, 0, threadfun, 0, 0, &thread_id);
	while (true)
	{
		
	}

	return 0;
}