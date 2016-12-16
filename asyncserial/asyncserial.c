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
	HANDLE serial = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
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

	unsigned char buffer[1];
	int numread;

	HANDLE ghEvents[2];
	ghEvents[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	ghEvents[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
	OVERLAPPED osReader = { .hEvent = ghEvents[0] };
	OVERLAPPED osWriter = { .hEvent = ghEvents[1] };


	while (ReadFile(serial, buffer, 1, &numread, &osReader) == TRUE)
		if (numread == 1)
			printf("%X\n", buffer[0]);
	if (GetLastError() != ERROR_IO_PENDING)
		oshibka("read serial");

	while (WriteFile(serial, "\xAF", 1, &numread, &osWriter) == TRUE);
	if (GetLastError() != ERROR_IO_PENDING)
		oshibka("write serial");

	while (TRUE)
	{
		switch (WaitForMultipleObjects(2, ghEvents, FALSE, INFINITE))
		{
		case WAIT_OBJECT_0:
			GetOverlappedResult(serial, &osReader, &numread, TRUE);
			do {
				if (numread == 1)
					printf("%X\n", buffer[0]);
			} while (ReadFile(serial, buffer, 1, &numread, &osReader) == TRUE);
			if (GetLastError() != ERROR_IO_PENDING)
				oshibka("read serial");
			break;
		case WAIT_OBJECT_0+1:
			GetOverlappedResult(serial, &osWriter, &numread, TRUE);
			while (WriteFile(serial, "\xAA", 1, &numread, &osWriter) == TRUE);
			if (GetLastError() != ERROR_IO_PENDING)
				oshibka("write serial");
			break;
		default:
			break;
		}
	}

	return 0;
}
