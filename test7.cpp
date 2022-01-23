#include <thread>
#include <memory>
#include <stdio.h>


class Thread
{
public:
	Thread() {}
	~Thread() {}
	void start()
	{
		m_stopped = false;
		m_spThread.reset(new std::thread(&Thread::threadFunc, this, 8888, 9999));
	}

	void stop()
	{
		m_stopped = true;
		if (m_spThread)
		{
			if (m_spThread->joinable())
				m_spThread->join();
		}
	}
private:
	void threadFunc(int arg1, int arg2)
	{
		while (!m_stopped)
		{
			printf("thread function use instance method(%d,%d)\n",arg1,arg2);
		}
	}

private:
	std::shared_ptr<std::thread> m_spThread;
	bool		m_stopped;
};

int main()
{
	Thread myThread;
	myThread.start();

	while(true)
	{
		
	}

	return 0;
}