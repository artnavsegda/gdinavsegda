#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#pragma comment(lib, "Wsock32.lib")

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

	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET)
	{
		printf("socket error\n");
		WSACleanup();
		return 1;
	}

	struct sockaddr_in client = {
		.sin_addr.s_addr = inet_addr("127.0.0.1"),
		.sin_family = AF_INET,
		.sin_port = htons(1100)
	};

	if (connect(sock, (struct sockaddr *)&client, sizeof(client)) == SOCKET_ERROR)
	{
		printf("connect error\n");
		closesocket(sock);
		exit(0);
	}

	send(sock, "hello", 6, 0);

	shutdown(sock, 2);
	closesocket(sock);

	return 0;
}
