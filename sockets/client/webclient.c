#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#pragma comment(lib, "Wsock32.lib")

const char ask[] = "GET / HTTP/1.0\n\n";

void oshibka(char *oshibkaname)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s: %s\n", oshibkaname, lpMsgBuf);
	LocalFree(lpMsgBuf);
	exit(1);
}

int main()
{
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

	int numwrite = send(sock,ask,strlen(ask),0);

	if (numwrite == SOCKET_ERROR)
	{
		oshibka("send");
	}
	else
	{
		printf("send %d bytes ok\n", numwrite);
	}

	if (shutdown(sock, 2) == SOCKET_ERROR)
	{
		oshibka("shutdown");
	}
	else
	{
		printf("shutdown ok\n");
	}
	closesocket(sock);

	return 0;
}
