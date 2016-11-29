#include <windows.h>
#include "dialog.h"

BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char workstring[100];
	switch (message)
	{
	case WM_INITDIALOG:
//		for (int i = 1; i <= 13; i++)
//			CheckDlgButton(hwndDlg, 4 + i, col[i]);
//		for (int i = 1; i <= 13; i++)
//			SetDlgItemInt(hwndDlg, 24 + i, level[i], TRUE);

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 4:
			//if (!GetDlgItemText(hwndDlg, ID_ITEMNAME, szItemName, 80)) 
			//     *szItemName=0; 
			break;
		case ID_DIALOG_EXIT:
			EndDialog(hwndDlg, wParam);
			return TRUE;
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
//			col[LOWORD(wParam) - 4] = IsDlgButtonChecked(hwndDlg, LOWORD(wParam));
			break;
		}
		break;
	}
	return FALSE;
}
