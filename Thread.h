#pragma once
 

#include <Windows.h>


#ifdef WIN32
typedef HANDLE THREAD_HANDLE;
#else 
typedef pthread_t	THREAD_HANDLE;
#endif  

class CThread
{
public:
	CThread();
	virtual ~CThread();
	virtual bool Create();
	THREAD_HANDLE GetHandle();
	void OSSleep(int nSeconds);
	void Sleeps(int nMilliseconds);
	bool Join();
	bool IsCurrentThread();
	void ExitThread();
private:
#ifdef WIN32
	static DWORD WINAPI _ThreadEntry(LPVOID pParam);
#else 
	static void* _ThreadEntry(void* pParam);
#endif

	virtual bool InitInstance();
	virtual void ExitInstance();
	virtual void Run() = 0;
private:
	THREAD_HANDLE m_hThread;
	DWORD	m_IDThread;
};

