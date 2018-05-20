#include <stdlib.h>
#include <stdio.h>
#include "StrDb.h"

int wmain(int argc, wchar_t *argv[])
{
    Store(L"ABC", NULL);
    Store(L"ABCDE", NULL);
    Store(L"12", NULL);
    Store(L"123", NULL);
    Store(L"12", NULL);
    Store(L"555", NULL);

    size_t nIndex = 0;
    bool bResult = DeleteByIndex(0);
    bResult = DeleteNextByContent(L"123", 0, &nIndex);
    bResult = DeleteNextByContent(L"ABCDE", 2, &nIndex);
    size_t nCount = DeleteAllByContent(L"12");

    return EXIT_SUCCESS;
}