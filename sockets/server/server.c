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
		.sin_port = htons(1100),
		.sin_addr.s_addr = INADDR_ANY
	};

	if (bind(sock,(struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
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
		while (recv(msgsock, buf, 100, 0) != SOCKET_ERROR)
			;
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
