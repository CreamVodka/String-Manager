/******************************************************
 - FileName
    StrDb.h
 - Description
    The user interface declaraction of string database
*******************************************************/
#pragma once
#include <stddef.h>
#include <stdbool.h>

/*
    Clear database
*/
void ClearDatabase();

/*
 - Description
    Defrag database, put all strings together and clear fragments
 - Return
    The free size of storage
*/
size_t DefragDatabase();

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
size_t GetItemCount();

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
const wchar_t *GetItem(size_t nIndex, size_t *lpLength);

/*
 - Description
    Store string to database
 - Input
    lpString: The string to store
 - Output
    lpIndex: String index in database, It can be NULL
 - Return
    true if successful, or false
*/
bool Store(const wchar_t *lpString, size_t *lpIndex);