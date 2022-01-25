#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <iostream>

class Task
{
public:
	Task(int taskID)
	{
		this->taskID = taskID;
	}
	~Task() = default;
	void doTask()
	{
		std::cout << "handle a tsk , id : " << taskID << ",thread: " <<
			std::this_thread::get_id() << std::endl;
	}
private:
	int taskID;
};

std::mutex   mymutex;
std::list<Task*>  tasks;
std::condition_variable mycv;

void* consumer_thread()
{
	Task* pTask = NULL;
	while (true)
	{
		std::unique_lock<std::mutex> guard(mymutex);
		while (tasks.empty())
		{
			mycv.wait(guard);
		}
		pTask = tasks.front();
		tasks.pop_front();
		if (pTask == NULL)
			continue;

		pTask->doTask();
		delete pTask;
		pTask = NULL;
	}
	return NULL;
}

void* produce_thread()
{
	int taskID = 0;
	Task* pTask = NULL;
	while (true)
	{
		pTask = new Task(taskID);
		{
			std::lock_guard<std::mutex> guard(mymutex);
			tasks.push_back(pTask);
			std::cout << "produce a task id : " << taskID << ",thread : " <<
				std::this_thread::get_id() << std::endl;

		}
		mycv.notify_one();
		taskID++;
		std::this_thread::sleep_for(std::chrono::seconds(1));

	}
	return NULL;
}

int main()
{
	std::thread c1(consumer_thread);
	std::thread c2(consumer_thread);
	std::thread c3(consumer_thread);
	std::thread c4(consumer_thread);
	std::thread c5(consumer_thread);

	std::thread p1(produce_thread);

	p1.join();

	c1.join();
	c2.join();
	c3.join();
	c4.join();
	c5.join();

	return 0;
}