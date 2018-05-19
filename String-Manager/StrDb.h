/******************************************************
 - FileName
    StrDb.h
 - Description
    The user interface declaraction of string database
*******************************************************/
#pragma once
#include <stddef.h>

/*
    Clear database
*/
void ClearDatabase();

/*
    Get the total size of storage
*/
size_t GetTotalSize();

/*
    Get the used size of storage
*/
size_t GetUsedSize();

/*
    Get the free size of storage
*/
size_t GetFreeSize();

/*
    Get string count in database
*/
size_t GetStringCount();

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
const wchar_t *GetString(size_t nIndex, size_t *lpLength);