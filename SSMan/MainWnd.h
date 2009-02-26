#pragma once
#include <atlcrack.h>
#include "AboutDlg.h"
#include "SystemTray/SystemTraySDK.h"
#include "resource.h"

typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, WS_EX_APPWINDOW> CMainWndTraits;

#ifdef ENABLE_SS_START_HOOK_MSG
extern HHOOK hHook;
extern UINT WM_SCRNSVSTART;
#endif

typedef struct _t_check_item
{
	UINT uIDCheckItem;
	UINT uCheck;
} check_item;

class CCheckedSystemTray : public CSystemTray
{
	typedef std::vector<check_item> check_item_vector;
protected:
	void CustomizeMenu(HMENU hMenu)
	{
		check_item_vector::iterator it;
		for(it = m_vCheckItem.begin(); it != m_vCheckItem.end(); it++)
		{
			::CheckMenuItem(hMenu, it->uIDCheckItem, it->uCheck);
		}
	}
public:
	void CheckItem(UINT uIDCheckItem, UINT uCheck)
	{
		check_item_vector::iterator it;
		for(it = m_vCheckItem.begin(); it != m_vCheckItem.end(); it++)
		{
			if(it->uIDCheckItem == uIDCheckItem)
				break;
		}
		if (it == m_vCheckItem.end())
		{
			check_item ci;
			ci.uIDCheckItem = uIDCheckItem;
			ci.uCheck = uCheck;
			m_vCheckItem.push_back(ci);
		}
		else
		{
			it->uIDCheckItem = uIDCheckItem;
			it->uCheck = uCheck;
		}
	}
	void Clear()
	{
		m_vCheckItem.clear();
	}
private:
	check_item_vector m_vCheckItem;
};

class CMainWnd : public CWindowImpl<CMainWnd, CWindow, CMainWndTraits>
{
public:
	DECLARE_WND_CLASS(_T("SSMan Window"))

	BEGIN_MSG_MAP(CMainWnd)
#ifdef ENABLE_SS_START_HOOK_MSG
		COMMAND_CODE_HANDLER_EX(WM_SCRNSVSTART, OnScrnsvstart)
#endif
		MESSAGE_HANDLER_EX(WM_ICON_NOTIFY, OnIconNotify)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(IDM_EXIT, OnExit)
		COMMAND_ID_HANDLER_EX(IDM_LAUNCHSCREENSAVER, OnLaunchscreensaver)
		COMMAND_ID_HANDLER_EX(IDM_ENABLESCREENSAVER, OnEnablescreensaver)
		COMMAND_ID_HANDLER_EX(IDM_DISABLESCREENSAVER, OnDisablescreensaver)
		COMMAND_ID_HANDLER_EX(IDM_SHOWDESKTOPPROPERTIES, OnShowdesktopProperties)
		COMMAND_ID_HANDLER_EX(IDM_ABOUT, OnAbout)
	END_MSG_MAP()

	CMainWnd(void);
	~CMainWnd(void);
	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
#if	defined(_WIN32_WCE)	//&& _WIN32_WCE	< 211
		BOOL bAltPressed = ((GetKeyState(VK_MENU) &	(1 << (sizeof(SHORT) * 8 - 1)))	!= 0);
		if (LOWORD(lParam) == WM_LBUTTONUP && bAltPressed)
#else
		if (LOWORD(lParam) == WM_RBUTTONUP)
#endif
		{
			CheckSSStatus();
		}
		return m_TrayIcon.OnTrayNotification(wParam, lParam);
	}
	LRESULT OnDestroy(void)
	{
		PostQuitMessage(0);
		return 0;
	}
	LRESULT OnClose(void)
	{
		DestroyWindow();
		return 0;
	}
	LRESULT OnExit(WORD wNotifyCode, WORD wID, HWND hWndCtl)
	{
		OnClose();
		return 0;
	}
	LRESULT OnLaunchscreensaver(WORD wNotifyCode, WORD wID, HWND hWndCtl)
	{
		ActivateSS();
		return 0;
	}
	LRESULT OnEnablescreensaver(WORD wNotifyCode, WORD wID, HWND hWndCtl)
	{
		if (FALSE == IsSSEnabled())
			EnableSS(TRUE);
		CheckSSStatus();
		return 0;
	}

	LRESULT OnDisablescreensaver(WORD wNotifyCode, WORD wID, HWND hWndCtl)
	{
		if (TRUE == IsSSEnabled())
			EnableSS(FALSE);
		CheckSSStatus();
		return 0;
	}
	LRESULT OnShowdesktopProperties(WORD wNotifyCode, WORD wID, HWND hWndCtl)
	{
		WinExec(_T("c:\\windows\\system32\\control.exe desktop"), SW_NORMAL);
		return 0;
	}
	LRESULT OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl)
	{
		CAboutDlg about;
		about.DoModal();
		return 0;
	}
#ifdef ENABLE_SS_START_HOOK_MSG
	LRESULT OnScrnsvstart(WORD wNotifyCode, WORD wID, HWND hWndCtl)
	{
		m_TrayIcon.ShowBalloon(_T("ScreenSaver is about to start"), _T("Starting ScreenSaver"), NIIF_INFO, 5);
		return 0;
	}
#endif

private:
	CCheckedSystemTray m_TrayIcon;
	CIcon m_Icon;
	int m_nSSEnabled;

	static int IsSSEnabled(void)
	{
		BOOL pvParam;
		if(!SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &pvParam, 0))
			return -1;
		return pvParam;
	}
	static int EnableSS(BOOL bEnable = TRUE)
	{
		return SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, bEnable, 0, 0);
	}
	int ActivateSS()
	{
		return SendMessage(WM_SYSCOMMAND, SC_SCREENSAVE, 0);
	}
#ifdef ENABLE_SS_START_HOOK_MSG
	static LRESULT CALLBACK HookProc(UINT code, WPARAM wParam, LPARAM lParam)
	{
		MSG *msg = (MSG *)lParam;
		if (msg->message == WM_SYSCOMMAND && msg->wParam == SC_SCREENSAVE)
		{
			// broadcast message to all top-level windows
			// Or execute some other code here
			::PostMessage(HWND_BROADCAST, WM_SCRNSVSTART, 0, 0);
		} 
		// Always call next hook in chain 
		return CallNextHookEx(hHook, code,  wParam, lParam);
	}
#endif
	void CheckSSStatus(void);
};
