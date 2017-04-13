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

struct tcpframestruct {
	unsigned short tsid;
	unsigned short protoid;
	unsigned short length;
};

struct askreadregstruct {
	unsigned short firstreg;
	unsigned short regnumber;
};

struct writeregstruct {
	unsigned short regaddress;
	unsigned short regvalue;
};

struct reqreadbitstruct {
	unsigned char bytestofollow;
	unsigned char coils[256];
};

struct reqreadwordstruct {
	unsigned char bytestofollow;
	unsigned short registers[127];
};

union pdudataunion {
	struct askreadregstruct askreadregs;
	struct reqreadbitstruct reqreadcoils;
	struct reqreadwordstruct reqreadholdings;
	struct writeregstruct writereg;
	unsigned char bytes[254];
	unsigned short words[127];
};

struct pduframestruct {
	unsigned char unitid;
	unsigned char fncode;
	union pdudataunion data;
};

struct mbframestruct {
	unsigned short tsid;
	unsigned short protoid;
	unsigned short length;
	struct pduframestruct pdu;
};

struct mbframestruct mbframe = {
	.tsid = 0x0100,
	.protoid = 0x0000,
	.length = 0x0600,
	.pdu = {
	.unitid = 50
	}
};

struct mbframestruct askframe;

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("name function code and both values\n");
		return 1;
	}

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

	struct sockaddr_in client = {
		.sin_addr.s_addr = inet_addr("192.168.1.150"),
		.sin_family = AF_INET,
		.sin_port = htons(502)
	};

	if (connect(sock, (struct sockaddr *)&client, sizeof(client)) == SOCKET_ERROR)
	{
		oshibka("connect");
		closesocket(sock);
		exit(0);
	}
	else
	{
		printf("connect ok\n");
	}

	sscanf(argv[1], "%hhu", &mbframe.pdu.fncode);
	sscanf(argv[2], "%hu", &mbframe.pdu.data.words[0]);
	mbframe.pdu.data.words[0] = htons(mbframe.pdu.data.words[0]);
	sscanf(argv[3], "%hu", &mbframe.pdu.data.words[1]);
	mbframe.pdu.data.words[1] = htons(mbframe.pdu.data.words[1]);

	int numwrite = send(sock, &mbframe, 12, 0);

	if (numwrite = SOCKET_ERROR)
	{
		oshibka("send");
	}
	else
	{
		printf("send %d bytes ok\n", numwrite);
	}

	int numread = recv(sock, &askframe, 100, 0);
	if (numwrite == SOCKET_ERROR)
	{
		oshibka("1 recv");
	}
	else
	{
		printf("1 recv %d bytes\n", numread);
		printf("TS id: %d\n", ntohs(askframe.tsid));
		printf("Protocol id: %d\n", ntohs(askframe.protoid));
		printf("Length: %d\n", ntohs(askframe.length));
	}

	numread = recv(sock, &askframe.pdu, ntohs(askframe.length), 0);
	if (numwrite == SOCKET_ERROR)
	{
		oshibka("2 recv");
	}
	else
	{
		printf("2 recv %d bytes\n", numread);
		printf("Unit id: %d\n", askframe.pdu.unitid);
		printf("Function code: %d\n", askframe.pdu.fncode);
		for (int i = 0; i < numread - 2; i++)
			printf("0x%02x ", askframe.pdu.data.bytes[i]);
		printf("\n");
		switch (askframe.pdu.fncode)
		{
		case 1:
		case 2:
			printf("number of bytes: %d\n", askframe.pdu.data.reqreadcoils.bytestofollow);
			for (int i = 0; i < askframe.pdu.data.reqreadcoils.bytestofollow; i++)
				printf("0x%02hhX ", askframe.pdu.data.reqreadcoils.coils[i]);
			printf("\n");
			break;
		case 3:
		case 4:
			printf("number of registers: %d\n", askframe.pdu.data.reqreadholdings.bytestofollow / 2);
			for (int i = 0; i < askframe.pdu.data.reqreadholdings.bytestofollow / 2; i++)
				printf("0x%04hX ", askframe.pdu.data.reqreadholdings.registers[i]);
			printf("\n");
			break;
		case 5:
		case 6:
			printf("register number %u\n", ntohs(askframe.pdu.data.writereg.regaddress));
			printf("register value %u\n", ntohs(askframe.pdu.data.writereg.regvalue));
			break;
		}
	}

	if (shutdown(sock, 2) == SOCKET_ERROR)
	{
		oshibka("send");
	}
	else
	{
		printf("shutdown ok\n");
	}
	closesocket(sock);

	return 0;
}
