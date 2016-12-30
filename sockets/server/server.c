#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#pragma comment(lib, "Wsock32.lib")

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
		printf("socket error\n");
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
		printf("bind error\n");
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
		printf("listen error\n");
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
			printf("accept error\n");
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
			printf("shutdown error\n");
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
