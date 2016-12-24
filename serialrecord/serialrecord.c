#include <stdio.h>
#include <windows.h>

char szFile[260] = "record.txt";

OPENFILENAME ofn = {
	.lStructSize = sizeof(ofn),
	.lpstrFile = szFile,
	.nMaxFile = sizeof(szFile),
	.lpstrFilter = "Text\0*.TXT\0All\0*.*\0",
	.nFilterIndex = 1,
	.lpstrFileTitle = NULL,
	.nMaxFileTitle = 0,
	.lpstrInitialDir = NULL,
	.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR
};

void oshibka(char *oshibkaname)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	CharToOem((LPCSTR)lpMsgBuf, (LPSTR)lpMsgBuf);
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

	HANDLE serial = CreateFile(argv[1], GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (serial == INVALID_HANDLE_VALUE)
		oshibka("cannot open serial");

	if (GetSaveFileName(&ofn) != TRUE)
		exit(1);
	HANDLE outputfile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (outputfile == INVALID_HANDLE_VALUE)
		oshibka("cannot create record file");

	DCB serialparams = { .BaudRate = CBR_115200,.ByteSize = 8,.StopBits = ONESTOPBIT,.Parity = NOPARITY };
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

	while (TRUE)
	{
		ReadFile(serial, buffer, 1, &numread, NULL);
		if (numread == 1)
		{
			WriteFile(outputfile, buffer, 1, &numread, NULL);
			putchar(buffer[0]);
		}
	}

	CloseHandle(serial);
	CloseHandle(outputfile);

	return 0;
}