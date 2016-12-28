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
		.sin_family = AF_INET,
		.sin_port = htons(1100)
	};

	InetPton(AF_INET, "127.0.0.1", &client.sin_addr);

	if (connect(sock, (struct sockaddr *)&client, sizeof(client)) == -1)
	{
		perror("connect error");
		close(sock);
		exit(0);
	}

	return 0;
}
