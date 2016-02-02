#define _WINSOCKAPI_

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <shlwapi.h>
#include <modbus.h>
#include "winhello.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "modbus.lib")

char configfile[] = ".\\winhello.ini";
int width = 300;
int height = 300;

BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int Value, LPCTSTR lpFileName)
{
	char Buffer[16];
	sprintf(Buffer, "%d", Value);
	return WritePrivateProfileString(lpAppName, lpKeyName, Buffer, lpFileName);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static OPENFILENAME ofn;
	static char szFile[260] = "hello";
	switch(msg)
	{
		case WM_CONTEXTMENU:
			TrackPopupMenu(GetSubMenu(GetMenu(hwnd),0),TPM_LEFTALIGN,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam),0,hwnd,NULL);
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_EXIT:
					DestroyWindow(hwnd);
				break;
				case ID_OPEN:
					;
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hwnd;
					ofn.lpstrFile = szFile;
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = "Text\0*.TXT\0All\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
					if (GetOpenFileName(&ofn) == TRUE)
					{
						if (_stricmp(".txt", PathFindExtension(ofn.lpstrFile))==0)
							MessageBox(hwnd,"Text file","message",MB_OK);
						//developmassive(ofn.lpstrFile);
					}
					break;
				case 2:
					;
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hwnd;
					ofn.lpstrFile = szFile;
					//ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = "Text\0*.TXT\0All\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
					if (GetSaveFileName(&ofn) == TRUE)
					{
						if (_stricmp(".txt", PathFindExtension(ofn.lpstrFile)) == 0)
							MessageBox(hwnd, "Text file", "message", MB_OK);
						//developmassive(ofn.lpstrFile);
					}
					break;
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
					if (GetMenuState(GetMenu(hwnd), LOWORD(wParam), MF_BYCOMMAND) & MF_CHECKED)
						CheckMenuItem(GetMenu(hwnd), LOWORD(wParam), MF_BYCOMMAND | MF_UNCHECKED);
					else
						CheckMenuItem(GetMenu(hwnd),LOWORD(wParam),MF_BYCOMMAND|MF_CHECKED);
				break;
			}
		break;
		case WM_CREATE:
			DragAcceptFiles(hwnd, TRUE);
			break;
		case WM_DESTROY:
			WritePrivateProfileInt("window","height",height,configfile);
			WritePrivateProfileInt("window","width",width,configfile);
			PostQuitMessage(0);
		break;
		case WM_DROPFILES:
			TCHAR lpszFile[MAX_PATH] = { 0 };
			UINT uFile = 0;
			HDROP hDrop = (HDROP)wParam;

			uFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);
			if (uFile != 1) {
				MessageBox(hwnd, "Dropping multiple files is not supported.", NULL, MB_ICONERROR);
				DragFinish(hDrop);
				break;
			}
			lpszFile[0] = '\0';
			if (DragQueryFile(hDrop, 0, lpszFile, MAX_PATH))
			{
				MessageBox(NULL, lpszFile, NULL, MB_ICONINFORMATION);
			}

			DragFinish(hDrop);
			break;
		case WM_SIZE:
			width = LOWORD(lParam);
			height = HIWORD(lParam);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;
	HWND hwnd;
	WNDCLASS wc = {0,WindowProc,0,0,hInstance,LoadIcon(hInstance, "Window"),LoadCursor(NULL, IDC_ARROW),(HBRUSH)(COLOR_WINDOW+1),"Menu","MainWindowClass"};
	RegisterClass(&wc);
	hwnd = CreateWindow("MainWindowClass","Window",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,GetPrivateProfileInt("window","width",CW_USEDEFAULT,configfile),GetPrivateProfileInt("window","height",CW_USEDEFAULT,configfile),NULL,NULL,hInstance,NULL);
	ShowWindow(hwnd,SW_SHOWDEFAULT);
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return 0;
}
