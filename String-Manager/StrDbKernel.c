/**************************************************
 - FileName
    StrDbKernel.c
 - Description
    String database, the kernel module of manager
***************************************************/
#include "StrDbKernel.h"
#include "StrDb.h"
#include <string.h>
#include <assert.h>

// String storage
static wchar_t      g_szStorage[STORAGE_SIZE] = { L'\0' };
static size_t       g_nUsedSize = 0;

// String index table, to locate all strings in storage
static StrIndex     g_StrIdxTab[MAX_STRING_COUNT] = { 0 };
static size_t       g_nStrCount = 0;

/*
    Get string by index
*/
wchar_t *_GetString(size_t nIndex, size_t *lpLength)
{
    if (nIndex < g_nStrCount)
    {
        if (lpLength != NULL)
        {
            *lpLength = g_StrIdxTab[nIndex].nLength;
        }

        return g_StrIdxTab[nIndex].lpData;
    }
    else
    {
        return NULL;
    }
}

const wchar_t *GetString(size_t nIndex, size_t *lpLength)
{
    return _GetString(nIndex, lpLength);
}

/*
    Clear database
*/
void ClearDatabase()
{
    memset(g_szStorage, '\0', sizeof(g_szStorage));
    memset(g_StrIdxTab, 0, sizeof(g_StrIdxTab));
    g_nUsedSize = 0;
    g_nStrCount = 0;
}

/*
    Get the total size of storage
*/
size_t GetTotalSize()
{
    return STORAGE_SIZE;
}

/*
    Get the used size of storage
*/
size_t GetUsedSize()
{
    return g_nUsedSize;
}

/*
    Get the free size of storage
*/
size_t GetFreeSize()
{
    return STORAGE_SIZE - g_nUsedSize;
}

/*
    Get string count in database
*/
size_t GetStringCount()
{
    return g_nStrCount;
}