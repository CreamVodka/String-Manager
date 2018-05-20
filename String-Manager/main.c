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

    size_t nCount = 0;
    const QueryRecord *lpRecords = FuzzyQueryAllStringsByContent(L"2", &nCount);
    for (size_t i = 0; i != nCount; ++i)
    {
        wprintf(L"Index: %u\r\nString: %s\r\n",
            lpRecords[i].nIndex,
            lpRecords[i].lpData);
    }

    lpRecords = FuzzyQueryAllStringsByContent(L"5", &nCount);
    lpRecords = FuzzyQueryAllStringsByContent(L"ABC", &nCount);
    lpRecords = FuzzyQueryAllStringsByContent(L"12345", &nCount);

    lpRecords = QueryAllStringsByContent(L"12", &nCount);
    for (size_t i = 0; i != nCount; ++i)
    {
        wprintf(L"Index: %u\r\nString: %s\r\n",
            lpRecords[i].nIndex,
            lpRecords[i].lpData);
    }

    size_t nLength = 0;
    const wchar_t *lpResult = QueryStringByIndex(2, &nLength);

    size_t nIndex = 0;
    lpResult = QueryNextStringByContent(L"12", 0, &nIndex);
    lpResult = QueryNextStringByContent(L"12", 3, &nIndex);
    lpResult = QueryNextStringByContent(L"12", 5, &nIndex);

    return EXIT_SUCCESS;
}