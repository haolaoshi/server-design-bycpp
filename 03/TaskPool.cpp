#include "TaskPool.h"
#include <chrono>
#include <functional>

TaskPool::TaskPool():m_bRunning(false)
{
}

TaskPool::~TaskPool()
{
	removeAllTasks();
}

void TaskPool::init(int threadNum)
{
	if (threadNum <= 0)
		threadNum = 5;
	m_bRunning = true;
	for (int i = 0; i < threadNum; i++)
	{
		std::shared_ptr<std::thread> spThread;
		spThread.reset(new std::thread(std::bind(&TaskPool::threadFunc,this)));
		m_threads.push_back(spThread);
	}
 
}

void TaskPool::stop()
{
	m_bRunning = false;
	m_cv.notify_all();
	for (auto& iter : m_threads)
	{
		if (iter->joinable())
			iter->join();
	}
}

void TaskPool::addTask(Task* tsk)
{
	std::shared_ptr<Task> spTask;
	spTask.reset();
	//lock_guard是一个互斥量包装程序，它提供了一种方便的RAII（Resource acquisition is initialization ）
	//风格的机制来在作用域块的持续时间内拥有一个互斥量。
	{
		std::lock_guard<std::mutex> guard(m_mutexList);
		m_taskList.push_back(spTask);
		std::cout << "add a task ." << std::endl;
	}
	m_cv.notify_one();
}

void TaskPool::removeAllTasks()
{
	{
		std::lock_guard<std::mutex> guard(m_mutexList);
		for (auto& iter : m_taskList)
			iter.reset();

		m_taskList.clear();
	}
}

void TaskPool::threadFunc()
{
	std::shared_ptr<Task> spTask;
	while (true)
	{
		{
			std::unique_lock<std::mutex> guard(m_mutexList);
			while (m_taskList.empty())
			{
				if (!m_bRunning)
					break;


				m_cv.wait(guard);
			}

			if (!m_bRunning)
				break;
			spTask = m_taskList.front();
			m_taskList.pop_front();
		}
		if (spTask == NULL)
			continue;
		spTask->doit();
		spTask.reset();

	}

	std::cout << "exit thread, thread id : " << std::this_thread::get_id() << std::endl;
}


int main()
{
	TaskPool thread_pool;
	thread_pool.init();

	Task* task = NULL;
	for (int i = 0; i < 10; i++)
	{
		task = new Task();
		thread_pool.addTask(task);
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));
	thread_pool.stop();

	return 0;
}