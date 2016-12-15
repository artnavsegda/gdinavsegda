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

unsigned char genchecksum(unsigned char *massive, int sizeofmassive)
{
	unsigned char checksum = 0;
	for (int i = 0; i<sizeofmassive; i++)
		checksum = checksum + massive[i];
	return checksum;
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

	int numread;
	WriteFile(serial, "\xAF", 1, &numread, NULL);

	unsigned char marker[1];

	do {
		ReadFile(serial, marker, 1, &numread, NULL);
	} while (numread != 1);

	printf("1 read %d\n", numread);

	unsigned char buffer[16];

	ReadFile(serial, buffer, 16, &numread, NULL);

	printf("2 read %d\n", numread);

	printf("calculated checksum %X\n", genchecksum(buffer, 16));

	unsigned char checksum[1];

	do {
		ReadFile(serial, checksum, 1, &numread, NULL);
	} while (numread != 1);

	printf("3 read %d\n", numread);

	printf("recieved checksum %X\n", checksum[0]);

	/*while (TRUE)
	{
		ReadFile(serial, buffer, 1, &numread, NULL);
		if (numread == 1)
			printf("%d: %X \n", GetTickCount() ,buffer[0]);
	}*/
    return 0;
}
