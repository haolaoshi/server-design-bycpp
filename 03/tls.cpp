#include <Windows.h>
#include <iostream>

__declspec(thread) int g_mydata = 1;

DWORD __stdcall WorkerThread(LPVOID lParam)
{
	while (true)
	{
		++g_mydata;
		Sleep(1000);
	}
	return 0;
}

DWORD __stdcall WorkerThread2(LPVOID lParam)
{
	//TLS local mem 
	while (true)
	{
		std::cout << "g_mydata=" << g_mydata << ",Thread id:" << GetCurrentThreadId() <<
			std::endl;
		Sleep(1000);
	}
	return 0;
}

int main()
{
	HANDLE hWorkerThread[2];
	hWorkerThread[0] = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
	hWorkerThread[1] = CreateThread(NULL, 0, WorkerThread2, NULL, 0, NULL);
	CloseHandle(hWorkerThread[0]);
	CloseHandle(hWorkerThread[1]);

	while (true)
	{
		Sleep(1000);
	}
}