#include <stdio.h>
#include <windows.h>

int main()
{
	printf("serial\n");
	HANDLE serial = CreateFile("COM1", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (serial == INVALID_HANDLE_VALUE);
	{
		printf("cannot open serial\n");
		exit(1);
	}
	DCB serialparams = { .BaudRate = CBR_9600,.ByteSize = 8,.StopBits = ONESTOPBIT,.Parity = NOPARITY };
	if (!SetCommState(serial, &serialparams))
	{
		printf("cannot set serial params\n");
	}
	unsigned char buffer[1];
	int numread;
	while (TRUE)
	{
		ReadFile(serial, buffer, 1, &numread, NULL);
		printf("%X ", buffer[1]);
	}
    return 0;
}
