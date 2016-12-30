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

	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET)
	{
		printf("socket error\n");
		WSACleanup();
		return 1;
	}

	struct sockaddr_in server = {
		.sin_family = AF_INET,
		.sin_port = htons(1100),
		.sin_addr.s_addr = INADDR_ANY
	};

	if (bind(sock,(struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("bind error");
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	if (listen(sock, 10) == SOCKET_ERROR)
	{
		perror("listen error\n");
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	while (1)
	{
		SOCKET msgsock = accept(sock, NULL, NULL);
		if (msgsock == INVALID_SOCKET)
		{
			printf("accept error");
			closesocket(sock);
			WSACleanup();
			return 1;
		}
		while (recv(msgsock, buf, 100, 0) != SOCKET_ERROR)
			;
		if (shutdown(msgsock, 2) == SOCKET_ERROR)
		{
			printf("shutdown error");
			closesocket(msgsock);
			closesocket(sock);
			return 1;
		}
		closesocket(msgsock);
	}
	closesocket(sock);

	return 0;
}
