/**************************************************
 - FileName
    StrDbKernel.h
 - Description
    String database, the kernel module of manager
***************************************************/
#pragma once
#include <stddef.h>

// For debug
#define STORAGE_SIZE        30
// #define STORAGE_SIZE        1000

// The number of characters in shortest string is 2, including '\0'
#define MAX_STRING_COUNT    STORAGE_SIZE / 2

/*
    Storage index to locate a string in database
*/
typedef struct _StrIndex
{
    wchar_t *lpData;    // String pointer
    size_t nLength;     // Number of characters in string, including '\0'
} StrIndex;

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
static wchar_t *_GetString(size_t nIndex, size_t *lpLength);