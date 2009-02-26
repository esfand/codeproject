// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define IDC_OUTBAR 10000		// WTLOUTBAR

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	CWtlOutBarImpl m_wtlOutbar;	// WTLOUTBAR

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)

		// WTLOUTBAR BEGIN
		NOTIFY_HANDLER(IDC_OUTBAR, OBN_FOLDEROPEN, OnFolderOpen)			
		NOTIFY_HANDLER(IDC_OUTBAR, OBN_ITEMSELECT, OnItemSelect)			
		NOTIFY_HANDLER(IDC_OUTBAR, OBN_ITEMSELECTAUTO, OnItemSelectAuto)
		// WTLOUTBAR END
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);

		// WTLOUTBAR BEGIN

		CImageList il;
		ATLVERIFY(il.Create(32, 32, ILC_COLOR8|ILC_MASK, 6, 1));

		HICON ico;
		ATLVERIFY((ico = ::LoadIcon(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON1))) != 0);
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY((ico = ::LoadIcon(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON2))) != 0);
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY((ico = ::LoadIcon(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON3))) != 0);
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY((ico = ::LoadIcon(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON4))) != 0);
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY((ico = ::LoadIcon(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON5))) != 0);
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY((ico = ::LoadIcon(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_ICON6))) != 0);
		ATLVERIFY(il.AddIcon(ico) != -1);

		RECT rect;				
		ATLVERIFY(GetClientRect(&rect));
		rect.right /= 3;		

		ATLVERIFY(m_wtlOutbar.Create(m_hWnd, &rect, NULL, WS_CHILD|WS_VISIBLE|OBS_FORCESELECT, 0, IDC_OUTBAR));

		ATLVERIFY(m_wtlOutbar.SetFolderCount(10));
		
		m_wtlOutbar.SetImageList(il, OBSIL_NORMAL);

		ATLVERIFY(m_wtlOutbar.AddFolder(OBFF_TEXT|OBFF_PARAM, _T("[A]"), 1) != -1);	
		ATLVERIFY(m_wtlOutbar.AddFolder(OBFF_TEXT|OBFF_PARAM, _T("[B]"), 2) != -1);
		ATLVERIFY(m_wtlOutbar.AddFolder(OBFF_TEXT|OBFF_PARAM, _T("[C]"), 3) != -1);
		ATLVERIFY(m_wtlOutbar.InsertFolder(0, OBFF_TEXT|OBFF_PARAM, _T("[A1]"), 5) != -1);

		ATLVERIFY(m_wtlOutbar.AddItem(0, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[A1 This is a very long long line]"), 0, 0) != -1);
		ATLVERIFY(m_wtlOutbar.AddItem(0, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[A2 This is a very long long line]"), 0, 0) != -1);
		ATLVERIFY(m_wtlOutbar.AddItem(0, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[A2]"), 1, 1) != -1);
		ATLVERIFY(m_wtlOutbar.AddItem(0, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[A3]"), 2, 2) != -1);

		ATLVERIFY(m_wtlOutbar.InsertItem(0, 2, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[A3--]"), 3, 3) != -1);
		ATLVERIFY(m_wtlOutbar.InsertItem(0, 1, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[A2--]"), 4, 4) != -1);
		ATLVERIFY(m_wtlOutbar.InsertItem(0, 0, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[A1--]"), 5, 5) != -1);

		ATLVERIFY(m_wtlOutbar.AddItem(1, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[B1]"), 3, 3) != -1);
		ATLVERIFY(m_wtlOutbar.AddItem(1, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[B2]"), 4, 4) != -1);
		ATLVERIFY(m_wtlOutbar.AddItem(1, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[B3]"), 5, 5) != -1);
		ATLVERIFY(m_wtlOutbar.AddItem(1, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[B4]"), 0, 6) != -1);

		ATLVERIFY(m_wtlOutbar.AddItem(2, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[C1]"), 3, 3) != -1);
		ATLVERIFY(m_wtlOutbar.AddItem(3, OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM, _T("[D1]"), 4, 4) != -1);

		// WTLOUTBAR END

		return TRUE;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: Add validation code 
		CloseDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CloseDialog(wID);
		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		m_wtlOutbar.SetWindowPos(NULL, 0, 0, LOWORD(lParam) / 3, HIWORD(lParam), SWP_NOMOVE|SWP_NOZORDER);

		return 0;
	}

	void CloseDialog(int nVal)
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}

	// WTLOUTBAR BEGIN
	LRESULT OnFolderOpen(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		TCHAR szStr[100];
		::wsprintf(szStr, _T("The Folder #%d has been opened."), m_wtlOutbar.GetOpenFolder());
		::SetWindowText(GetDlgItem(IDC_STATUS), szStr);

		return 0;
	}

	LRESULT OnItemSelect(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		TCHAR szStr[100];
		::wsprintf(szStr, _T("The Item #%d of the Folder #%d has been selected."), m_wtlOutbar.GetSelectedItem(), m_wtlOutbar.GetSelectedFolder());
		::SetWindowText(GetDlgItem(IDC_STATUS), szStr);
		return 0;
	}

	LRESULT OnItemSelectAuto(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		TCHAR szStr[100];
		::wsprintf(szStr, _T("The Item #%d of the Folder #%d has been auto-selected."), m_wtlOutbar.GetSelectedItem(), m_wtlOutbar.GetSelectedFolder());
		::SetWindowText(GetDlgItem(IDC_STATUS), szStr);
		return 0;
	}
	// WTLOUTBAR END
};
