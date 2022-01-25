#include <Windows.h>
#include <iostream>
#include <list>

class Task
{
public:
	Task(int taskID)
	{
		this->taskID = taskID;
	}
	void doTask()
	{
		std::cout << "handle task id : " << taskID << ",THREAD ID : " <<
			GetCurrentThreadId() << std::endl;
	}
private:
	int taskID;
};


CRITICAL_SECTION myCriticalSection;
CONDITION_VARIABLE myConditionVariable;
std::list<Task*>  tasks;


DWORD WINAPI consumerThread(LPVOID param)
{
	Task* pTask = NULL;
	while (true)
	{
		EnterCriticalSection(&myCriticalSection);
		while (tasks.empty())
		{
			SleepConditionVariableCS(&myConditionVariable,&myCriticalSection,INFINITE);
		}
		pTask = tasks.front();
		tasks.pop_front();

		LeaveCriticalSection(&myCriticalSection);
		if (pTask == NULL)
			continue;
		pTask->doTask();
		delete pTask;

		pTask = NULL;
	}
	return 0;
}

DWORD WINAPI produceThread(LPVOID param)
{
	int taskID = 0;
	Task* pTask = NULL;

	while (true)
	{
		pTask = new Task(taskID);
		EnterCriticalSection(&myCriticalSection);
		tasks.push_back(pTask);
		std::cout << "produce a task : " << taskID << ",thread-id: " <<
			GetCurrentThreadId() << std::endl;
		LeaveCriticalSection(&myCriticalSection);
		WakeConditionVariable(&myConditionVariable);

		taskID++;

		Sleep(1000);
	}
	return 0;
}


int main()
{
	InitializeCriticalSection(&myCriticalSection);
	InitializeConditionVariable(&myConditionVariable);
	HANDLE consumerThreads[5];
	for (int i = 0; i < 5; i++)
		consumerThreads[i] = CreateThread(NULL, 0, consumerThread, NULL, 0, NULL);
	HANDLE producer = CreateThread(NULL, 0, produceThread, NULL, 0, NULL);
	WaitForSingleObject(producer, INFINITE);
	for (int i = 0; i < 5; i++)
		WaitForSingleObject(consumerThreads[i], INFINITE);
	DeleteCriticalSection(&myCriticalSection);

	return 0;
}