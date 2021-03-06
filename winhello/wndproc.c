#define _WINSOCKAPI_

#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>

#include "menu.h"
#include "wndproc.h"
#include "dialog.h"

extern char configfile[];
extern int width;
extern int height;

char szFile[260] = "hello";

OPENFILENAME ofn = {
	.lStructSize = sizeof(ofn),
	.lpstrFile = szFile,
	.nMaxFile = sizeof(szFile),
	.lpstrFilter = "Text\0*.TXT\0All\0*.*\0",
	.nFilterIndex = 1,
	.lpstrFileTitle = NULL,
	.nMaxFileTitle = 0,
	.lpstrInitialDir = NULL,
	.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ofn.hwndOwner = hwnd;
	switch (msg)
	{
	case WM_CONTEXTMENU:
		TrackPopupMenu(GetSubMenu(GetMenu(hwnd), 0), TPM_LEFTALIGN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, hwnd, NULL);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_EXIT:
			DestroyWindow(hwnd);
			break;
		case ID_OPEN:
			;
			if (GetOpenFileName(&ofn) == TRUE)
			{
				if (_stricmp(".txt", PathFindExtension(ofn.lpstrFile)) == 0)
					MessageBox(hwnd, "Text file", "message", MB_OK);
				//developmassive(ofn.lpstrFile);
			}
			break;
		case ID_SAVE:
			;
			if (GetSaveFileName(&ofn) == TRUE)
			{
				if (_stricmp(".txt", PathFindExtension(ofn.lpstrFile)) == 0)
					MessageBox(hwnd, "Text file", "message", MB_OK);
				//developmassive(ofn.lpstrFile);
			}
			break;
		case ID_DIALOG:
			DialogBox(GetModuleHandle(NULL), "MainDialog", hwnd, (DLGPROC)DialogFunc);
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
				CheckMenuItem(GetMenu(hwnd), LOWORD(wParam), MF_BYCOMMAND | MF_CHECKED);
			break;
		}
		break;
	case WM_CREATE:
		DragAcceptFiles(hwnd, TRUE);
		break;
	case WM_DESTROY:
		WritePrivateProfileInt("window", "height", height, configfile);
		WritePrivateProfileInt("window", "width", width, configfile);
		PostQuitMessage(0);
		break;
	case WM_DROPFILES:
		;
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