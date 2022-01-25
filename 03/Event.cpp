#include <Windows.h>
#include <string>
#include <iostream>

bool g_bTaskComplete = false;
std::string g_TaskResult;
#if 0
DWORD __stdcall WorkThreadProc(LPVOID lpThreadParameter)
{
	Sleep(3000);
	g_TaskResult = "task complete.";
	g_bTaskComplete = true;

	return 0;
}


int main()
{
	HANDLE hWorkerThread = CreateThread(NULL, 0, WorkThreadProc, NULL, 0, NULL);
	while (true)
	{
		if (g_bTaskComplete)
		{
			std::cout << g_TaskResult << std::endl;
			break;
		}
		else
			std::cout << "Task in progress ... " << std::endl;
	}
	CloseHandle(hWorkerThread);

	return 0;
}

#else

HANDLE g_TaskEvent = NULL;

DWORD __stdcall WorkerThreadProc(LPVOID lpThreadParameter)
{
	Sleep(3000);
	g_TaskResult = "task complete.";
	g_bTaskComplete = true;

	SetEvent(g_TaskEvent);

	return 0;
}

int main()
{
	g_TaskEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE hWorkerThread = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, NULL);
	DWORD dwResult = WaitForSingleObject(hWorkerThread, INFINITE);
	if (dwResult == WAIT_OBJECT_0)
		std::cout << g_TaskResult << std::endl;

	CloseHandle(hWorkerThread);
	CloseHandle(g_TaskEvent);

	return 0;
}
#endif // 0


