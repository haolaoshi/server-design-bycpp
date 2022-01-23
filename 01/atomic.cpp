#include <atomic>
#include <stdio.h>

int main()
{
	std::atomic<int> value;
	value = 99;
	printf("value = %d\n", (int)value);

	value++;
	printf("value = %d\n", (int)value);

	return 0;
}