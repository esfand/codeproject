// MainFrm.h : interface of the CMainWnd class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ntray.h"

//////////////////////////////////////////////////////////////////////////
CTrayNotifyIcon g_TrayIcon1;
CTrayNotifyIcon g_TrayIcon2;
CTrayNotifyIcon g_TrayIcon3;
#define WM_TRAYNOTIFY WM_USER + 100
//////////////////////////////////////////////////////////////////////////

class CMainWnd : public CWindowImpl<CMainWnd>
	, public CUpdateUI<CMainWnd>
	, public CMessageFilter
	, public CIdleHandler
{
public:
	DECLARE_WND_CLASS("WTLNTRAY")

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return FALSE;
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainWnd)
		UPDATE_ELEMENT(ID_HAPPY,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_SAD,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_ANIMATED,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_SHOW,UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)

		MESSAGE_HANDLER(WM_TRAYNOTIFY, OnTrayNotification)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		COMMAND_HANDLER(ID_HAPPY, 0, OnHappy)
		COMMAND_HANDLER(ID_SAD, 0, OnSad)
		COMMAND_HANDLER(ID_ANIMATED, 0, OnAnimate)
		COMMAND_HANDLER(ID_HIDE, 0, OnHide)
		COMMAND_HANDLER(ID_SHOW, 0, OnShow)
		COMMAND_HANDLER(ID_APP_EXIT, 0, OnFileExit)

		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CUpdateUI<CMainWnd>)
//		CHAIN_MSG_MAP(CWindowImpl<CMainWnd>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	HWND Create()
	{
//////////////////////////////////////////////////////////////////////////
		m_bHappy = TRUE;
		m_hIcons[0] = CTrayNotifyIcon::LoadIconResource(IDR_HAPPY);
		m_hIcons[1] = CTrayNotifyIcon::LoadIconResource(IDR_SAD);
		m_nTimerCount = 0;
//////////////////////////////////////////////////////////////////////////

		RECT rc;
		memset(&rc,0,sizeof(RECT));
		return CWindowImpl<CMainWnd>::Create(NULL,rc,_T("WTLNTRAY"),WS_POPUPWINDOW,WS_EX_TOOLWINDOW);
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);
//////////////////////////////////////////////////////////////////////////
		if (!g_TrayIcon1.Create(this, IDR_TRAYPOPUP, _T("First Tray Icon"), _T("Demo Tray Application"), _T("First Tray Icon"), 10, CTrayNotifyIcon::User, m_hIcons[0], WM_TRAYNOTIFY, 0, TRUE))
			//if (!g_TrayIcon1.Create(this, IDR_TRAYPOPUP, _T("First Tray Icon"), m_hIcons[0], WM_TRAYNOTIFY)) //Uncomment this line and comment out line above if you want to start with a tray icon without animations
		{
			MessageBox(_T("Failed to create tray icon 1"), "Error",MB_OK | MB_ICONSTOP);
			return -1;
		}
		//For testing purposes, change the default menu item on the first icon to be the bottom item on the context menu
		g_TrayIcon1.SetDefaultMenuItem(ID_APP_ABOUT, FALSE);

		if (!g_TrayIcon2.Create(this, IDR_TRAYPOPUP2, _T("Second Tray Icon"), _T("Demo Tray Application"), _T("Second Tray Icon"), 10, CTrayNotifyIcon::Info, m_hIcons, 2, 600, WM_TRAYNOTIFY, IDR_TRAYPOPUP2))
			//if (!g_TrayIcon2.Create(this, IDR_TRAYPOPUP2, _T("Second Tray Icon"), _T("Demo Tray Application"), _T("Second Tray Icon"), 10, CTrayNotifyIcon::Info, m_hIcons[0], WM_TRAYNOTIFY, IDR_TRAYPOPUP2)) //Uncomment this line and comment out line above if you want a tray icon without animations
		{
			MessageBox(_T("Failed to create tray icon 2"),"Error", MB_OK | MB_ICONSTOP);
			return -1;
		}

		//Create the dynamic systray
		if (!CTrayNotifyIcon::GetDynamicDCAndBitmap(&m_TrayIconDC, &m_BitmapTrayIcon))
		{
			MessageBox(_T("Failed to create tray icon 3"),"Error", MB_OK | MB_ICONSTOP);
			return -1;
		}
		if (!g_TrayIcon3.Create(this, IDR_TRAYPOPUP3, _T("Dynamic Tray Icon"), _T("Demo Tray Application"), _T("Dynamic Tray Icon"), 10, CTrayNotifyIcon::Warning, &m_BitmapTrayIcon, WM_TRAYNOTIFY, IDR_TRAYPOPUP3))
		{
			MessageBox(_T("Failed to create tray icon 3"),"Error", MB_OK | MB_ICONSTOP);
			return -1;
		}

		//Also create a timer used for the dynamic tray icons
		m_nTimerID = SetTimer(1, 1000, NULL);
