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
	if (argc < 2)
	{
		printf("name serial port and maybe save file\n");
		exit(1);
	}

	HANDLE serial = CreateFile(argv[1], GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (serial == INVALID_HANDLE_VALUE)
		oshibka("cannot open serial");

	char *recordfilename;
	if (argc >= 3)
		recordfilename = argv[2];
	else
		if (GetSaveFileName(&ofn) != TRUE)
			exit(1);
		else
			recordfilename = ofn.lpstrFile;

	HANDLE outputfile = CreateFile(recordfilename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (outputfile == INVALID_HANDLE_VALUE)
		oshibka("cannot create record file");

	DCB serialparams = {
		.DCBlength = sizeof(DCB),
		.BaudRate = CBR_115200,
		.fBinary = TRUE,
		.fParity = FALSE,
		.fOutxCtsFlow = FALSE,
		.fOutxDsrFlow = FALSE,
		.fDtrControl = DTR_CONTROL_ENABLE,
		.fDsrSensitivity = FALSE,
		.fTXContinueOnXoff = TRUE,
		.fOutX = FALSE,
		.fInX = FALSE,
		.fErrorChar = FALSE,
		.fNull = FALSE,
		.fRtsControl = RTS_CONTROL_ENABLE,
		.fAbortOnError = FALSE,
		.ByteSize = 8,
		.Parity = NOPARITY,
		.StopBits = ONESTOPBIT
	};
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

	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE)
		oshibka("GetStdHandle");
	FlushConsoleInputBuffer(hStdin);

	unsigned char buffer[1];
	int numread;
	INPUT_RECORD irInBuf[1];

	WriteFile(serial, "x", 1, &numread, NULL);
	Sleep(1000);

	PurgeComm(serial, PURGE_RXCLEAR);

	while (TRUE)
	{
		ReadFile(serial, buffer, 1, &numread, NULL);
		if (numread == 1)
		{
			WriteFile(outputfile, buffer, 1, &numread, NULL);
			putchar(buffer[0]);
		}
		PeekConsoleInput(hStdin, irInBuf, 1, &numread);
		if (numread == 1)
		{
			FlushConsoleInputBuffer(hStdin);
			if (irInBuf[0].EventType == KEY_EVENT && irInBuf[0].Event.KeyEvent.uChar.AsciiChar == 'q')
				break;
		}
	}

	CloseHandle(serial);
	CloseHandle(outputfile);

	return 0;
}