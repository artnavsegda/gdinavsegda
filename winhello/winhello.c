#define _WINSOCKAPI_

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <shlwapi.h>
#include <modbus.h>
#include "winhello.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "modbus.lib")

BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int Value, LPCTSTR lpFileName)
{
	char Buffer[16];
	sprintf(Buffer, "%d", Value);
	return WritePrivateProfileString(lpAppName, lpKeyName, Buffer, lpFileName);
}