//////////////////////////////////////////////////////////////////////////
		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		PostMessage(WM_QUIT);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: add code to initialize document

		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnTrayNotification(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=TRUE;
		g_TrayIcon1.OnTrayNotification(wParam, lParam);
		g_TrayIcon2.OnTrayNotification(wParam, lParam);
		g_TrayIcon3.OnTrayNotification(wParam, lParam);
		return 0L;
	}

    LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_nTimerID == wParam)
		{
			//Draw into the tray icon bitmap
			CString sNum;
			sNum.Format(_T("%d"), m_nTimerCount);
			m_TrayIconDC.SetBkMode(TRANSPARENT);
			m_TrayIconDC.SetTextColor(RGB(255,255,255));
			int w = m_TrayIconDC.GetDeviceCaps(LOGPIXELSX);
			int h = m_TrayIconDC.GetDeviceCaps(LOGPIXELSY);
			CRect r;
			r.top=0;
			r.left=0;
			r.right=w;
			r.bottom=h;
			CBrush blackBrush;
			blackBrush.CreateSolidBrush(BLACK_BRUSH);
			m_TrayIconDC.FillRect(&r, blackBrush.m_hBrush);
			m_TrayIconDC.TextOut(0, 0, sNum);

			//Update it
			g_TrayIcon3.SetIcon(&m_BitmapTrayIcon);

			//Increment the count, ready for the next time aroun
			m_nTimerCount++;
			if (m_nTimerCount == 100)
				m_nTimerCount = 0;
		}
		return 0L;

	}

	LRESULT OnHappy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		m_bHappy = !m_bHappy;

		if (m_bHappy)
		{
			g_TrayIcon1.SetIcon(CTrayNotifyIcon::LoadIconResource(IDR_HAPPY));
			g_TrayIcon1.SetTooltipText(_T("I'm Happy!"));
			UISetCheck(ID_HAPPY,true);
			UISetCheck(ID_SAD,false);
		}
		else
		{
			g_TrayIcon1.SetIcon(CTrayNotifyIcon::LoadIconResource(IDR_SAD));
			g_TrayIcon1.SetTooltipText(_T("I'm Sad!"));
			UISetCheck(ID_HAPPY,false);
			UISetCheck(ID_SAD,true);
		}
		return 0L;

	}
	LRESULT OnSad(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		OnHappy(wNotifyCode, wID,hWndCtl,bHandled );
		return 0L;
	}
	LRESULT OnAnimate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		bHandled = TRUE;
		if (g_TrayIcon1.UsingAnimatedIcon())
		{
			g_TrayIcon1.SetIcon(m_hIcons[0]);
			UISetCheck(ID_ANIMATED,false);
		}
		else
		{
			g_TrayIcon1.SetIcon(m_hIcons, 2, 500);
			UISetCheck(ID_ANIMATED,true);
		}
		return 0L;
	}
	LRESULT OnHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		g_TrayIcon1.HideIcon();
		UIEnable(ID_SHOW,true);
		return 0L;
	}
	LRESULT OnShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		UIEnable(ID_SHOW,false);
		g_TrayIcon1.ShowIcon();
		return 0L;
	}
//////////////////////////////////////////////////////////////////////////
protected:
	HICON m_hIcons[2];
	BOOL m_bHappy;
	CDC m_TrayIconDC;
	CBitmap m_BitmapTrayIcon;
	UINT_PTR m_nTimerID;
	int m_nTimerCount;
//////////////////////////////////////////////////////////////////////////
};
