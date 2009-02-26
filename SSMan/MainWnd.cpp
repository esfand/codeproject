#include "StdAfx.h"
#include "mainwnd.h"
#include "resource.h"

#ifdef ENABLE_SS_START_HOOK_MSG
HHOOK hHook;
UINT WM_SCRNSVSTART;
#endif

CMainWnd::CMainWnd(void)
{
#ifdef ENABLE_SS_START_HOOK_MSG
	hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)HookProc, _Module.GetModuleInstance(), 0);
	WM_SCRNSVSTART = RegisterWindowMessage(SSMSG);
#endif
	m_nSSEnabled = IsSSEnabled();
}

CMainWnd::~CMainWnd(void)
{
#ifdef ENABLE_SS_START_HOOK_MSG
	UnhookWindowsHookEx(hHook);
#endif
	if (-1 != m_nSSEnabled)
		EnableSS(m_nSSEnabled);
	m_TrayIcon.Clear();
}

LRESULT CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_Icon.LoadIcon(IDR_MAINFRAME, 16, 16/*, LR_SHARED*/);
	
	if(FALSE == m_TrayIcon.Create(_Module.GetModuleInstance(), m_hWnd, WM_ICON_NOTIFY, _T("ScreenSaver Manager"), m_Icon, IDR_MAIN_MENU))
	{
		ATLTRACE(_T("Tray icon creation failed!\n"));
		SetMsgHandled(FALSE);
		return -1;
	}
	CheckSSStatus();
	return 0;
}

void CMainWnd::CheckSSStatus(void)
{
	int nSSEnabled = IsSSEnabled();
	if (-1 != nSSEnabled)
	{
		if (nSSEnabled)
		{
			m_TrayIcon.CheckItem(IDM_ENABLESCREENSAVER, MF_CHECKED | MF_BYCOMMAND);
			m_TrayIcon.CheckItem(IDM_DISABLESCREENSAVER, MF_UNCHECKED | MF_BYCOMMAND);
		}
		else
		{
			m_TrayIcon.CheckItem(IDM_ENABLESCREENSAVER, MF_UNCHECKED | MF_BYCOMMAND);
			m_TrayIcon.CheckItem(IDM_DISABLESCREENSAVER, MF_CHECKED | MF_BYCOMMAND);
		}
	}
}
