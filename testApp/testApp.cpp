#include <windows.h>
#include <stdio.h>

int main()
{
    printf("start\n");
    int msgboxID = MessageBox(
        NULL,
        L"Sdelat oplaty ?",
        L"Okoshko",
        MB_ICONEXCLAMATION | MB_YESNO
    );

    if (msgboxID == IDYES)
    {
        return 321;
    }
    printf("stop\n");

    //printf("starting to sleep...\n");
    //Sleep(3000); // Sleep three seconds
    //printf("sleep ended\n");
    return 123;
}
