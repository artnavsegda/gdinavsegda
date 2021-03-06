#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#pragma comment(lib, "Wsock32.lib")

void oshibka(char *oshibkaname)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s: %s\n", oshibkaname, lpMsgBuf);
	LocalFree(lpMsgBuf);
	exit(1);
}

struct askreadregstruct {
	unsigned short firstreg;
	unsigned short regnumber;
};

struct reqreadstruct {
	unsigned char bytestofollow;
	unsigned char bytes[254];
};

struct writeregstruct {
	unsigned short regaddress;
	unsigned short regvalue;
};

struct writemulticoilstruct {
	unsigned short firstreg;
	unsigned short regnumber;
	unsigned char bytestofollow;
	unsigned char coils[256];
};

struct writemultiregstruct {
	unsigned short firstreg;
	unsigned short regnumber;
	unsigned char bytestofollow;
	unsigned short registers[127];
};

union pdudataunion {
	struct askreadregstruct askreadregs;
	struct reqreadstruct reqread;
	struct writeregstruct writereg;
	struct writemulticoilstruct writemulticoil;
	struct writemultiregstruct writemultireg;
	unsigned short words[127];
	unsigned char bytes[254];
};

struct pduframestruct {
	unsigned char unitid;
	unsigned char fncode;
	union pdudataunion data;
	//	unsigned short data[256];
};

struct mbframestruct {
	unsigned short tsid;
	unsigned short protoid;
	unsigned short length;
	struct pduframestruct pdu;
};

struct mbframestruct askmbframe;

// example modbus data table
unsigned short offset = 20; //starting postion in register data
unsigned short table[100]; //actual registers data
unsigned short amount = 100; //maximum allowed position to read


int main()
{
	int buf[100];
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	else
	{
		printf("WSAStartup ok\n");
	}

	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET)
	{
		oshibka("socket");
		WSACleanup();
		return 1;
	}
	else
	{
		printf("socket ok\n");
	}

	struct sockaddr_in server = {
		.sin_family = AF_INET,
		.sin_port = htons(502),
		.sin_addr.s_addr = INADDR_ANY
	};

	if (bind(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		oshibka("bind");
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("bind ok\n");
	}

	if (listen(sock, 10) == SOCKET_ERROR)
	{
		oshibka("listen");
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("listen ok\n");
	}

	while (1)
	{
		SOCKET msgsock = accept(sock, NULL, NULL);
		if (msgsock == INVALID_SOCKET)
		{
			oshibka("accept");
			closesocket(sock);
			WSACleanup();
			return 1;
		}
		else
		{
			printf("accept ok\n");
		}

		int numread = recv(msgsock, &askmbframe, 6, 0);
		if (numread == SOCKET_ERROR)
		{
			oshibka("1 recv");
		}
		else
		{
			printf("1 recv %d bytes\n", numread);
			printf("TS id: %d\n", ntohs(askmbframe.tsid));
			printf("Protocol id: %d\n", ntohs(askmbframe.protoid));
			printf("Length: %d\n", ntohs(askmbframe.length));
		}

		numread = recv(msgsock, &askmbframe.pdu, ntohs(askmbframe.length), 0);
		if (numread == SOCKET_ERROR)
		{
			oshibka("2 recv");
		}
		else
		{
			printf("2 recv %d bytes\n", numread);
			printf("Unit id: %d\n", askmbframe.pdu.unitid);
			printf("Function code: %d\n", askmbframe.pdu.fncode);
			for (int i = 0; i<(numread - 2) / 2; i++)
				printf("%u ", ntohs(askmbframe.pdu.data.words[i]));
			printf("\n");
		}

		int firstrequest = 0;
		int requestnumber = 0;

		switch (askmbframe.pdu.fncode)
		{
		case 1:
		case 2:
			askmbframe.pdu.data.reqread.bytestofollow = ntohs(askmbframe.pdu.data.askreadregs.regnumber) / 8;
			if ((ntohs(askmbframe.pdu.data.askreadregs.regnumber) % 8)>0)
				askmbframe.pdu.data.reqread.bytestofollow++;
			askmbframe.length = htons(askmbframe.pdu.data.reqread.bytestofollow + 3);
			// fill all requested coil bytes with zeroes
			for (int i = 0; i < askmbframe.pdu.data.reqread.bytestofollow; i++)
				askmbframe.pdu.data.reqread.bytes[i] = 0x00;
			break;
		case 3:
		case 4:
			firstrequest = ntohs(askmbframe.pdu.data.askreadregs.firstreg);
			printf("Requesing register starting from: %d\n", firstrequest);
			requestnumber = ntohs(askmbframe.pdu.data.askreadregs.regnumber);
			printf("Number of registers requested : %d\n", requestnumber);
			askmbframe.pdu.data.reqread.bytestofollow = requestnumber * 2;
			askmbframe.length = htons(askmbframe.pdu.data.reqread.bytestofollow + 3);
			// fill every requested register with 0xABCD
			for (int i = 0; i < requestnumber;i++)
				((unsigned short *)&askmbframe.pdu.data.reqread.bytes)[i] = htons(0xABCD);
			break;
		case 5:
		case 6:
			//same as request
			break;
		case 15:
		case 16:
			askmbframe.length = htons(6);
			break;
		default:
			break;
		}

		int replylength = ntohs(askmbframe.length) + 6;
		printf("replylength %d\n", replylength);

		int numwrite = send(msgsock, &askmbframe, replylength, 0);
		if (numwrite == SOCKET_ERROR)
		{
			oshibka("send");
		}
		else
		{
			printf("send %d bytes ok\n", numwrite);
		}

		if (shutdown(msgsock, 2) == SOCKET_ERROR)
		{
			oshibka("shutdown");
			closesocket(msgsock);
			closesocket(sock);
			return 1;
		}
		else
		{
			printf("shutdown ok\n");
		}
		closesocket(msgsock);
	}
	closesocket(sock);

	return 0;
}
