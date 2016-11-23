#include <stdlib.h>

// use fastcall to pass ptr
__attribute__((fastcall)) int toupper_singlebyte__asm(char* pStr);
__attribute__((fastcall)) int toupper_4atonce_asm(char* pStr);

int main()
{
    int iSize = 8;
    char* pStr = (char*) malloc(iSize);

    for(int i = 0; i<iSize;++i)
    {

        pStr[i] = (i % 2 == 0 ? 0x61 : 0x41) + (i % 26);
    }
    pStr[iSize-1] = 0;
    
    toupper_singlebyte__asm(pStr);
    toupper_4atonce_asm(pStr);
    
    free(pStr);
    return 0;
}