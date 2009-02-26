// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define IDC_OUTBAR 10000

class CMainFrame : public CMDIFrameWindowImpl<CMainFrame>, 
		public CSplitterImpl<CMainFrame>,
		public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CMDICommandBarCtrl m_CmdBar;
	CWtlOutBarImpl m_wtlOutbar;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CMDIFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		HWND hWnd = MDIGetActive();
		if(hWnd != NULL)
			return (BOOL)::SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM)pMsg);

		return FALSE;
	}

	virtual BOOL OnIdle()
	{
//		UIUpdateToolBar();
		return FALSE;
	}

	void UpdateLayout(BOOL bResizeBars = TRUE)
	{
		RECT rect = { 0 };
		GetClientRect(&rect);

		// position bars and offset their dimensions
		UpdateBarsPosition(rect, bResizeBars);

		SetSplitterRect(&rect);
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNCDestroy)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_WINDOW_CASCADE, OnWindowCascade)
		COMMAND_ID_HANDLER(ID_WINDOW_TILE_HORZ, OnWindowTile)
		COMMAND_ID_HANDLER(ID_WINDOW_ARRANGE, OnWindowArrangeIcons)

		NOTIFY_HANDLER(IDC_OUTBAR, OBN_FOLDEROPEN, OnFolderOpen)
		NOTIFY_HANDLER(IDC_OUTBAR, OBN_ITEMSELECT, OnItemSelect)

		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CSplitterImpl<CMainFrame>)
		CHAIN_MSG_MAP(CMDIFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// create command bar window
		HWND hWndCmdBar;
		ATLVERIFY((hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE)) != NULL);
		// attach menu
		ATLVERIFY(m_CmdBar.AttachMenu(GetMenu()));
		// load command bar images
		ATLVERIFY(m_CmdBar.LoadImages(IDR_MAINFRAME));
		// remove old menu
		ATLVERIFY(SetMenu(NULL));

		HWND hWndToolBar;
		ATLVERIFY((hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE)) != NULL);

		ATLVERIFY(CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE));
		ATLVERIFY(AddSimpleReBarBand(hWndCmdBar));
		ATLVERIFY(AddSimpleReBarBand(hWndToolBar, NULL, TRUE));

		ATLVERIFY(CreateSimpleStatusBar());

		ATLVERIFY(m_wtlOutbar.Create(m_hWnd, rcDefault, NULL, WS_CHILD|WS_VISIBLE|OBS_FORCESELECT, 0, IDC_OUTBAR));
		ATLVERIFY(SetSplitterPane(SPLIT_PANE_LEFT, m_wtlOutbar));

		ATLVERIFY(CreateMDIClient());
		ATLVERIFY(SetSplitterPane(SPLIT_PANE_RIGHT, m_hWndMDIClient));

		ATLVERIFY(SetSplitterPos(100));

		ATLVERIFY(m_CmdBar.SetMDIClient(m_hWndMDIClient));
		ATLVERIFY(m_wtlOutbar.SetFolderCount(10));

		CImageList il;
		ATLVERIFY(il.Create(32, 32, ILC_COLOR8|ILC_MASK, 6, 1));

		CIconHandle ico;
		ATLVERIFY(ico.LoadIcon(IDI_ICON1));
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY(ico.Detach());
		ATLVERIFY(ico.LoadIcon(IDI_ICON2));
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY(ico.Detach());
		ATLVERIFY(ico.LoadIcon(IDI_ICON3));
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY(ico.Detach());
		ATLVERIFY(ico.LoadIcon(IDI_ICON4));
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY(ico.Detach());
		ATLVERIFY(ico.LoadIcon(IDI_ICON5));
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY(ico.Detach());
		ATLVERIFY(ico.LoadIcon(IDI_ICON6));
		ATLVERIFY(il.AddIcon(ico) != -1);
		ATLVERIFY(ico.Detach());

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

		ATLVERIFY(m_wtlOutbar.GetHWNDHandle(-1).ModifyStyleEx(0, WS_EX_STATICEDGE));

		UIAddToolBar(hWndToolBar);
		UISetCheck(ID_VIEW_TOOLBAR, 1);
		UISetCheck(ID_VIEW_STATUS_BAR, 1);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CChildFrame* pChild = new CChildFrame;
		pChild->CreateEx(m_hWndMDIClient);

		// TODO: add code to initialize document

		return 0;
	}

	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MDICascade();
		return 0;
	}

	LRESULT OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MDITile();
		return 0;
	}

	LRESULT OnWindowArrangeIcons(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MDIIconArrange();
		return 0;
	}

	LRESULT OnFolderOpen(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		TCHAR szStr[100];
		::wsprintf(szStr, _T("The Folder #%d has been opened."), m_wtlOutbar.GetOpenFolder());
		MessageBox(szStr, _T("Folder Opened"), 0);

		return 0;
	}

	LRESULT OnItemSelect(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		TCHAR szStr[100];
		::wsprintf(szStr, _T("The Item #%d of the Folder #%d has been selected."), m_wtlOutbar.GetSelectedItem(), m_wtlOutbar.GetSelectedFolder());
		MessageBox(szStr, _T("Item Selected"), 0);

		return 0;
	}
};
