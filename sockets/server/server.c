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

	if (sock == -1)
	{
		printf("socket error\n");
		WSACleanup();
		exit(0);
	}

	struct sockaddr_in server = {
		.sin_family = AF_INET,
		.sin_port = htons(1100),
		.sin_addr.s_addr = INADDR_ANY
	};

	if (bind(sock,(struct sockaddr *)&server, sizeof(server)) == -1)
	{
		printf("bind error");
		close(sock);
		WSACleanup();
		exit(0);
	}

	if (listen(sock, 10) == -1)
	{
		perror("listen error\n");
		close(sock);
		WSACleanup();
		exit(0);
	}

	while (1)
	{
		SOCKET msgsock = accept(sock, NULL, NULL);
		if (msgsock == -1)
		{
			printf("accept error");
			close(sock);
			WSACleanup();
			exit(0);
		}

	}

	return 0;
}
