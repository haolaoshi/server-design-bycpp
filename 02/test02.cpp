#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#define WAVE_FILE_NAME  "1.wav"

DWORD WINAPI AdjustWav(LPVOID lpParameter)
{
	time_t now = time(NULL);
	struct tm  t;
	 
	 
	 gmtime_s( &t, &now) ;
 

	localtime_s(&t, &now);

	//localtime_s(now,&t);
	char timeStr[32] = { 0 };
	sprintf_s(timeStr, "%04d/%02d/%02d %02d:%02d:%02d",
		t.tm_year + 1900,
		t.tm_mon + 1,
		t.tm_mday,
		t.tm_hour,
		t.tm_min,
		t.tm_sec);

	FILE* fp;
	fopen_s(&fp, WAVE_FILE_NAME, "wb");
	if (fp == NULL)
	{
		printf("cannot oepn write file\n");
		exit(-2);
	}

	size_t sizeToWrite = strlen(timeStr) + 1;
	size_t n = fwrite(timeStr, 1, sizeToWrite, fp);
	if (n != sizeToWrite)
	{
		printf("write file error\n");
		exit(-3);
	}

	fclose(fp);
	return 2;
}

int main()
{
	DWORD dwFileThreadID;
	HANDLE hFileThread = CreateThread(NULL, 0, AdjustWav, NULL, 0, &dwFileThreadID);
	if (hFileThread == NULL)
	{
		printf("create thread failed\n");
		exit(-2);
	}

	WaitForSingleObject(hFileThread, INFINITE);

	FILE* fp;
	fopen_s(&fp, WAVE_FILE_NAME, "r");
	if (fp == NULL)
	{
		printf("read file err\n");
		return -2;
	}

	char buf[32] = { 0 };
	int sizeRead = fread(buf, 1, 32, fp);
	if (sizeRead == 0)
	{
		printf("read 0 \n");
		return -3;
	}

	printf("%s\n", buf);

	fclose(fp);

	return 0;
}