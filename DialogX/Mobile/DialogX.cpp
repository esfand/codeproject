// DialogX.cpp : main source file for DialogX.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atlmisc.h>
#include <atldlgs.h>
#include <atlsplit.h>
#define _WTL_CE_NO_ZOOMSCROLL
#define _WTL_CE_NO_FULLSCREEN
#include <atlwince.h>

// Runtime device information
class Device
{
	static bool _b2003, _bSP;
public:
	Device()
	{
		OSVERSIONINFO osvi;
		GetVersionEx(&osvi);
		_b2003 = osvi.dwMajorVersion < 5;
		WCHAR sType[32];
		SystemParametersInfo(SPI_GETPLATFORMTYPE, 32, sType, FALSE);
		_bSP = _wcsnicmp(sType, L"SmartPhone", 10) == 0;
	}
	static const bool Is2003()
	{
		return _b2003;
	}
	static const bool IsSmartPhone()
	{
		return _bSP; 
	}
} _device;

bool Device::_b2003, Device::_bSP;
CAppModule _Module;

#ifdef WIN32_PLATFORM_PSPC
#include "resourceppc.h"
#else
#include "resourcesp.h"
#endif

#include "FileControls.h"
#include "atldlgx.h"
#include "DialogX.h"

#include "AboutDlg.h"
#include "DialogXFrame.h"

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = CDialogXFrame::ActivatePreviousInstance(hInstance, lpstrCmdLine);

	if(FAILED(hRes) || S_FALSE == hRes)
	{
		return hRes;
	}

	hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	AtlInitCommonControls(ICC_DATE_CLASSES);
	SHInitExtraControls();

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = CDialogXFrame::AppRun(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}

