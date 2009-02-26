#pragma once
// This class only contains static functions.
// It is a convenient way to let any WTL window class
// inherit non-blocking wait function calls.  I like
// to use inheritance to encapsulate the calls rather
// than declaring them as global.
// These functions borrow heavily from the article
// "Using a Non-Blocking Versions of Win32 Wait Functions"
// Andy Smith July 31, 2002
// http://www.codeguru.com/cpp/w-p/win32/article.php/c4531/
class CWaitFuncs
{
public:
static DWORD WaitForSingleObject(HANDLE hHandle,DWORD dwMilliseconds);
static DWORD WaitForSingleObjectEx(HANDLE hHandle,DWORD dwMilliseconds,BOOL bAlertable);
static DWORD WaitForMultipleObjects(DWORD nCount,LPHANDLE lpHandles,BOOL bWaitAll,DWORD dwMilliseconds);
static DWORD WaitForMultipleObjectsEx(DWORD nCount,LPHANDLE lpHandles,BOOL fWaitAll,DWORD dwMilliseconds,BOOL bAlertable);
static BOOL SleepEx(DWORD dwMilliseconds, BOOL bProcessMsgs=TRUE);

};