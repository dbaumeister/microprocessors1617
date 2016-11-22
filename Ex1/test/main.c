#include <stdlib.h>

// use fastcall to pass ptr
__attribute__((fastcall)) int toupper_asm(char* pStr);

int main()
{
    int iSize = 8;
    char* pStr = (char*) malloc(iSize);

    for(int i = 0; i<iSize;++i)
    {
        pStr[i] = 0x41 + (i % 26);
    }
    pStr[iSize-1] = 0;
    
    toupper_asm(pStr);

    free(pStr);
    return 0;
}