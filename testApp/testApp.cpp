#include <windows.h>
#include <stdio.h>

typedef int (*card_authorize9)(char* track2, struct auth_answer9* auth_answer);

int main()
{
    printf("start\n");

    HMODULE pilotNt = LoadLibraryA("pilot_nt.dll");

    if (!pilotNt)
    {
        printf("pilot_nt.dll not found\n");
        return -1;
    }

    try {
        card_authorize9 function = (card_authorize9)GetProcAddress(pilotNt, "card_authorize9");
        if (!function)
        {
            printf("pilot_nt.dll not found\n");
            return -2;
        }
    }
    catch (...)
    {
        FreeLibrary(pilotNt);
    }

    /*int msgboxID = MessageBox(
        NULL,
        L"Sdelat oplaty ?",
        L"Okoshko",
        MB_ICONEXCLAMATION | MB_YESNO
    );

    if (msgboxID == IDYES)
    {
        return 321;
    }
    printf("stop\n");*/

    //printf("starting to sleep...\n");
    //Sleep(3000); // Sleep three seconds
    //printf("sleep ended\n");
    return 123;
}
