#include "..\WinMain.h"		// standard application include
#include "Misc.h"			// include for this file

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// MISC UTILITY ROUTINES //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// local prototypes
static bool GetRegValue(const HKEY hReg, const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType, void *pDest, DWORD dwSize);
static bool SetRegValue(const HKEY hReg, const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType, void *pDest, DWORD dwSize);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		GetCmdLineValue (const LPTSTR szArg, LPTSTR szDest, size_t nLen)
/ /
/ /		szArg = the name of the switch/argument to search for
/ /		szDest = pointer to a valid buffer to store the result in
/ /		nLen = length of the destination buffer
/ /
/ / NOTES:
/ /		Returns a boolean value to indicate whether or not the argument
/ /		was found, not to be mistaken with the value of the argument.
/ /
/ / PURPOSE:
/ /		Searches the command line for a particular argument/value pair in a
/ /		Unicode friendly fashion and 9x support unlike CommandLineToArgvW.
/*/

bool
GetCmdLineValue (const LPTSTR szArg, LPTSTR szDest, size_t nLen)
{
	LPTSTR szCmd=NULL;		// pointer to the command line
	LPTSTR szTest=NULL;		// worker variable
	bool bRetVal = false;	// returen value

	szCmd = GetCommandLine();	// unicode safe

	// validate our data before continuing
	if(((szArg != NULL) && (_tcslen(szArg) > 0)) &&
		((szCmd != NULL) && (_tcslen(szCmd) > 0)))
	{
		// look for the / or - delimiters and move back on char in the buffer
		// this is because we must test for a space before the arg so we can
		// safely assume it's a real arg and not contained in another string
		szTest = _tcspbrk(szCmd, _T("/-"));

		// validate and also make sure this isn't the last char in the string
		if(szTest != NULL && _tcslen(szTest) != 0)
		{
			// unicode safe, if space, then we have an arg, so test it
			szTest = _tcsdec(szCmd, szTest);
			if(NMATCH(szTest, _T(" "), 1))
			{
				// pass up the space and delimeter to test the arg name
				szTest = _tcsinc(szTest);
				szTest = _tcsinc(szTest);

				if(NMATCH(szArg, szTest, _tcslen(szArg)))
				{
					// ok, we have a match, that's enough for the function's
					//  return value but we need to also check to see to see
					// if the arg has an associated value with it

					// check to see if there is an = after the arg name
					szTest = _tcsninc(szTest, _tcslen(szArg));
					if(szTest != NULL && NMATCH(szTest, _T("="), 1))
					{
						// we have one, see what data (until next space) is there
						szTest = _tcsinc(szTest);
						if(szTest != NULL)
						{
							LPTSTR szTemp = NULL;

							szTemp = (LPTSTR)_tcschr(szTest, _T(' '));
							if(szTemp != NULL)
							{
								// take everything the buffer will hold, up until the space
								if(nLen > (size_t)(szTemp - szTest))
									_tcsncpy_s(szDest, nLen, szTest, szTemp - szTest);
								else
									_tcsncpy_s(szDest, nLen, szTest, nLen);
							}
							else
							{
								// take everything the buffer will hold
								_tcsncpy_s(szDest, nLen, szTest, nLen);
							}
						}
					}

					bRetVal = true;
				}
			}
		}
	}

	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		GetRegValue (const HKEY hReg, const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType,
/ /			void *pDest, DWORD dwSize)
/ /
/ / PURPOSE:
/ /		Local function intended to serve as a means to repeat the repeatative operations
/ /		needed to access registry value data frequently. It should not be called directly.
/*/

