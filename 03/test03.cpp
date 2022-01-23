#include <stdio.h>
#include <thread>

void threadproc()
{
	while(true)
	{
		printf("i am a new thread\n");

	}
}

void  fun()
{
	std::thread t(threadproc);
	t.detach();
}


int main()
{

	fun();	
	while(true)
	{

	}
	return 0;
}
