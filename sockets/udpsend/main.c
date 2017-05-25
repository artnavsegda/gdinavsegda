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

char package[] = "hello world\n";

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

	SOCKET sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
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

	struct sockaddr_in other = {
		.sin_addr.s_addr = inet_addr("127.0.0.1"),
		.sin_family = AF_INET,
		.sin_port = htons(123)
	};

	int slen = sizeof(other);

	int numwrite = sendto(sock, package, sizeof(package), 0, (struct sockaddr *)&other, slen);

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
