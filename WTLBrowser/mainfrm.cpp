// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "WTLBrowserView.h"
#include "MainFrm.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;
	if (pMsg->message==WM_CHAR && m_URL==pMsg->hwnd)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			{
				CString szURL;
				int nLength=m_URL.GetWindowTextLength();
				m_URL.GetWindowText(szURL.GetBuffer(nLength),nLength+1);
				szURL.ReleaseBuffer();
				m_view.Navigate(szURL);
				return TRUE;
			}
		}
		return FALSE;
	}
	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UIEnable(ID_FILE_SAVE,m_view.QueryStatusWB(OLECMDID_SAVE) & OLECMDF_ENABLED);
	UIEnable(ID_FILE_SAVE_AS,m_view.QueryStatusWB(OLECMDID_SAVEAS) & OLECMDF_ENABLED);
	UIEnable(ID_FILE_PRINT,m_view.QueryStatusWB(OLECMDID_PRINT) & OLECMDF_ENABLED);
	UIEnable(ID_FILE_PRINT_PREVIEW,m_view.QueryStatusWB(OLECMDID_PRINTPREVIEW) & OLECMDF_ENABLED);
	UIEnable(ID_FILE_PRINT_SETUP,m_view.QueryStatusWB(OLECMDID_PAGESETUP) & OLECMDF_ENABLED);

	if (GetFocus()==m_URL)
	{
		DWORD dwSelection=m_URL.GetSel();
		BOOL bEnable=HIWORD(dwSelection)!=LOWORD(dwSelection);
		UIEnable(ID_EDIT_CUT,bEnable);
		UIEnable(ID_EDIT_COPY,bEnable);
		if (m_URL.OpenClipboard())
		{
			UIEnable(ID_EDIT_PASTE,IsClipboardFormatAvailable(CF_TEXT));
			CloseClipboard();
		}
		else
			UIEnable(ID_EDIT_PASTE,FALSE);
		UIEnable(ID_EDIT_UNDO,m_URL.CanUndo());
	}
	else
	{
		UIEnable(ID_EDIT_CUT,m_view.QueryStatusWB(OLECMDID_CUT) & OLECMDF_ENABLED);
		UIEnable(ID_EDIT_COPY,m_view.QueryStatusWB(OLECMDID_COPY) & OLECMDF_ENABLED);
		UIEnable(ID_EDIT_PASTE,m_view.QueryStatusWB(OLECMDID_PASTE) & OLECMDF_ENABLED);
		UIEnable(ID_EDIT_UNDO,m_view.QueryStatusWB(OLECMDID_UNDO) & OLECMDF_ENABLED);
	}

	UIEnable(ID_VIEW_GOTO_BACK,m_view.CanBack());
	UIEnable(ID_VIEW_GOTO_FORWARD,m_view.CanForward());
	m_StatusBar.SetPaneIcon(IDR_LOCK,m_view.IsSecured() ? m_hSecured : NULL);
	UIUpdateToolBar();
	UIUpdateStatusBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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

	CString szAddress;
	szAddress.LoadString(IDS_ADDRESS);
	m_URL.Create(m_hWnd,CRect(0,0,0,18),NULL,WS_CHILD|WS_VISIBLE,WS_EX_STATICEDGE);
	AddSimpleReBarBand(m_URL,szAddress.GetBuffer(0),TRUE);
	m_URL.SetFont(AtlGetDefaultGuiFont());
	SHAutoComplete(m_URL,SHACF_URLALL);

	CreateSimpleStatusBar();
	m_StatusBar.SubclassWindow(m_hWndStatusBar);
	int nPanes[]={ID_DEFAULT_PANE,IDR_LOCK,IDR_PROGRESS};
	m_StatusBar.SetPanes(nPanes,sizeof(nPanes)/sizeof(int),false);
	m_StatusBar.SetPaneWidth(IDR_LOCK,30);
	m_StatusBar.SetPaneWidth(IDR_PROGRESS,50);
	
	//TODO: Replace with a URL of your choice
	m_hWndClient = m_view.Create(m_hWnd, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	m_view.GoHome();

	return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.ExecWB(OLECMDID_SAVE,OLECMDEXECOPT_DODEFAULT,NULL,NULL);
	return 0;
}

LRESULT CMainFrame::OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.ExecWB(OLECMDID_SAVEAS,OLECMDEXECOPT_DODEFAULT,NULL,NULL);
	return 0;
}

LRESULT CMainFrame::OnFilePrint(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.ExecWB(OLECMDID_PRINT,OLECMDEXECOPT_DODEFAULT,NULL,NULL);
	return 0;
}

LRESULT CMainFrame::OnFilePrintPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.ExecWB(OLECMDID_PRINTPREVIEW,OLECMDEXECOPT_DODEFAULT,NULL,NULL);
	return 0;
}

LRESULT CMainFrame::OnFilePageSetup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.ExecWB(OLECMDID_PAGESETUP,OLECMDEXECOPT_DODEFAULT,NULL,NULL);
	return 0;
}

LRESULT CMainFrame::OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (GetFocus()==m_URL)
		m_URL.Cut();
	else
		m_view.ExecWB(OLECMDID_CUT,OLECMDEXECOPT_DONTPROMPTUSER,NULL,NULL);
	return 0;
}

LRESULT CMainFrame::OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (GetFocus()==m_URL)
		m_URL.Copy();
	else
		m_view.ExecWB(OLECMDID_COPY,OLECMDEXECOPT_DONTPROMPTUSER,NULL,NULL);
	return 0;
}   

LRESULT CMainFrame::OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (GetFocus()==m_URL)
		m_URL.Paste();
	else
		m_view.ExecWB(OLECMDID_PASTE,OLECMDEXECOPT_DONTPROMPTUSER,NULL,NULL);
	return 0;
}   

LRESULT CMainFrame::OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (GetFocus()==m_URL)
		m_URL.Undo();
	else
		m_view.ExecWB(OLECMDID_UNDO,OLECMDEXECOPT_DONTPROMPTUSER,NULL,NULL);
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewGotoBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.GoBack();
	return 0;
}

LRESULT CMainFrame::OnViewGotoForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.GoForward();
	return 0;
}

LRESULT CMainFrame::OnViewGotoHome(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.GoHome();
	return 0;
}

LRESULT CMainFrame::OnViewStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.Stop();
	return 0;
}

LRESULT CMainFrame::OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.Refresh();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}
