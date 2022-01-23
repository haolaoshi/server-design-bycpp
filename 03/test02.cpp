#include <stdio.h>
#include <thread>

void threadproc()
{
	while(true)
	{
		printf("i am a new thread\n");

	}
}

void threadproc2(int a,int b)
{
	while(true)
	{
		printf("i am 2\n");
	}
}


int main()
{
	std::thread t1(threadproc);
	std::thread t2(threadproc2,1,2);
	
	while(true)
	{

	}
	return 0;
}
