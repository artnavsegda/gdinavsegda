#include <stdio.h>
#include <windows.h>

char configfile[] = ".\\glhello.ini";
int xwidth, yheight;

BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int Value, LPCTSTR lpFileName)
{
	char Buffer[16];
	snprintf(Buffer, 16, "%d", Value);
	return WritePrivateProfileString(lpAppName, lpKeyName, Buffer, lpFileName);
}

