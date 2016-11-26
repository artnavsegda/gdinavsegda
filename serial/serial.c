#include <stdio.h>
#include <windows.h>

int main()
{
	printf("hello world\n");
	HANDLE serial = CreateFile("COM4", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (serial == INVALID_HANDLE_VALUE);
	{
		printf("cannot open serial\n");
		exit(1);
	}
    return 0;
}
