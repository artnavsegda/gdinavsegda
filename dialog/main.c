#include <windows.h>
#include "dialog.h"

BOOL CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hwndDlg, ID_DIALOG_TEXT ,"Hello");
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_DIALOG_TEXT:
			break;
		case ID_DIALOG_EXIT:
			DestroyWindow(hwndDlg);
			return TRUE;
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwndDlg);
		return TRUE;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
		break;
	}
	return FALSE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;
	HWND hwnd = CreateDialog(hInstance, "MainDialog", 0, DialogProc);
	ShowWindow(hwnd, nCmdShow);
	
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
	}
	return 0;
}