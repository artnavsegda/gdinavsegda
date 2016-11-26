#include <stdio.h>
#include <windows.h>

int main()
{
	printf("hello world\n");
	CreateFile("COM4", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    return 0;
}
