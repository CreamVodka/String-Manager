/**************************************************
 - FileName
    StrDbKernel.h
 - Description
    String database, the kernel module of manager
***************************************************/
#pragma once
#include <stddef.h>
#include <stdbool.h>

// For debug
#define STORAGE_SIZE        50
// #define STORAGE_SIZE        1000

// The number of characters in shortest string is 2, including '\0'
#define MAX_STRING_COUNT    STORAGE_SIZE / 2

/*
    Storage index to locate a string in database
*/
typedef struct _Index
{
    wchar_t *lpData;    // String pointer
    size_t nLength;     // Number of characters in string, including '\0'
} Index;

/*
 - Description
    Get string by index
 - Input
    nIndex: The index of string
 - Output
    lpLength: Number of characters in string, including '\0'. It can be NULL
 - Return
    The string pointer, or NULL if index is out of range
*/
static wchar_t *_GetItem(size_t nIndex, size_t *lpLength);

/*
 - Description
    Lookup request free size in storage
 - Input
    nMinSize: Minmum request size
    AllowDefrag: Whether allow database to defrag storage when necessary
 - Output
    lpIndex: Space index in table
 - Return
    The free space pointer, or NULL
*/
static wchar_t *LookupFreeSpace(size_t nMinSize, bool AllowDefrag, size_t *lpIndex);

/*
 - Description
    Insert a new string index to table
 - Input
    nLocation: Location to insert
    lpString: Releated string
*/
static void InsertIndex(size_t nLocation, wchar_t *lpString);

/*
 - Description
    Delete a string index from table
 - Input
    nLocation: Location to Delete
*/
static void DeleteIndex(size_t nLocation);

/*
    Clear query records
*/
static void ClearQueryRecords();

/*
 - Description
    Query string by index
 - Input
    nIndex: The index of string
 - Output
    lpLength: Number of characters in string, including '\0'. It can be NULL
 - Return
    The string pointer, or NULL if index is out of range
*/
static wchar_t *_QueryStringByIndex(size_t nIndex, size_t *lpLength);

/*
 - Description
    Query next matched string by content
 - Input
    lpString: The string to query
    nBeginIndex: The index of beginning to search
 - Output
    lpMatchIndex: The matched string index. It can be NULL
 - Return
    The next matched string pointer, or NULL
*/
static wchar_t *_QueryNextStringByContent(
    const wchar_t *lpString, size_t nBeginIndex, size_t *lpMatchIndex);