static bool
GetRegValue (const HKEY hReg, const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType,
			void *pDest, DWORD dwSize)
{
	HKEY hKey = NULL;
	bool bReturn = false;
	DWORD dwRealType = 0;

	// open the key in the registry
	if((RegOpenKeyEx(hReg, szSubkey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
		&& (hKey != NULL))
	{
		if(pDest != NULL)
		{
			// do not forget, the buffer size for string data must also include a terminating zero
			if(RegQueryValueEx(hKey, szValue, NULL, &dwRealType, (LPBYTE)pDest, &dwSize) == ERROR_SUCCESS)
			{
				// here we add our own bit of type safety (b/c the API really doesn't), if the returned type
				// doesn't match the type passed, we wipe the buffer and return false to avoid a casting err
				if(dwRealType != dwType)
					SecureZeroMemory(pDest, dwSize);
				else
					bReturn = true;
			}
		}

		// clean up
		RegCloseKey(hKey);
	}

	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		GetSysValue (const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType, void *pDest, DWORD dwSize)
/ /
/ /		szSubkey = name of the subkey to look for the values in
/ /		szValue = name of the value to read data from (use NULL for default value)
/ /		dwType = type of data to pull: REG_DWORD, REG_SZ, etc.
/ /		pDest = destination buffer to return the data in, can be of any type
/ /		dwSize = size of the destination buffer (buffer size always, not length of chars for string data)
/ /
/ / PURPOSE:
/ /		Function intended to serve as a means to repeat the repeatative operations
/ /		needed to access registry value data frequently from the local machine hive.
/*/

bool
GetSysValue (const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType, void *pDest, DWORD dwSize)
{
	// since this is a wrapper function, call the real one to get the data
	return GetRegValue(HKEY_LOCAL_MACHINE, szSubkey, szValue, dwType, pDest, dwSize);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		GetUserValue (const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType, void *pDest, DWORD dwSize)
/ /
/ /		szSubkey = name of the subkey to look for the values in
/ /		szValue = name of the value to read data from (use NULL for default value)
/ /		dwType = type of data to pull: REG_DWORD, REG_SZ, etc.
/ /		pDest = destination buffer to return the data in, can be of any type
/ /		dwSize = size of the destination buffer (buffer size always, not length of chars for string data)
/ /
/ / PURPOSE:
/ /		Function intended to serve as a means to repeat the repeatative operations
/ /		needed to access registry value data frequently from the local user hive.
/ /
/ / NOTE:
/ /		This will prepend company and software name before the subkey that's pass to automatically
/ /		keep the data orginaized in the registry and so the caller needn't worry about it.
/*/

bool
GetUserValue (const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType, void *pDest, DWORD dwSize)
{
	TCHAR szPath[MAX_LOADSTRING] = {0};

	// we need to create the string to prepend to the passed subkey (saves time for the caller)
	// to do this, we'll concat the passed value after the company name and app name constants
	_tcscpy_s(szPath, SIZE(szPath), _T("Software\\"));
	_tcscat_s(szPath, SIZE(szPath), COMPANY_NAME);
	_tcscat_s(szPath, SIZE(szPath), _T("\\"));
	_tcscat_s(szPath, SIZE(szPath), APP_NAME);

	// this can be left null if company/software name is sufficient
	if(szSubkey != NULL)
	{
		_tcscat_s(szPath, SIZE(szPath), _T("\\"));
		_tcscat_s(szPath, SIZE(szPath), szSubkey);
	}

	// since this is a wrapper function, call the real one to get the data
	return GetRegValue(HKEY_CURRENT_USER, szPath, szValue, dwType, pDest, dwSize);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		MoveWnd (HWND hwnd, unsigned short nOrientation, HWND hwndPreferred)
/ /
/ / NOTES:
/ /		Use a bitwise OR with the MV_BOTTOM, MV_CENTER, MV_LEFT,
/ /		MV_MIDDLE, MV_RIGHT, and MV_TOP constants to position.
/ /
/ / PURPOSE:
/ /		Function to move a window in one of nine possible combinations,
/ /		either relative to a preferred window or the work area.
/ /
/ /	CREDITS:
/ /		This function originated as the CenterWindow function found in the book
/ /		"Win32 Programming" by Brent E. Rector and Joseph M. Newcomer.  I,
/ /		Jeremy Falcon, modified it to allow for movement control, and renamed
/ /		it similar to MoveWindow (Win32 API) to reflect the new functionality.
/*/

bool
MoveWnd (HWND hwnd, unsigned short nOrientation, HWND hwndPreferred)
{
	POINT	PreferredPoint = {0};
	RECT	WndRect = {0}, PreferredRect = {0};
	SIZE	WndSize = {0};

	// don't bother unless we have a valid window to center
	if(!IsWindow(hwnd))	return false;

	/*/
	/ / Move the window around the preferred window if it exists;
	/ / otherwise, move the window relevant to the work area.
	/*/
    if(hwndPreferred != NULL)
	{
        if(!IsWindow(hwndPreferred)) return false;
        GetWindowRect(hwndPreferred, &PreferredRect);
    }
    else
	{
		// attempt to get the dimensions for the work area
		if(!SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &PreferredRect, 0))
		{
			/*/
			/ / If the above call failed Explorer is not running correctly;
			/ / therefore, there is no work area (and taskbar). So, we'll
			/ / use the screen size instead for our coordinates.
			/*/
			SetRect(&PreferredRect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		}
	}

	// get the rect of the window (to be moved) and compute the width/height
	GetWindowRect(hwnd, &WndRect);
	WndSize.cx = (WndRect.right  - WndRect.left);
	WndSize.cy = (WndRect.bottom - WndRect.top);

	/*/
	/ / NOTE: We do not need to check for left and top because the point
	/ / already defaults to 0,0. Check for the rest and change accordingly.
	/*/

    // calc the left and top positions of the window to be moved
	if((nOrientation & MV_BOTTOM) == MV_BOTTOM)
		PreferredPoint.y = PreferredRect.bottom - WndSize.cy;

	if((nOrientation & MV_CENTER) == MV_CENTER)
		PreferredPoint.x = (PreferredRect.right - WndSize.cx) / 2;

	if((nOrientation & MV_MIDDLE) == MV_MIDDLE)
		PreferredPoint.y = (PreferredRect.bottom - WndSize.cy) / 2;

	if((nOrientation & MV_RIGHT) == MV_RIGHT)
		PreferredPoint.x = PreferredRect.right - WndSize.cx;

    // reposition the window
	return SetWindowPos(hwnd, NULL, PreferredPoint.x, PreferredPoint.y,
						0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / int
/ /		MsgBox(HWND hWnd, unsigned int uText, unsigned int uType)
/ /
/ / PURPOSE:
/ /		Simplify the usage of the MessageBox() function with using resouce identifiers.
/ /
/ / NOTES:
/ /		This assumes that the resouce file belongs to the module (exe) that started the
/ /		current process.
/*/

__inline int
MsgBox(HWND hWnd, unsigned int uText, unsigned int uType)
{
	TCHAR szText[MAX_LOADSTRING]={0};

	// load the resources into our buffers and call MessageBox()
	// the caption will always be the name of the application
	if((LoadString(GetModuleHandle(NULL), uText, szText, SIZE(szText)) > 0))
	{
		// if hWnd is NULL, make sure the type MB_TASKMODAL is specified
		if((hWnd == NULL) && ((uType & MB_TASKMODAL) != MB_TASKMODAL)) uType |= MB_TASKMODAL;
		return MessageBox(hWnd, szText, APP_NAME, uType);
	}
	else
		return 0; // indicates an error, same as MessageBox()
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		OnGLError (LPCSTR szProcedure)
/ /
/ /		szProcedure = intended to serve as the procedure name the error occured in
/ /
/ / PURPOSE:
/ /		This routine handles OGL errors in whichever way deemed fit
/ /		by the application, whether it be messaging, logging, etc.
/ /
/ / NOTES:
/ /		This must be called in the context of the thread it belongs to.
/*/

#ifdef _DEBUG
bool
OnGLError (LPCSTR szProcedure)
{
	GLenum glErr = GL_NO_ERROR;
	bool bReturn = false;
	TCHAR szOutput[MAX_LOADSTRING] = {0}, szError[MAX_LOADSTRING] = {0};

	// to avoid 5 million message boxes, do not use a loop for this
	if((glErr = glGetError()) != GL_NO_ERROR)
	{
		// note: we don't need to use resource strings for this
		// because this is for debugging only, thus it only needs
		// to be in one language - the one being developed in
		switch(glErr)
		{
			case GL_INVALID_ENUM:
				_tcscpy_s(szError, SIZE(szError), _T("Invalid enumeration value!"));
				break;

			case GL_INVALID_VALUE:
				_tcscpy_s(szError, SIZE(szError), _T("Invalid value!"));
				break;

			case GL_INVALID_OPERATION:
				_tcscpy_s(szError, SIZE(szError), _T("Invalid operation!"));
				break;

			case GL_STACK_OVERFLOW:
				_tcscpy_s(szError, SIZE(szError), _T("Stack Overflow!"));
				break;

			case GL_STACK_UNDERFLOW:
				_tcscpy_s(szError, SIZE(szError), _T("Stack Underflow!"));
				break;

			case GL_OUT_OF_MEMORY:
				_tcscpy_s(szError, SIZE(szError), _T("Out of memory!"));
				break;
		}

		// display a message to the user
		_stprintf_s(szOutput, SIZE(szOutput),
			_T("The OpenGL subsystem has generated the following error.\n\nError Info:\t%s\nProcedure:\t%s"), szError, szProcedure);

		MessageBox(NULL, szOutput, _T("Debug Mode Ouput"), MB_OK|MB_ICONERROR|MB_TASKMODAL);

		// let the caller know we had an error
		bReturn = true;
	}

	return bReturn;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		SetRegValue (const HKEY hReg, const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType,
/ /			void *pDest, DWORD dwSize)
/ /
/ / PURPOSE:
/ /		Local function intended to serve as a means to repeat the repeatative operations
/ /		needed to access registry value data frequently. It should not be called directly.
/*/

static bool
SetRegValue (const HKEY hReg, const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType,
			void *pDest, DWORD dwSize)
{
	HKEY hKey = NULL;
	bool bReturn = false;

	// open (or creat if it doesn't exist) the key in the registry
	if((RegCreateKeyEx(hReg, szSubkey, 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) && (hKey != NULL))
	{
		if(pDest != NULL)
		{
			// do not forget, the buffer size for string data must also include a terminating zero
			if(RegSetValueEx(hKey, szValue, 0, dwType, pDest, dwSize) == ERROR_SUCCESS)
				bReturn = true;
		}

		// clean up
		RegCloseKey(hKey);
	}

	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		SetSysValue (const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType, void *pDest, DWORD dwSize)
/ /
/ /		szSubkey = name of the subkey to look for the values in
/ /		szValue = name of the value to read data from (use NULL for default value)
/ /		dwType = type of data to set: REG_DWORD, REG_SZ, etc.
/ /		pDest = destination buffer to return the data in, can be of any type
/ /		dwSize = size of the destination buffer (buffer size always, not length of chars for string data)
/ /
/ / PURPOSE:
/ /		Function intended to serve as a means to repeat the repeatative operations
/ /		needed to access registry value data frequently from the local machine hive.
/*/

bool
SetSysValue (const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType, void *pDest, DWORD dwSize)
{
	// since this is a wrapper function, call the real one to get the data
	return SetRegValue(HKEY_LOCAL_MACHINE, szSubkey, szValue, dwType, pDest, dwSize);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / bool
/ /		SetUserValue (const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType, void *pDest, DWORD dwSize)
/ /
/ /		szSubkey = name of the subkey to look for the values in
/ /		szValue = name of the value to read data from (use NULL for default value)
/ /		dwType = type of data to set; REG_DWORD, REG_SZ, etc.
/ /		pDest = destination buffer to return the data in, can be of any type
/ /		dwSize = size of the destination buffer (buffer size always, not length of chars for string data)
/ /
/ / PURPOSE:
/ /		Function intended to serve as a means to repeat the repeatative operations
/ /		needed to access registry value data frequently from the local user hive.
/ /
/ / NOTE:
/ /		This will prepend company and software name before the subkey that's pass to automatically
/ /		keep the data orginaized in the registry and so the caller needn't worry about it.
/*/

bool
SetUserValue (const LPTSTR szSubkey, const LPTSTR szValue, DWORD dwType, void *pDest, DWORD dwSize)
{
	TCHAR szPath[MAX_LOADSTRING] = {0};

	// we need to create the string to prepend to the passed subkey (saves time for the caller)
	// to do this, we'll concat the passed value after the company name and app name constants
	_tcscpy_s(szPath, SIZE(szPath), _T("Software\\"));
	_tcscat_s(szPath, SIZE(szPath), COMPANY_NAME);
	_tcscat_s(szPath, SIZE(szPath), _T("\\"));
	_tcscat_s(szPath, SIZE(szPath), APP_NAME);

	// this can be left null if company/software name is sufficient
	if(szSubkey != NULL)
	{
		_tcscat_s(szPath, SIZE(szPath), _T("\\"));
		_tcscat_s(szPath, SIZE(szPath), szSubkey);
	}

	// since this is a wrapper function, call the real one to get the data
	return SetRegValue(HKEY_CURRENT_USER, szPath, szValue, dwType, pDest, dwSize);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*/
/ / void
/ /		SetVSync (VSYNC Status)
/ /
/ /		Status = flag to indicate if vsync will be enabled or disabled or left alone
/ /
/ / PURPOSE:
/ /		This is will either enable or disable vertical sync using a wiggle extension.
/ /
/ / NOTE:
/ /		On nVidia cards, this can be overridden so an application cannot change this.
/ /		Also, like most routines, this needs to be called after there's a valid RC.
/*/

void
SetVSync (VSYNC Status)
{
	// this does not support Unicode, but that's ok because the user
	// will never see the string data that we test with
	const char *szExt = (char *)glGetString(GL_EXTENSIONS);

	// only attempt this if the extension is supported
	if((szExt != NULL) && (strstr(szExt, "WGL_EXT_swap_control") != NULL))
	{
		typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC) (int);
		PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = NULL;

		// this accepts one param, 0 = off and 1 = on
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress(_T("wglSwapIntervalEXT"));

		switch(Status)
		{
			case VSync_On:
				if(wglSwapIntervalEXT != NULL) wglSwapIntervalEXT(true);
				break;

			case VSync_Off:
				if(wglSwapIntervalEXT != NULL) wglSwapIntervalEXT(false);
				break;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////