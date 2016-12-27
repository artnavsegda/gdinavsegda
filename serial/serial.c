#include <stdio.h>
#include <windows.h>

struct a5framestruct {
	short currentpmt;
	short valueamp1;
	short valueamp2;
	short valueamp3;
	short battery;
	short autosampler;
	short temperature;
	short currentbattery;
	char reserved1;
	char reserved2;
	char reserved3;
	char reserved4;
	char voltagepmt;
};

struct afframestruct {
	unsigned char coefficentamp3;
	unsigned char coefficentamp2;
	unsigned char coefficentamp1;
	short dark_current;
	short biasamp3;
	short biasamp2;
	short biasamp1;
	short switch_threshhold;
	short coefficentcalib;
	unsigned char zerobias;
};

struct aeframestruct {
	unsigned char timeaverage;
	unsigned char timezero;
	unsigned char timecumulation;
	unsigned char timeintegration;
	short lowerthreshold;
	short upperthreshold;
};

struct a0framestruct {
	char devicenumber[4];
};

struct a1framestruct {
	char calibrationdate[24];
};

struct adframestruct {
	short operatinglamp1;
	short operatinglamp2;
};

struct aaframestruct {
	short linearization;
};

struct a2framestruct {
	unsigned char language;
	unsigned char configuration;
};

struct a7framestruct {
	unsigned char devicetype;
};

struct a4framestruct {
	short valveoff;
	short valveon;
};

struct a6framestruct {
	short fractional;
	short main;
};

struct frame75struct {
	short sensivity;
	short outputwindow;
	short windowoffset;
};

struct frame76struct {
	short pmtsupplyshift;
};

struct frame77struct {
	short pmtsupplysensivity;
};

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

void sendcommand(HANDLE serial, unsigned char marker, unsigned char *frame, int framelength)
{
	int numwrite;
	unsigned char markerframe[1], checksum[1];
	markerframe[0] = marker;
	WriteFile(serial, markerframe, 1, &numwrite, NULL);
	Sleep(20);
	WriteFile(serial, frame, framelength, &numwrite, NULL);
	checksum[0] = genchecksum(frame, framelength);
	WriteFile(serial, checksum, 1, &numwrite, NULL);
}

void transfermode(HANDLE serial)
{
	unsigned char frame[1];
	ZeroMemory(frame, 1);
	frame[0] = 0x01;
	sendcommand(serial, 0xCA, frame, 1);
}

int readframe(HANDLE serial, int framelength, void *frame)
{
	unsigned char checksum[1];
	int numread;
	ReadFile(serial, frame, framelength, &numread, NULL);
	printf("read frame %d\n", numread);
	ReadFile(serial, checksum, 1, &numread, NULL);
	printf("read checksum %d\n", numread);
	printf("checksum %d\n", checksum[0]);
	if (genchecksum(frame, framelength) == checksum[0])
	{
		printf("valid frame\n");
		return 0;
	}
	return 1;
}

