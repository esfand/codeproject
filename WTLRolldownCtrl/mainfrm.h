// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__963EBFEB_E6BE_4313_BEC8_E759E41B9B6C__INCLUDED_)
#define AFX_MAINFRM_H__963EBFEB_E6BE_4313_BEC8_E759E41B9B6C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMainFrame
	: public CFrameWindowImpl<CMainFrame>
	, public CUpdateUI<CMainFrame>
	, public CMessageFilter
	, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CRolloutView m_view;
	CCommandBarCtrl m_CmdBar;
	CPaneContainer  m_PropPane;
	CRolloutContainerClient m_RolloutContainer;
	CRolloutCtrl<CDlg1> m_dlg1;
	CRolloutCtrl<CDlg2> m_dlg2;
	CRolloutCtrl<CDlg3> m_dlg3;
	CRolloutCtrl<CDlg4> m_dlg4;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(m_RolloutContainer.PreTranslateMessage(pMsg))
			return TRUE;

		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	void UpdateLayout(BOOL bResizeBars = TRUE)
	{
		RECT rect;
		GetClientRect(&rect);

		// position bars and offset their dimensions
		UpdateBarsPosition(rect, bResizeBars);

		// resize client window
		if(m_PropPane.m_hWnd != NULL && ::IsWindowVisible(m_PropPane.m_hWnd) == TRUE)
		{
			SIZE ctrlSize;
			m_RolloutContainer.GetClientSize(&ctrlSize);
			int cx = ctrlSize.cx;
			RECT rc;
			rc = rect;
			rect.right -= cx;
			rc.left = rect.right;
			m_PropPane.SetWindowPos(NULL, rc.left, rc.top,
				rc.right - rc.left, rc.bottom - rc.top,
				SWP_NOZORDER | SWP_NOACTIVATE);
		}
		if(m_hWndClient != NULL)
		{
			::SetWindowPos(m_hWndClient, NULL, rect.left, rect.top,
				rect.right - rect.left, rect.bottom - rect.top,
				SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_RANGE_HANDLER(ID_VIEW_TOGGLEROLLOUTCONTROL1STATE, ID_VIEW_TOGGLEROLLOUTCONTROL4STATE, OnToggleRolloutCtrlState)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOGGLEROLLOUTCONTROL1STATE, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_TOGGLEROLLOUTCONTROL2STATE, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_TOGGLEROLLOUTCONTROL3STATE, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_TOGGLEROLLOUTCONTROL4STATE, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// create command bar window
		HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		// attach menu
		m_CmdBar.AttachMenu(GetMenu());
		// load command bar images
		m_CmdBar.LoadImages(IDR_MAINFRAME);
		// remove old menu
		SetMenu(NULL);

		HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

		CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		AddSimpleReBarBand(hWndCmdBar);
		AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
		CreateSimpleStatusBar();

		m_PropPane.Create(m_hWnd, _T("Rollout Control Sample"));
		m_PropPane.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);
		m_RolloutContainer.Create(m_PropPane.m_hWnd);

		m_dlg1.Create(m_RolloutContainer.m_hWnd, _T("Rollout Control 1"));
		m_RolloutContainer.AddRollout(m_dlg1);
		m_dlg2.Create(m_RolloutContainer.m_hWnd, _T("Rollout Control 2"));
//		m_dlg2.EnableWindow(FALSE);
		m_RolloutContainer.AddRollout(m_dlg2);
		m_dlg3.Create(m_RolloutContainer.m_hWnd, _T("Rollout Control 3"));
		m_RolloutContainer.AddRollout(m_dlg3);
		m_dlg4.Create(m_RolloutContainer.m_hWnd, _T("Rollout Control 4"));
		m_RolloutContainer.AddRollout(m_dlg4);
//		m_RolloutContainer.SetSpacing(10);

		m_RolloutContainer.ShowWindow(SW_SHOWNORMAL);
		m_PropPane.SetClient(m_RolloutContainer.m_hWnd);

		m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

		UIAddToolBar(hWndToolBar);
		UISetCheck(ID_VIEW_TOGGLEROLLOUTCONTROL1STATE, 1);
		UISetCheck(ID_VIEW_TOGGLEROLLOUTCONTROL2STATE, 1);
		UISetCheck(ID_VIEW_TOGGLEROLLOUTCONTROL3STATE, 1);
		UISetCheck(ID_VIEW_TOGGLEROLLOUTCONTROL4STATE, 1);
		UISetCheck(ID_VIEW_TOOLBAR, 1);
		UISetCheck(ID_VIEW_STATUS_BAR, 1);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: add code to initialize document

		return 0;
	}

	LRESULT OnToggleRolloutCtrlState(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int  index = wID - ID_VIEW_TOGGLEROLLOUTCONTROL1STATE;
		bool bEnabled = !m_RolloutContainer.IsRolloutEnabled(index);
		m_RolloutContainer.EnableRollout(index, bEnabled);

		UISetCheck(wID, bEnabled);
		UpdateLayout();
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
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MAINFRM_H__963EBFEB_E6BE_4313_BEC8_E759E41B9B6C__INCLUDED_)
