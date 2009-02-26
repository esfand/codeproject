// DialogXFrame.h : interface of the CDialogXFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CDialogXFrame : 
	public CFrameWindowImpl<CDialogXFrame>, 
	public CAppWindow<CDialogXFrame>,
	public CMessageFilter
{
public:
	DECLARE_APP_FRAME_CLASS(NULL, IDR_MAINFRAME, L"Software\\WTL\\DialogX")

	int m_icolor;

	void DateDialog()
	{
		SYSTEMTIME st = {0};
		if(CMyDateDialog().DoModal(m_hWnd, (LPARAM)&st) == IDOK)
		{
			TCHAR szDate[32] = {0};
			::GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, szDate, 32);
			CStatusBarCtrl(m_hWndStatusBar).SetText(0, szDate);
		}
	}

	void ToolMenu(POINT pt)
	{
		static const UINT commands[] = 
		{
			ID_FILEDIALOG, ID_DATEDIALOG, ID_COLORMENU, 
			ID_EDIT_STATUS, ID_TOOLMENU, ID_APP_ABOUT
		};

		int iSel = CMyToolMenu::TrackCellMenu(pt, TPM_BOTTOMALIGN);
		if(iSel != -1)
			PostMessage(WM_COMMAND, commands[iSel]);
	}

	void ColorMenu()
	{
		CRect rect;
		GetClientRect(rect);
		CPoint pt(rect.Size().cx / 2, rect.bottom);
		ClientToScreen(&pt);
		CMenuBarCtrl mb(m_hWndCECommandBar);
		// for PPC2003 menubar
		mb.PressButton(ID_COLORMENU, TRUE);
		int iSel = CMyColorMenu::TrackCellMenu(pt, TPM_CENTERALIGN | TPM_BOTTOMALIGN, m_icolor);
		mb.PressButton(ID_COLORMENU, FALSE);
		if (iSel != -1)
		{
			m_icolor = iSel;
			Invalidate();
		}
	}

	void EditStatusPane(int iPane)
	{
#ifdef WIN32_PLATFORM_PSPC
		SHSipPreference(m_hWnd, SIP_UP);
        CStatusPaneEditor(m_hWndStatusBar).EditPane(iPane);
		SHSipPreference(m_hWnd, SIP_DOWN);
#else
        CStatusPaneEditor(m_hWndStatusBar).EditPane(iPane);
#endif
	}

	void FileDialog()
	{
		CString sPath = L"\\";
		LPTSTR sbuf = sPath.GetBuffer(MAX_PATH);

		if (CMyFileDialog().DoModal(m_hWnd, (LPARAM)sbuf) == IDOK)
			CStatusBarCtrl(m_hWndStatusBar).SetText(0, sbuf);
	}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CFrameWindowImpl<CDialogXFrame>::PreTranslateMessage(pMsg);
	}


	BEGIN_MSG_MAP(CDialogXFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
#ifdef WIN32_PLATFORM_PSPC 
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		NOTIFY_HANDLER(ATL_IDW_STATUS_BAR, NM_CLICK, OnStatusBarClick)
#ifdef WM_CONTEXTMENU
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
#else
		NOTIFY_CODE_HANDLER(GN_CONTEXTMENU, OnContextMenu)
#endif
#endif
		COMMAND_ID_HANDLER(ID_COLORMENU, OnColorMenu)
		COMMAND_ID_HANDLER(ID_DATEDIALOG, OnDateDialog)
		COMMAND_ID_HANDLER(ID_EDIT_STATUS, OnEditStatus)
		COMMAND_ID_HANDLER(ID_FILEDIALOG, OnFileDialog)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_ACTION, OnAction)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CAppWindow<CDialogXFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CDialogXFrame>)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_icolor = 39;

		if (Device::Is2003() && !Device::IsSmartPhone())
			CreateSimpleCEMenuBar(IDR_MAINFRAME, 0, IDR_MAINFRAME, 6);
		else
			CreateSimpleCEMenuBar();
#ifdef WIN32_PLATFORM_WFSP 
			AtlActivateBackKey(m_hWndCECommandBar);
#endif 
		// StatusBar creation 
		CreateSimpleStatusBar();
		// register object for message filtering
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);

		return 0;
	}

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CRect rf;
		GetClientRect(rf);
		CDCHandle((HDC)wParam).FillSolidRect(rf, colors[m_icolor]);

		return TRUE; // background is erased
	}

#if defined WIN32_PLATFORM_PSPC 
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		SHRGINFO shrg = {sizeof(SHRGINFO), m_hWnd, NULL, SHRG_NOTIFYPARENT};
		shrg.ptDown = CPoint(lParam);
		SHRecognizeGesture(&shrg);
		return 0;
	}

	LRESULT OnStatusBarClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		EditStatusPane(0);
		return 0;
	}

#ifdef WM_CONTEXTMENU
	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		ToolMenu(CPoint(lParam));
		return 0;
	}
#else
	LRESULT OnContextMenu(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		PNMRGINFO pnmrgi = (PNMRGINFO)pnmh;
		ToolMenu(pnmrgi->ptAction);
		return 0;
	}
#endif // WM_CONTEXTMENU
#endif // WIN32_PLATFORM_PSPC

	LRESULT OnColorMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ColorMenu();
		return 0;
	}

	LRESULT OnDateDialog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DateDialog();
		return 0;
	}

	LRESULT OnEditStatus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EditStatusPane(0);
		return 0;
	}

	LRESULT OnFileDialog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		FileDialog();
		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnAction(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CRect rect;
		GetWindowRect(rect);
		ToolMenu(CPoint(rect.left, rect.bottom));
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}
};
