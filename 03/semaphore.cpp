#include <Windows.h>
#include <string>
#include <list>
#include <iostream>
#include <time.h>

HANDLE g_hMsgSemaphore;
std::list<std::string> g_listChatMsgs;
CRITICAL_SECTION g_csMsg;

DWORD __stdcall WorkerThreadProc(LPVOID lpThreadParameter)
{
	int nMsgIndex = 0;
	while (true)
	{
		EnterCriticalSection(&g_csMsg);
		int count = rand() % 4 + 1;
		for (int i = 0; i < count; i++)
		{
			nMsgIndex++;
			SYSTEMTIME st;
			GetLocalTime(&st);
			char szCharMsg[64] = { 0 };
			sprintf_s(szCharMsg, "[%04d-%02d-%02d %02d:%02d:%02d:%03d] A new Msg,No.%d",
				st.wYear, st.wMonth, st.wDay,
				st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
				nMsgIndex);
			g_listChatMsgs.emplace_back(szCharMsg);
		}
		LeaveCriticalSection(&g_csMsg);
		ReleaseSemaphore(g_hMsgSemaphore, count, NULL);

	}
	return 0;
}

DWORD __stdcall ParseThreadCall(LPVOID lpThreadParameter)
{
	DWORD dwThreadID = GetCurrentThreadId();
	std::string current;

	while (true)
	{
		if (WaitForSingleObject(g_hMsgSemaphore, INFINITE) == WAIT_OBJECT_0)
		{
			EnterCriticalSection(&g_csMsg);
			if (!g_listChatMsgs.empty())
			{
				current = g_listChatMsgs.front();
				g_listChatMsgs.pop_front();
				std::cout << "Thread: " << dwThreadID << "  parse msg: " << current
					<< std::endl;

			}
			LeaveCriticalSection(&g_csMsg);
		}
	}
	return 0;
}

int main()
{
	srand(time(NULL));
	InitializeCriticalSection(&g_csMsg);
	g_hMsgSemaphore = CreateSemaphore(NULL, 0, INT_MAX, NULL);
	HANDLE hNetThread = CreateThread(NULL, 0, WorkerThreadProc, NULL,0, NULL);
	
	HANDLE hWorkerThreads[4];
	for (int i = 0; i < 4; i++)
		hWorkerThreads[i] = CreateThread(NULL, 0, ParseThreadCall, NULL, 0, NULL);

	for (int i = 0; i < 4; i++)
	{
		WaitForSingleObject(hWorkerThreads[i], INFINITE);
		CloseHandle(hWorkerThreads[i]);
	}

	WaitForSingleObject(hNetThread, INFINITE);

	CloseHandle(hNetThread);
	CloseHandle(g_hMsgSemaphore);

	DeleteCriticalSection(&g_csMsg);

	return 0;
}