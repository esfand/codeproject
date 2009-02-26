// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "ToolbarHelperSampleView.h"
#include "MainFrm.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UISetCheck(ID_VIEW_RED, m_view.m_clr == RED);
	UISetCheck(ID_VIEW_GREEN, m_view.m_clr == GREEN);
	UISetCheck(ID_VIEW_BLUE, m_view.m_clr == BLUE);
	// Update the combobox selection
	if (GetFocus() != m_wndCombo && m_wndCombo.GetCurSel() != CB_ERR &&
		m_wndCombo.GetItemData(m_wndCombo.GetCurSel()) != m_view.m_clr)
	{
		switch (m_view.m_clr)
		{
		case RED:
			m_wndCombo.SetCurSel(0);
			break;
		case GREEN:
			m_wndCombo.SetCurSel(1);
			break;
		case BLUE:
			m_wndCombo.SetCurSel(2);
			break;
		}
	}

	UIUpdateToolBar();
	return FALSE;
}

void CMainFrame::OnToolBarCombo(HWND hWndCombo, UINT nID, int nSel, LPCTSTR lpszText, DWORD dwItemData)
{
	if (nID == ID_COMBO_PLACEHOLDER)
	{
		m_view.m_clr = dwItemData;
		m_view.Invalidate();
	}
	AtlMessageBox(*this, lpszText, IDR_MAINFRAME);
}

void CMainFrame::PrepareToolBarMenu(UINT nMenuID, HMENU hMenu)
{
	if (nMenuID == IDR_NEW)
	{
		CMenuHandle menu(hMenu);
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, ID_NEW_DOCUMENT, _T("Document..."));
		menu.AppendMenu(MF_STRING, ID_NEW_TEMPLATE, _T("Template..."));
	}
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	m_CmdBar.LoadImages(IDR_MENU_IMAGES);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE_EX);

	// Add toolbar text
	AddToolbarButtonText(hWndToolBar, ID_FILE_SAVE);
	AddToolbarButtonText(hWndToolBar, ID_APP_ABOUT, _T("About"));
	AddToolbarButtonText(hWndToolBar, ID_EDIT_PASTE, IDS_TOOLBAR_TEXT);

	AddToolBarDropDownMenu(hWndToolBar, ID_VIEW_COLOUR, IDR_COLOUR);
	AddToolbarButtonText(hWndToolBar, ID_VIEW_COLOUR);

	AddToolBarDropDownMenu(hWndToolBar, ID_FILE_NEW, IDR_NEW);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);	

	// Create a combobox on the toolbar
	m_wndCombo = CreateToolbarComboBox(hWndToolBar, ID_COMBO_PLACEHOLDER);
	// Add some items
	m_wndCombo.AddString(_T("Red"));	
	m_wndCombo.AddString(_T("Green"));
	m_wndCombo.AddString(_T("Blue"));
	m_wndCombo.SetItemData(0, RED);
	m_wndCombo.SetItemData(1, GREEN);
	m_wndCombo.SetItemData(2, BLUE);
	m_wndCombo.SetCurSel(0);

	// Create a combobox that doesn't need to be updated
	CComboBox combo = CreateToolbarComboBox(hWndToolBar, ID_COMBO_PLACEHOLDER2, 8, 8);
	for (int i = 1; i <= 20; i++)
	{
		CString strItem;
		strItem.Format(_T("Item %d"), i);
		combo.AddString(strItem);
	}
	combo.SetCurSel(0);

	CreateSimpleStatusBar();

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

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

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnViewColour(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	switch (wID)
	{
	case ID_VIEW_RED:
		m_view.m_clr = RED;
		break;
	case ID_VIEW_GREEN:
		m_view.m_clr = GREEN;
		break;
	case ID_VIEW_BLUE:
		m_view.m_clr = BLUE;
		break;
	}
	m_view.Invalidate();
	return 0;
}

LRESULT CMainFrame::OnViewColourCycle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_view.m_clr == RED)
		m_view.m_clr = GREEN;
	else if (m_view.m_clr == GREEN)
		m_view.m_clr = BLUE;
	else
		m_view.m_clr = RED;
	m_view.Invalidate();
	return 0;
}

LRESULT CMainFrame::OnNewThing(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	switch (wID)
	{
	case ID_NEW_FILE:
		AtlMessageBox(*this, _T("New File"), IDR_MAINFRAME);
		break;
	case ID_NEW_DOCUMENT:
		AtlMessageBox(*this, _T("New Document"), IDR_MAINFRAME);
		break;
	case ID_NEW_TEMPLATE:
		AtlMessageBox(*this, _T("New Template"), IDR_MAINFRAME);
		break;
	}
	return 0;
}