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
    Record string query result
*/
typedef struct _QueryRecord
{
    const wchar_t *lpData;  // String pointer
    size_t nIndex;          // String Index in database
} QueryRecord;

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
const wchar_t *QueryByIndex(size_t nIndex, size_t *lpLength);

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
const wchar_t *QueryNextByContent(
    const wchar_t *lpString, size_t nBeginIndex, size_t *lpMatchIndex);

/*
 - Description
    Query all strings by content
 - Input
    lpString: The string to query
 - Output
    lpMatchCount: The matched strings count
 - Return
    The all matched records
*/
const QueryRecord *QueryAllByContent(const wchar_t *lpString, size_t *lpMatchCount);

/*
 - Description
    Fuzzy query all strings by content
 - Input
    lpString: The string to query
 - Output
    lpMatchCount: The matched strings count
 - Return
    The all matched records
*/
const QueryRecord *FuzzyQueryAllByContent(const wchar_t *lpString, size_t *lpMatchCount);

/*
 - Description
    Delete string by index
 - Input
    nIndex: The index of string
 - Return
    true if successful, or false
*/
bool DeleteByIndex(size_t nIndex);

/*
 - Description
    Delete next matched string by content
 - Input
    lpString: The string to delete
    nBeginIndex: The index of beginning to search
 - Output
    lpDeleteIndex: The deleted string index. It can be NULL
 - Return
    true if successful, or false
*/
bool DeleteNextByContent(const wchar_t *lpString, 
    size_t nBeginIndex, size_t *lpDeleteIndex);

/*
 - Description
    Delete all matched string by content
 - Input
    lpString: The string to delete
 - Return
    The deleted strings count
*/
size_t DeleteAllByContent(const wchar_t *lpString);

/*
 - Description
    Alter string by index
 - Input
    nIndex: The index of source string
    lpNewString: The new string
 - Output
    lpNewIndex: The new string index. It can be NULL
 - Return
    true if successful, or false
*/
bool AlterByIndex(size_t nIndex, const wchar_t *lpNewString, size_t *lpNewIndex);

/*
 - Description
    Alter next matched string by content
 - Input
    lpSrcString: The source string
    nBeginIndex: The index of beginning to search
    lpNewString: The new string
 - Output
    lpSrcIndex: The altered string index. It can be NULL
    lpNewIndex: The new string index. It can be NULL
 - Return
    true if successful, or false
*/
bool AlterNextByContent(const wchar_t *lpSrcString, size_t nBeginIndex, 
    const wchar_t *lpNewString, size_t *lpSrcIndex, size_t *lpNewIndex);

/*
 - Description
    Alter all matched string by content
 - Input
    lpSrcString: The source string
    lpNewString: The new string
 - Return
    The altered strings count
*/
size_t AlterAllByContent(const wchar_t *lpSrcString, const wchar_t *lpNewString);