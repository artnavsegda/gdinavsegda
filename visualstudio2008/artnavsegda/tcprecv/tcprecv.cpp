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

struct sockaddr* resolve(char* hostname, char* port)
{
	static struct sockaddr_in client;
	struct hostent* host = gethostbyname(hostname);

	if (host != NULL)
		client.sin_addr.s_addr = *(u_long*)host->h_addr_list[0];
	else
		client.sin_addr.s_addr = inet_addr(hostname);
	client.sin_family = AF_INET;
	client.sin_port = htons(atoi(port));
	return (struct sockaddr*) &client;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		puts("usage: tcprecv <ipaddr> <port>");
		return 0;
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

	if (connect(sock, resolve(argv[1], argv[2]), sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		oshibka("connect");
		closesocket(sock);
		exit(0);
	}
	else
	{
		printf("connect ok\n");
	}

	char buf[100];

	int numread = recv(sock,buf,100,0);

    if (numread == -1)
	{
		oshibka("recv error");
		closesocket(sock);
		return 1;
	}
	else
	{
		printf("recv %d bytes\n",numread);
		for (int i=0; i<numread;i++)
		{
			printf("0x%02X ",buf[i]);
		}
		printf("\n");
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
