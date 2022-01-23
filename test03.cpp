#include <Windows.h>
#include <stdio.h>

int main()
{
	LONG nPreValue = 99;
	LONG nPostValue = InterlockedIncrement(&nPreValue);

	printf("nPreValue= %d, nPostValue = %d\n", nPreValue, nPostValue);

	return 0;
}