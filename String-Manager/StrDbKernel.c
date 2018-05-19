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

#pragma warning(disable:4996) 
#pragma warning(disable:4018) 

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
    Lookup request free size in storage
*/
wchar_t *LookupFreeSpace(size_t nMinSize, bool AllowDefrag, size_t *lpIndex)
{
    assert(nMinSize <= STORAGE_SIZE);
    assert(lpIndex != NULL);

    if (nMinSize > GetFreeSize())
    {
        return NULL;
    }

    // The beginning space of storage is free
    if ((g_nUsedSize == 0) || 
        (g_StrIdxTab[0].lpData != g_szStorage &&
            g_StrIdxTab[0].lpData - g_szStorage >= nMinSize))
    {
        *lpIndex = 0;
        return g_szStorage;
    }
    else
    {
        // Check string gap space
        for (size_t i = 0; i != g_nStrCount - 1; ++i)
        {
            wchar_t *lpPreEnd = g_StrIdxTab[i].lpData + g_StrIdxTab[i].nLength;
            if (g_StrIdxTab[i + 1].lpData - lpPreEnd >= nMinSize)
            {
                *lpIndex = i + 1;
                return lpPreEnd;
            }
        }

        // Check last free space
        wchar_t *lpLastStrEnd = g_StrIdxTab[g_nStrCount - 1].lpData + g_StrIdxTab[g_nStrCount - 1].nLength;
        wchar_t *lpStorageEnd = &g_szStorage[STORAGE_SIZE];
        if (lpStorageEnd - lpLastStrEnd >= nMinSize)
        {
            *lpIndex = g_nStrCount;
            return lpLastStrEnd;
        }

        // Too many fragments, no enough continuous space
        if (AllowDefrag == true)
        {
            // Defrag and lookup again
            DefragDatabase();
            return LookupFreeSpace(nMinSize, false, lpIndex);
        }
        else
        {
            return NULL;
        }
    }
}

/*
    Store string to database
*/
bool StoreString(const wchar_t *lpString, size_t *lpIndex)
{
    assert(lpString != NULL);

    size_t nIndex = 0;
    size_t nLength = wcslen(lpString) + 1;
    wchar_t *lpBuffer = LookupFreeSpace(nLength, true, &nIndex);
    if (lpBuffer != NULL)
    {
        wcscpy(lpBuffer, lpString);
        InsertStringIndex(nIndex, lpBuffer);

        g_nUsedSize += nLength;
        ++g_nStrCount;
        return true;
    }
    else
    {
        return false;
    }
}

/*
    Insert a new string index to table
*/
void InsertStringIndex(size_t nLocation, wchar_t *lpString)
{
    assert(nLocation <= g_nStrCount);
    assert(lpString != NULL);

    size_t nRest = g_nStrCount - nLocation;
    memmove(&g_StrIdxTab[nLocation + 1], &g_StrIdxTab[nLocation], sizeof(StrIndex) * nRest);
    
    g_StrIdxTab[nLocation].lpData = lpString;
    g_StrIdxTab[nLocation].nLength = wcslen(lpString) + 1;
}

/*
    Delete a string index from table
*/
void DeleteStringIndex(size_t nLocation)
{
    assert(nLocation < g_nStrCount);

    size_t nRest = g_nStrCount - nLocation - 1;
    memmove(&g_StrIdxTab[nLocation], &g_StrIdxTab[nLocation + 1], sizeof(StrIndex) * nRest);
    
    // Set invalid index to NULL
    memset(&g_StrIdxTab[g_nStrCount - 1], 0, sizeof(StrIndex));
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
    Defrag database, put all strings together and clear fragments
*/
size_t DefragDatabase()
{
    wchar_t *lpDest = g_szStorage;
    for (size_t i = 0; i != g_nStrCount; ++i)
    {
        StrIndex *lpIndex = &g_StrIdxTab[i];
        memmove(lpDest, lpIndex->lpData, lpIndex->nLength);
        lpIndex->lpData = lpDest;
        lpDest += lpIndex->nLength;     // Store one next to one
    }

    return GetFreeSize();
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