// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "ButtonView.h"
#include "MainFrm.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return 0;
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	RECT rcCmdBar = {0, 0, 100, 100};
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcCmdBar, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
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

	// Add toolbars and stuff
	UIAddToolBar(m_CmdBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	UpdateLayout();
	
	RECT ClientRect;
	GetClientRect(&ClientRect);

	// Set the button parameters. You must set these before you create the navwindow. If you don't, don't expect it to work ;)
	m_navwindow.SetButtonHeight(31); // The button height is the height of the big buttons and the command bar
	m_navwindow.SetSmallButtonWidth(24); // The small button width is the width of the little buttons on the command bar.

	// Drop in our control.
	m_hWndClient = m_navwindow.Create(m_hWnd, ClientRect, L"Nav_Main", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_navwindow.SetPopBack(150); // Set the distance from the top of the client rect of the window where the buttons start popping back. 
	                             // If not set or set to 0, the buttons will pop back when the buttons are pushed past the top of the window.

	// For the sake of example, there is only one sample window for all the buttons. 
	// If you want a different window for each button (you probably do), 
	// you will want a different line and class for each window in your Mainframe.
	// As these are for example, the following two lines and the for loop are to be erased and replaced with whatever you want.
	button_view.resize(9);
	button_view_top.resize(9);
	for (size_t view_increment = 0; view_increment < button_view.size(); view_increment++)
	{
		button_view[view_increment].Create(m_navwindow, rcDefault, L"Right Window", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		button_view_top[view_increment].Create(m_navwindow.m_HorizontalSplitter, rcDefault, L"Top Window", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		
		std::wostringstream vertical_caption;
		vertical_caption << L"Vertical " << view_increment;
		button_view[view_increment].window_caption = vertical_caption.str();
		button_view[view_increment].is_horizontal = false;
		
		std::wostringstream horizontal_caption;
		horizontal_caption << L"Horizontal " << view_increment;
		button_view_top[view_increment].window_caption = horizontal_caption.str();
		button_view_top[view_increment].is_horizontal = true;
		button_view_top[view_increment].pop_back_amount = m_navwindow.m_HorizontalSplitter.pop_back_y;
	}
	// Don't erase anything past here...it contains the rest of the parameters needed ;)
	
	// Set the button font.
	m_navwindow.SetButtonFont(L"Arial", 8, FontStyleBold);

	// Add all of our buttons to our button view. Use a for loop if you have the necessary arrays of strings.
	m_navwindow.AddButtonWithView(L"Print", L"RolloutIcons\\Print_2.tif", L"RolloutIcons\\Print_2_Small.tif", button_view[0], button_view_top[0]);
	m_navwindow.AddButtonWithView(L"Add", L"RolloutIcons\\Add_2.tif", L"RolloutIcons\\Add_2_Small.tif", button_view[1], button_view_top[1]);
	m_navwindow.AddButtonWithView(L"Edit", L"RolloutIcons\\Edit_2.tif", L"RolloutIcons\\Edit_2_Small.tif", button_view[2], button_view_top[2]);
	m_navwindow.AddButtonWithView(L"Delete", L"RolloutIcons\\Delete_2.tif", L"RolloutIcons\\Delete_2_Small.tif", button_view[3], button_view_top[3]);
	m_navwindow.AddButtonWithView(L"Search", L"RolloutIcons\\Search_2.tif", L"RolloutIcons\\Search_2_Small.tif", button_view[4], button_view_top[4]);
	m_navwindow.AddButtonWithView(L"Back", L"RolloutIcons\\Back_2.tif", L"RolloutIcons\\Back_2_Small.tif", button_view[5], button_view_top[5]);
	m_navwindow.AddButtonWithView(L"Forward", L"RolloutIcons\\Forward_2.tif", L"RolloutIcons\\Forward_2_Small.tif", button_view[6], button_view_top[6]);
	m_navwindow.AddButtonWithView(L"Save", L"RolloutIcons\\Save_2.tif", L"RolloutIcons\\Save_2_Small.tif", button_view[7], button_view_top[7]);
	m_navwindow.AddButtonWithView(L"Exit", L"RolloutIcons\\Exit_2.tif", L"RolloutIcons\\Exit_2_Small.tif", button_view[8], button_view_top[8]);

	m_navwindow.SetSplitterPos(242); // Set this to be where the vertical splitter will be located on your window.
	m_navwindow.SetDefaultButtons(5); // Set this to be 1 higher than the buttons you want to show initially. If none at all, put 1. If not even the command bar, put 0.

	return 0;
}

LRESULT CMainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_navwindow.DestroyWindow();
	DestroyWindow();
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
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