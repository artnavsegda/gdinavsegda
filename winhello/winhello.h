#include <windows.h>

#define ID_EXIT 0
#define ID_DIALOG 1
#define ID_SAVE 2
#define ID_OPEN 3

BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int Value, LPCTSTR lpFileName);