// These functions borrow heavily from the article
// "Using a Non-Blocking Versions of Win32 Wait Functions"
// Andy Smith July 31, 2002
// http://www.codeguru.com/cpp/w-p/win32/article.php/c4531/
// Mr. Smith's work proved invaluable while developing the WinInetTest program
#include "stdafx.h"
#include "WaitFuncs.h"


DWORD CWaitFuncs::WaitForSingleObject(HANDLE hHandle,DWORD dwMilliseconds)
{
	return WaitForMultipleObjectsEx(1, &hHandle,FALSE,dwMilliseconds,FALSE);
}

DWORD CWaitFuncs::WaitForSingleObjectEx(HANDLE hHandle,DWORD dwMilliseconds,BOOL bAlertable)
{
	return WaitForMultipleObjectsEx(1, &hHandle,FALSE,dwMilliseconds,bAlertable);
}

DWORD CWaitFuncs::WaitForMultipleObjects(DWORD nCount,LPHANDLE lpHandles,BOOL bWaitAll,DWORD dwMilliseconds)
{
	return WaitForMultipleObjectsEx(nCount, lpHandles,bWaitAll,dwMilliseconds,FALSE);
}

DWORD CWaitFuncs::WaitForMultipleObjectsEx(DWORD nCount,LPHANDLE lpHandles,BOOL fWaitAll,DWORD dwMilliseconds,BOOL bAlertable)
{
	DWORD dwRetval(WAIT_FAILED);
	DWORD dwStartTime = GetTickCount();		// this is the start time of the function 
	DWORD dwRemaining = dwMilliseconds;		// this variable is the time left to wait before returning to caller
	DWORD dwFlags = (bAlertable ? MWMO_ALERTABLE : 0);

	for(DWORD i=0;i<nCount; )
	{	
		if(fWaitAll)
		{
			// wait for each HANDLE in the array
			// NB: note that using the flag MWO_WAITALL requires that all the objects in the array are 
			// signalled AS WELL AS there is a message in the input queue. Therefore if this thread is 
			// not a message processing thread we may never get a message and  
			// MsgWaitForMultipleObjectsEx() may never return.
			dwRetval=MsgWaitForMultipleObjectsEx(1,&lpHandles[i],dwRemaining,QS_ALLINPUT,dwFlags);
	
			// if object is signalled we must wait for the next object in the array
			if(dwRetval==WAIT_OBJECT_0){i++;}

			dwRetval=WAIT_OBJECT_0+nCount;
		}
		else
		{
			// wait for any of the objects in the array of HANDLEs
			dwRetval=MsgWaitForMultipleObjectsEx(nCount,lpHandles,dwRemaining,QS_ALLINPUT,dwFlags);
		}

		if(dwRetval==WAIT_OBJECT_0+nCount)
		{
			//messages in queue need to be processed
			MSG msg;
			while (PeekMessage(&msg, 0, 0, 0,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			// if we are not waiting indefinitely calculate the remaining time we need to wait
			if(INFINITE!=dwRemaining)
			{
				// calculate the time which has elapsed since entering the function
				DWORD dwElapsed = GetTickCount() - dwStartTime;

				if(dwMilliseconds>dwElapsed)
				{
					// calculate any remaining time we need to wait 
					dwRemaining = dwMilliseconds - dwElapsed;
				}
				else
				{
					// no need to wait any longer
					dwRetval = WAIT_TIMEOUT; 

					// escape the loop
					i = nCount;			
				}
			}
		}
		else
		{
			// something else happened - escape the loop
			i = nCount;
		}
	}

	return dwRetval;
}

BOOL CWaitFuncs::SleepEx(DWORD dwMilliseconds, BOOL bProcessMsgs)
{
	DWORD dwRetval(WAIT_TIMEOUT);
	if(bProcessMsgs)
	{
		// We want to do a non blocking sleep for dwMilliseconds.
		// So we create a dummy event object which we will never signal
		// This is because MsgWaitForMultipleObjectsEx requires a non NULL 
		// Win32 object to wait upon
		HANDLE hWaitObject = CreateEvent(NULL,TRUE,FALSE,NULL);
		
		dwRetval = WaitForSingleObject(hWaitObject,dwMilliseconds);

		//destroy the event object
		CloseHandle(hWaitObject);
	}
	else
	{
		// do a normal sleep call
		::SleepEx(dwMilliseconds,FALSE);
	}

	return dwRetval==WAIT_TIMEOUT?TRUE:FALSE;
}
