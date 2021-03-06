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

// Min array size to store '0'~'9', 'a'~'z' and 'A'~'Z' counts
#define MIN_STAT_SIZE   62

// String storage
static wchar_t      g_szStorage[STORAGE_SIZE] = { L'\0' };
static size_t       g_nUsedSize = 0;

// String index table, to locate all strings in storage
static Index        g_IdxTab[MAX_STRING_COUNT] = { 0 };
static size_t       g_nCount = 0;

// Record string query results
static QueryRecord  g_QueryRecords[MAX_STRING_COUNT] = { 0 };

/*
    Get string by index
*/
wchar_t *_GetItem(size_t nIndex, size_t *lpLength)
{
    if (nIndex < g_nCount)
    {
        if (lpLength != NULL)
        {
            *lpLength = g_IdxTab[nIndex].nLength;
        }

        return g_IdxTab[nIndex].lpData;
    }
    else
    {
        return NULL;
    }
}

const wchar_t *GetItem(size_t nIndex, size_t *lpLength)
{
    return _GetItem(nIndex, lpLength);
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
        (g_IdxTab[0].lpData != g_szStorage &&
            g_IdxTab[0].lpData - g_szStorage >= nMinSize))
    {
        *lpIndex = 0;
        return g_szStorage;
    }
    else
    {
        // Check string gap space
        for (size_t i = 0; i != g_nCount - 1; ++i)
        {
            wchar_t *lpPreEnd = g_IdxTab[i].lpData + g_IdxTab[i].nLength;
            if (g_IdxTab[i + 1].lpData - lpPreEnd >= nMinSize)
            {
                *lpIndex = i + 1;
                return lpPreEnd;
            }
        }

        // Check last free space
        wchar_t *lpLastEnd = g_IdxTab[g_nCount - 1].lpData + g_IdxTab[g_nCount - 1].nLength;
        wchar_t *lpStorageEnd = &g_szStorage[STORAGE_SIZE];
        if (lpStorageEnd - lpLastEnd >= nMinSize)
        {
            *lpIndex = g_nCount;
            return lpLastEnd;
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
bool Store(const wchar_t *lpString, size_t *lpIndex)
{
    assert(lpString != NULL);

    size_t nIndex = 0;
    size_t nLength = wcslen(lpString) + 1;
    wchar_t *lpBuffer = LookupFreeSpace(nLength, true, &nIndex);
    if (lpBuffer != NULL)
    {
        memset(lpBuffer, '\0', nLength * sizeof(wchar_t));
        wcscpy(lpBuffer, lpString);
        InsertIndex(nIndex, lpBuffer);
        if (lpIndex != NULL)
        {
            *lpIndex = nIndex;
        }

        g_nUsedSize += nLength;
        ++g_nCount;
        return true;
    }
    else
    {
        return false;
    }
}

/*
    Clear query records
*/
void ClearQueryRecords()
{
    memset(g_QueryRecords, 0, sizeof(g_QueryRecords));
}

/*
    Query string by index
*/
wchar_t *_QueryByIndex(size_t nIndex, size_t *lpLength)
{
    return _GetItem(nIndex, lpLength);
}

const wchar_t *QueryByIndex(size_t nIndex, size_t *lpLength)
{
    return _QueryByIndex(nIndex, lpLength);
}

/*
    Query next matched string by content
*/
wchar_t *_QueryNextByContent(const wchar_t *lpString,
    size_t nBeginIndex, size_t *lpMatchIndex)
{
    assert(lpString != NULL);
    assert(nBeginIndex <= g_nCount);

    size_t nLength = wcslen(lpString) + 1;
    for (size_t i = nBeginIndex; i < g_nCount; ++i)
    {
        if (g_IdxTab[i].nLength == nLength
            && wcscmp(lpString, g_IdxTab[i].lpData) == 0)
        {
            if (lpMatchIndex != NULL)
            {
                *lpMatchIndex = i;
            }

            return g_IdxTab[i].lpData;
        }
    }

    return NULL;
}

const wchar_t *QueryNextByContent(
    const wchar_t *lpString, size_t nBeginIndex, size_t *lpMatchIndex)
{
    return _QueryNextByContent(lpString, nBeginIndex, lpMatchIndex);
}

/*
    Query all strings by content
*/
const QueryRecord *QueryAllByContent(const wchar_t *lpString, size_t *lpMatchCount)
{
    assert(lpString != NULL);
    assert(lpMatchCount != NULL);

    ClearQueryRecords();

    size_t nMatchCount = 0, nMatchIndex = 0;
    wchar_t *lpResult = _QueryNextByContent(lpString, nMatchIndex, &nMatchIndex);
    while (lpResult != NULL)
    {
        g_QueryRecords[nMatchCount].lpData = lpResult;
        g_QueryRecords[nMatchCount].nIndex = nMatchIndex;
        ++nMatchCount;

        lpResult = _QueryNextByContent(lpString, nMatchIndex + 1, &nMatchIndex);
    }

    *lpMatchCount = nMatchCount;
    return g_QueryRecords;
}

/*
    Fuzzy query all strings by content
*/
const QueryRecord *FuzzyQueryAllByContent(const wchar_t *lpString, size_t *lpMatchCount)
{
    assert(lpString != NULL);
    assert(lpMatchCount != NULL);

    ClearQueryRecords();

    size_t nMatchCount = 0;
    for (size_t i = 0; i != g_nCount; ++i)
    {
        if (wcsstr(g_IdxTab[i].lpData, lpString) != NULL)
        {
            g_QueryRecords[nMatchCount].lpData = g_IdxTab[i].lpData;
            g_QueryRecords[nMatchCount].nIndex = i;
            ++nMatchCount;
        }
    }

    *lpMatchCount = nMatchCount;
    return g_QueryRecords;
}

/*
    Delete string by index
*/
bool DeleteByIndex(size_t nIndex)
{
    size_t nLength = 0;
    wchar_t *lpString = _GetItem(nIndex, &nLength);
    if (lpString != NULL)
    {
        memset(lpString, '\0', nLength * sizeof(wchar_t));
        DeleteIndex(nIndex);
        --g_nCount;
        g_nUsedSize -= nLength;
        return true;
    }
    else
    {
        return false;
    }
}

/*
    Delete next matched string by content
*/
bool DeleteNextByContent(const wchar_t *lpString, 
    size_t nBeginIndex, size_t *lpDeleteIndex)
{
    assert(lpString != NULL);
    assert(nBeginIndex <= g_nCount);

    size_t nDeleteIndex = 0;
    if (QueryNextByContent(lpString, nBeginIndex, &nDeleteIndex) != NULL)
    {
        DeleteByIndex(nDeleteIndex);
        if (lpDeleteIndex != NULL)
        {
            *lpDeleteIndex = nDeleteIndex;
        }

        return true;
    }
    else
    {
        return false;
    }
}

/*
    Delete all matched string by content
*/
size_t DeleteAllByContent(const wchar_t *lpString)
{
    assert(lpString != NULL);

    size_t nDeleteCount = 0, nDeleteIndex = 0;
    bool bResult = DeleteNextByContent(lpString, nDeleteIndex, &nDeleteIndex);
    while (bResult != false)
    {
        ++nDeleteCount;
        bResult = DeleteNextByContent(lpString, nDeleteIndex, &nDeleteIndex);
    }

    return nDeleteCount;
}

/*
    Alter string by index
*/
bool AlterByIndex(size_t nIndex, const wchar_t *lpNewString, size_t *lpNewIndex)
{
    assert(lpNewString != NULL);

    size_t nSrcLength = 0;
    wchar_t *lpSrcString = _GetItem(nIndex, &nSrcLength);
    if (lpSrcString != NULL)
    {
        size_t nNewLength = wcslen(lpNewString) + 1;
        if (nNewLength <= nSrcLength)
        {
            // Alter on the same place
            memset(lpSrcString, '\0', nSrcLength * sizeof(wchar_t));
            wcscpy(lpSrcString, lpNewString);
            g_IdxTab[nIndex].nLength = nNewLength;
            g_nUsedSize -= (nSrcLength - nNewLength);
            if (lpNewIndex != NULL)
            {
                *lpNewIndex = nIndex;
            }

            return true;
        }
        else if (nNewLength - nSrcLength <= GetFreeSize())
        {
            // Delete source and find a new place to store
            DeleteByIndex(nIndex);
            return Store(lpNewString, lpNewIndex);
        }
    }

    return false;
}

/*
    Alter next matched string by content
*/
bool AlterNextByContent(const wchar_t *lpSrcString, size_t nBeginIndex,
    const wchar_t *lpNewString, size_t *lpSrcIndex, size_t *lpNewIndex)
{
    assert(lpSrcString != NULL);
    assert(lpNewString != NULL);
    assert(nBeginIndex <= g_nCount);

    size_t nSrcIndex = 0;
    if (QueryNextByContent(lpSrcString, nBeginIndex, &nSrcIndex) != NULL)
    {
        if (lpSrcIndex != NULL)
        {
            *lpSrcIndex = nSrcIndex;
        }

        return AlterByIndex(nSrcIndex, lpNewString, lpNewIndex);;
    }
    else
    {
        return false;
    }
}

/*
    Alter all matched string by content
*/
size_t AlterAllByContent(const wchar_t *lpSrcString, const wchar_t *lpNewString)
{
    assert(lpSrcString != NULL);
    assert(lpNewString != NULL);

    size_t nAlterCount = 0, nAlterIndex = 0;
    if (wcscmp(lpSrcString, lpNewString) != 0)
    {
        bool bResult = AlterNextByContent(lpSrcString,
            nAlterIndex, lpNewString, &nAlterIndex, NULL);
        while (bResult != false)
        {
            ++nAlterCount;
            bResult = AlterNextByContent(lpSrcString,
                nAlterIndex, lpNewString, &nAlterIndex, NULL);
        }
    }

    return nAlterCount;
}

/*
    Insert a new string index to table
*/
void InsertIndex(size_t nLocation, wchar_t *lpString)
{
    assert(nLocation <= g_nCount);
    assert(lpString != NULL);

    size_t nRest = g_nCount - nLocation;
    memmove(&g_IdxTab[nLocation + 1], &g_IdxTab[nLocation], sizeof(Index) * nRest);
    
    g_IdxTab[nLocation].lpData = lpString;
    g_IdxTab[nLocation].nLength = wcslen(lpString) + 1;
}

/*
    Delete a string index from table
*/
void DeleteIndex(size_t nLocation)
{
    assert(nLocation < g_nCount);

    size_t nRest = g_nCount - nLocation - 1;
    memmove(&g_IdxTab[nLocation], &g_IdxTab[nLocation + 1], sizeof(Index) * nRest);
    
    // Set invalid index to NULL
    memset(&g_IdxTab[g_nCount - 1], 0, sizeof(Index));
}

/*
    Clear database
*/
void ClearDatabase()
{
    memset(g_szStorage, '\0', sizeof(g_szStorage));
    memset(g_IdxTab, 0, sizeof(g_IdxTab));
    g_nUsedSize = 0;
    g_nCount = 0;
}

/*
    Defrag database, put all strings together and clear fragments
*/
size_t DefragDatabase()
{
    wchar_t *lpDest = g_szStorage;
    for (size_t i = 0; i != g_nCount; ++i)
    {
        Index *lpIndex = &g_IdxTab[i];
        MoveString(lpDest, lpIndex->lpData);
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
size_t GetItemCount()
{
    return g_nCount;
}

/*
    Get storage pointer
*/
const wchar_t GetStorage()
{
    return g_szStorage;
}

/*
    Count the number and frequency of '0'~'9', 'a'~'z' and 'A'~'Z'
*/
bool Statistic(size_t *lpCounts, size_t nSize, size_t *lpTotal)
{
    assert(lpCounts != NULL);

    /*
        Char: '0' ~ '9'
        ASCII: 0x30 ~ 0x39
        Index = ASCII - 0x30

        Char: 'A' ~ 'Z'
        ASCII: 0x41 ~ 0x5A
        Index = ASCII - 0x37

        Char: 'a' ~ 'z'
        ASCII: 0x61 ~ 0x7A
        Index = ASCII - 0x3D
    */

    if (nSize >= MIN_STAT_SIZE)
    {
        size_t nTotal = 0;
        for (size_t i = 0; i != g_nCount; ++i)
        {
            wchar_t *lpString = g_IdxTab[i].lpData;
            for (size_t j = 0; j != g_IdxTab[i].nLength - 1; ++j)
            {
                ++nTotal;

                wchar_t cha = lpString[j];
                if (L'0' <= cha && cha <= L'9')
                {
                    ++lpCounts[cha - 0x30];
                }
                else if (L'A' <= cha && cha <= L'Z')
                {
                    ++lpCounts[cha - 0x37];
                }
                else if (L'a' <= cha && cha <= L'z')
                {
                    ++lpCounts[cha - 0x3D];
                }
            }
        }

        if (lpTotal != NULL)
        {
            *lpTotal = nTotal;
        }

        return true;
    }
    else
    {
        return false;
    }
}

/*
    Move string from source to dest, and set invalid data to '\0'
*/
void MoveString(wchar_t *lpDest, wchar_t *lpSrc)
{
    assert(lpDest != NULL);
    assert(lpDest != NULL);

    size_t nLength = wcslen(lpSrc) + 1;
    if (lpSrc < lpDest && lpDest < lpSrc + nLength)
    {
        wchar_t *lpSrcData = lpSrc + nLength - 1;
        wchar_t *lpDestData = lpDest + nLength - 1;
        for (size_t i = 0; i != nLength; ++i, --lpSrcData, --lpDestData)
        {
            *lpDestData = *lpSrcData;
            *lpSrcData = '\0';
        }
    }
    else
    {
        wchar_t *lpSrcData = lpSrc;
        wchar_t *lpDestData = lpDest;
        for (size_t i = 0; i != nLength; ++i, ++lpSrcData, ++lpDestData)
        {
            *lpDestData = *lpSrcData;
            *lpSrcData = '\0';
        }
    }
}