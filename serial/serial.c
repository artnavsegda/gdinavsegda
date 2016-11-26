#include <stdio.h>
#include <windows.h>

int main()
{
	LPVOID lpMsgBuf;
	printf("serial\n");
	HANDLE serial = CreateFile("COM1", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (serial == INVALID_HANDLE_VALUE)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
		printf("cannot open serial: %s\n", lpMsgBuf);
		LocalFree(lpMsgBuf);
		exit(1);
	}
	DCB serialparams = { .BaudRate = CBR_9600,.ByteSize = 8,.StopBits = ONESTOPBIT,.Parity = NOPARITY };
	if (!SetCommState(serial, &serialparams))
	{
		printf("cannot set serial params\n");
	}
	unsigned char buffer[23];
	int numread;
	while (TRUE)
	{
		ReadFile(serial, buffer, 23, &numread, NULL);
		for (int i = 0; i < 23; i++)
			printf("%X ", buffer[i]);
		printf("\n");
	}
    return 0;
}
