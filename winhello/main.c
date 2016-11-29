#define _WINSOCKAPI_

#include <windows.h>
#include "winhello.h"
#include "main.h"
#include "wndproc.h"

char configfile[] = ".\\winhello.ini";
int width = 300;
int height = 300;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;
	HWND hwnd;
	WNDCLASS wc = {
		.style = 0,
		.lpfnWndProc = WindowProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = LoadIcon(hInstance, "Window"),
		.hCursor = LoadCursor(NULL, IDC_ARROW),
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
		.lpszMenuName = "Menu",
		.lpszClassName = "MainWindowClass"
	};
	RegisterClass(&wc);
	hwnd = CreateWindow("MainWindowClass", "Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, GetPrivateProfileInt("window", "width", CW_USEDEFAULT, configfile), GetPrivateProfileInt("window", "height", CW_USEDEFAULT, configfile), NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return 0;
}