int detecttransfersize(HANDLE serial)
{
	int counter = 0;
	int numread;
	unsigned char frame[22];
	while (TRUE)
	{
		ReadFile(serial, frame, 1, &numread, NULL);
		if (numread == 1)
		{
			if (frame[0] != 0xA5)
				counter++;
			else
				return counter;
		}
	}
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
	unsigned char frame[22];
	int counter = 0;

	while (detecttransfersize(serial) != 22)
		transfermode(serial);

	/*while (TRUE)
	{
		ReadFile(serial, frame, 1, &numread, NULL);
		if (numread == 1)
		{
			if (frame[0] != 0xA5)
			{
				counter++;
			}
			else
			{
				printf("a5 length %d\n", counter);
				if (counter == 20)
					transfermode(serial);
				if (counter == 22)
					break;
				counter = 0;
			}
		}
	}*/

	/*ReadFile(serial, frame, 22, &numread, NULL);
	printf("read %d\n", numread);*/
	struct a5framestruct a5frame;
	struct afframestruct afframe;
	struct aeframestruct aeframe;
	struct a0framestruct a0frame;
	struct a1framestruct a1frame;
	struct adframestruct adframe;
	struct aaframestruct aaframe;
	struct a2framestruct a2frame;
	struct a7framestruct a7frame;
	struct a4framestruct a4frame;
	struct a6framestruct a6frame;
	struct frame75struct frame75;
	struct frame76struct frame76;
	struct frame77struct frame77;
	long a5counter = 0;
	long afcounter = 0;
	long aecounter = 0;
	long a0counter = 0;
	long a1counter = 0;
	long adcounter = 0;
	long aacounter = 0;
	long a2counter = 0;
	long a7counter = 0;
	long a4counter = 0;
	long a6counter = 0;
	long counter75 = 0;
	long counter76 = 0;
	long counter77 = 0;

	readframe(serial, 21, &a5frame);

	while (TRUE)
	{
		ReadFile(serial, frame, 1, &numread, NULL);
		if (numread == 1)
		{
			printf("marker %X\n", frame[0]);
			switch (frame[0])
			{
			case 0xA5:
				if (!readframe(serial, 21, &a5frame))
				{
					printf("%6ld 0xA5 %6d %6d %6d %6d %6d %6d %6d %6d\n", a5counter++,
						a5frame.currentpmt,
						a5frame.valueamp1,
						a5frame.valueamp2,
						a5frame.valueamp3,
						a5frame.battery,
						a5frame.autosampler,
						a5frame.temperature,
						a5frame.currentbattery);
				}
				break;
			case 0xAF:
				if (!readframe(serial, 16, &afframe))
				{
					printf("%6ld 0xAF %6d %6d %6d %6d %6d %6d %6d %6d %6d %6d\n", afcounter++,
						afframe.coefficentamp3,
						afframe.coefficentamp2,
						afframe.coefficentamp1,
						afframe.dark_current,
						afframe.biasamp3,
						afframe.biasamp2,
						afframe.biasamp1,
						afframe.switch_threshhold,
						afframe.coefficentcalib,
						afframe.zerobias);
				}
			case 0xAE:
				if (!readframe(serial, 8, &aeframe))
				{
					printf("%6ld 0xAE %6d %6d %6d %6d %6d %6d\n", aecounter++,
						aeframe.timeaverage,
						aeframe.timezero,
						aeframe.timecumulation,
						aeframe.timeintegration,
						aeframe.lowerthreshold,
						aeframe.upperthreshold);
				}
				break;
			case 0xA0:
				if (!readframe(serial, 4, &a0frame))
				{
					printf("%6ld 0xA0 %s\n", a0counter++,
						a0frame.devicenumber);
				}
				break;
			case 0xA1:
				if (!readframe(serial, 25, &a1frame))
				{
					printf("%6ld 0xA1 %s\n", a1counter++,
						a1frame.calibrationdate);
				}
				break;
			case 0xAD:
				if (!readframe(serial, 4, &adframe))
				{
					printf("%6ld 0xAD %d %d\n", adcounter++,
						adframe.operatinglamp1,
						adframe.operatinglamp2);
				}
				break;
			case 0xAA:
				if (!readframe(serial, 2, &aaframe))
				{
					printf("%6ld 0xAA %d\n", aacounter++,
						aaframe.linearization);
				}
				break;
			case 0xA2:
				if (!readframe(serial, 2, &a2frame))
				{
					printf("%6ld 0xA2 %d %d\n", a2counter++,
						a2frame.language,
						a2frame.configuration);
				}
				break;
			case 0xA7:
				if (!readframe(serial, 1, &a7frame))
				{
					printf("%6ld 0xA7 %d\n", a7counter++,
						a7frame.devicetype);
				}
				break;
			case 0xA4:
				if (!readframe(serial, 4, &a4frame))
				{
					printf("%6ld 0xA4 %d %d\n", a4counter++,
						a4frame.valveoff,
						a4frame.valveon);
				}
				break;
			case 0xA6:
				if (!readframe(serial, 4, &a6frame))
				{
					printf("%6ld 0xA6 %d\n", a6counter++,
						a6frame.fractional,
						a6frame.main);
				}
				break;
			case 0x75:
				if (!readframe(serial, 6, &frame75))
				{
					printf("%6ld 0xA6 %d %d %d\n", counter75++,
						frame75.sensivity,
						frame75.outputwindow,
						frame75.windowoffset);
				}
				break;
			case 0x76:
				if (!readframe(serial, 2, &frame76))
				{
					printf("%6ld 0xA6 %d\n", counter76++,
						frame76.pmtsupplyshift);
				}
				break;
			case 0x77:
				if (!readframe(serial, 2, &frame77))
				{
					printf("%6ld 0xA6 %d\n", counter77++,
						frame77.pmtsupplysensivity);
				}
				break;
			default:
				while (detecttransfersize(serial) != 22)
					transfermode(serial);
				readframe(serial, 21, &a5frame);
				break;
			}
		}
	}

	/*unsigned char marker[1];

	while (marker[0] != 0xA5)
	{
		ReadFile(serial, marker, 1, &numread, NULL);
	}

	printf("1 read %d marker 0x%X\n", numread, marker[0]);

	unsigned char buffer[21];

	ReadFile(serial, buffer, 21, &numread, NULL);

	printf("2 read %d\n", numread);

	printf("%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15], buffer[16], buffer[17], buffer[18], buffer[19], buffer[20]);

	printf("calculated checksum %X\n", genchecksum(buffer, 21));

	unsigned char checksum[1];

	ReadFile(serial, checksum, 1, &numread, NULL);

	printf("3 read %d\n", numread);

	printf("recieved checksum %X\n", checksum[0]);*/

	/*unsigned char frame[1];

	while (TRUE)
	{
		ReadFile(serial, frame, 1, &numread, NULL);
		if (numread == 1)
			printf("%d: %X \n", GetTickCount() , frame[0]);
	}*/
    return 0;
}
