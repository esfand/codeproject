// SSMan.cpp : main source file for SSMan.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "aboutdlg.h"
#include "MainWnd.h"

CAppModule _Module;

#include "LimitSingleInstance.h"

// The one and only CLimitSingleInstance object.
CLimitSingleInstance g_SingleInstanceObj(TEXT("{E2695FD3-7265-456c-8546-AABC0D62C9C9}"));

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainWnd wndMain;

	if(NULL == wndMain.Create(NULL, CWindow::rcDefault, _T("SSMan")))
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 1;
	}

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	if (g_SingleInstanceObj.IsAnotherInstanceRunning())
		return FALSE;

//	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
//	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	int hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
//	::CoUninitialize();

	return nRet;
}
