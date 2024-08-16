#include <windows.h>
#include <stdio.h>
#include "pilot_nt.h"

typedef int (*dynamic_function)(char* track2, struct auth_answer9* auth_answer);

int main(int argc, char* argv[])
{
    printf("start\n");

    int amount = atoi(argv[1]);

    printf("%d rubles", amount / 100);

    return -1;

    HMODULE pilotNt = LoadLibrary(L"pilot_nt.dll");

    if (!pilotNt)
    {
        printf("pilot_nt.dll not found\n");
        return -1;
    }

    try {
        dynamic_function card_authorize9Fn = (dynamic_function)GetProcAddress(pilotNt, "_card_authorize9");
        if (!card_authorize9Fn)
        {
            printf("card_authorize9 not found\n");
            return -2;
        }

        auth_answer9 argument;
        memset(&argument, 0, sizeof(argument));
        argument.ans.TType = OP_PURCHASE;
        argument.ans.Amount = amount;
        printf("Let's try to pay!\n");
        DWORD result = card_authorize9Fn(NULL, &argument);
        printf("Operation completed with code %d \n'", result);
        printf("Auth result is %s \n'", argument.ans.RCode);
        printf("Auth desc is is %s \n'", argument.ans.AMessage);
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
