#include <stdio.h>
#include <windows.h>

void oshibka(char *oshibkaname)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s: %s\n", oshibkaname, lpMsgBuf);
	LocalFree(lpMsgBuf);
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("name serial port\n");
		exit(1);
	}
	printf("serial\n");
	HANDLE serial = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (serial == INVALID_HANDLE_VALUE)
		oshibka("cannot open serial");
	DCB serialparams = { .BaudRate = CBR_9600,.ByteSize = 8,.StopBits = ONESTOPBIT,.Parity = NOPARITY };
	if (!SetCommState(serial, &serialparams))
		oshibka("cannot set serial params");
	COMMTIMEOUTS timeouts = {
		.ReadIntervalTimeout = 50,
		.ReadTotalTimeoutConstant = 50,
		.ReadTotalTimeoutMultiplier = 10,
		.WriteTotalTimeoutConstant = 50,
		.WriteTotalTimeoutMultiplier = 10
	};
	if (!SetCommTimeouts(serial, &timeouts))
		oshibka("cannot set serial timeouts");
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
