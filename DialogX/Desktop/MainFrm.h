// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainFrame : public CFrameWindowImpl<CMainFrame>
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CToolTipCtrl m_TT;
	int m_icolor;
	int m_Pressed;

#if (_WIN32_WINNT >= 0x0600) && defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN)
	int SelectShellDialog()
	{
		CTaskDialog td(m_hWnd);
		td.ModifyFlags(0, TDF_POSITION_RELATIVE_TO_WINDOW | TDF_USE_COMMAND_LINKS);
		td.SetWindowTitle(ID_VISTA_SHELLDIALOG);
		td.SetMainIcon(TD_INFORMATION_ICON);
		td.SetMainInstructionText(L"Select shell browser dialog:");
		TASKDIALOG_BUTTON tdb[2] = 
		{
			{ID_SHELLDIALOG, L"CBjarkeShellDialog"},
			{ID_SHELLDIALOG + 1, L"CMyVistaShellDialog"}
		};
		td.SetButtons(tdb,2);
		td.SetCommonButtons(TDCBF_CANCEL_BUTTON);

		int iRes = 0;
		ATLVERIFY(td.DoModal(m_hWnd, &iRes) == S_OK);

		return iRes == IDCANCEL ? -1 : iRes - ID_SHELLDIALOG;
	}
#endif // (_WIN32_WINNT >= 0x0600) && defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN)

	void ShellDialog()
	{
		int iRet = IDCANCEL;
		CTempBuffer<TCHAR> sbuf(MAX_PATH);
		*sbuf = _T('\0');

#if (_WIN32_WINNT >= 0x0600) && defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN)

		int iSel = RunTimeHelper::IsVista() ? SelectShellDialog() : 0;

		if (iSel == -1)
				return;
		else if (iSel == 1)
		{
			CMyVistaShellDialog dlg(CSize(600, 400));
			iRet = dlg.DoModal(m_hWnd, (LPARAM)(LPTSTR)sbuf);
		}
		else
#endif // (_WIN32_WINNT >= 0x0600) && defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN)
		{
			CBjarkeShellDialog dlg(CSize(600, 400));
			iRet = dlg.DoModal(m_hWnd, (LPARAM)(LPTSTR)sbuf);
		}
		if (iRet == IDOK)
			CStatusBarCtrl(m_hWndStatusBar).SetText(2, sbuf);
	}

	void DateDialog()
	{
		CMyDateDialog dlg(CSize(150, 150));
		SYSTEMTIME st = {0};
		if(dlg.DoModal(m_hWnd, (LPARAM)&st) == IDOK)
		{
			TCHAR szDate[32] = {0};
			::GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, szDate, 32);
			CStatusBarCtrl(m_hWndStatusBar).SetText(1, szDate);
		}
	}

	void ToolMenu(POINT pt)
	{
		static const UINT commands[] = {ID_SHELLDIALOG, ID_DATEDIALOG, 
			ID_COLORMENU, ID_EDIT_STATUS, ID_TOOLMENU, ID_APP_ABOUT};

		int iSel;
		if (RunTimeHelper::IsCommCtrl6())
			iSel = CMyToolMenu::TrackCellMenu(pt);
		else 
			iSel = CCellMenu<IDR_MAINFRAME, 6, 1>::TrackCellMenu(pt);

		if(iSel != -1)
			PostMessage(WM_COMMAND, commands[iSel]);
	}

	void ColorMenu(POINT pt)
	{
		int iSel = CMyColorMenu::TrackCellMenu(pt, TPM_TOPALIGN, m_icolor);
		if (iSel != -1)
		{
			m_icolor = iSel;
			Invalidate();
		}
	}

	void EditStatusPane(int iPane)
	{
		CStatusPaneEditor(m_hWndStatusBar).EditPane(iPane);
	}

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_COLORMENU, OnColorMenu)
		COMMAND_ID_HANDLER(ID_DATEDIALOG, OnDateDialog)
		COMMAND_ID_HANDLER(ID_EDIT_STATUS, OnEditStatus)
		COMMAND_ID_HANDLER(ID_SHELLDIALOG, OnShellDialog)
		NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnMenu)
		NOTIFY_HANDLER(ATL_IDW_STATUS_BAR, NM_CLICK, OnStatusBarClick)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_icolor = 39;
		m_Pressed = -1;
		SetMenu(NULL);

		DWORD dwStyle = ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST;
		UINT uiTB = RunTimeHelper::IsCommCtrl6() ? IDR_MAINFRAME32 : IDR_MAINFRAME;
		CToolBarCtrl tb = CreateSimpleToolBarCtrl(m_hWnd, uiTB, FALSE, dwStyle);
		tb.SetExtendedStyle(TBSTYLE_EX_MIXEDBUTTONS); 
		for (int iBtn = 0 ; iBtn < tb.GetButtonCount() ; iBtn++)
		{
			TBBUTTONINFO tbbi = {sizeof (TBBUTTONINFO), TBIF_COMMAND | TBIF_STYLE | TBIF_BYINDEX};
			tb.GetButtonInfo(iBtn, &tbbi);
			if (tbbi.fsStyle ^ BTNS_SEP)
			{
				CString sBtn;
				sBtn.LoadString(tbbi.idCommand);
				sBtn = sBtn.Mid(sBtn.Find('\n') + 1);
				tbbi.dwMask = TBIF_TEXT | TBIF_BYINDEX| TBIF_STYLE;
				tbbi.fsStyle |= BTNS_SHOWTEXT;
				if ((tbbi.idCommand == ID_COLORMENU) || (tbbi.idCommand == ID_TOOLMENU))
					tbbi.fsStyle |= BTNS_WHOLEDROPDOWN;
				tbbi.pszText = sBtn.GetBuffer(255);
				tb.SetButtonInfo(iBtn, &tbbi);
			}
		}

		CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		AddSimpleReBarBand(tb);

		CreateSimpleStatusBar(_T("Right click for tool menu"));
		CStatusBarCtrl sb(m_hWndStatusBar);
		// Set 3 panes in the status bar
		int iParts[3] = {150, 400, -1};
		sb.SetParts(3, iParts);

		// Create tooltip
		m_TT.Create(m_hWnd, NULL, NULL, TTS_NOPREFIX | TTS_BALLOON);
		CToolInfo ti(TTF_IDISHWND | TTF_SUBCLASS, sb.m_hWnd, 0, 0, _T("Click to edit"));
		m_TT.AddTool(ti);
		m_TT.Activate(TRUE);

		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnColorMenu(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CPoint pt;
		GetCursorPos(&pt);
		ColorMenu(pt);
		return 0;
	}

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CDCHandle dc = (HDC)wParam;
		CRect rf;
		GetClientRect(rf);
		dc.FillSolidRect(rf, colors[m_icolor]);

		return TRUE; // background is erased
	}

	LRESULT OnMenu(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR)pnmh;
		if (m_Pressed ==  lpnmtb->iItem)
		{
			m_Pressed = -1;
			return TBDDRET_DEFAULT;
		}
		else
			m_Pressed = lpnmtb->iItem;

		CToolBarCtrl tb(lpnmtb->hdr.hwndFrom);
		tb.PressButton(ID_COLORMENU, m_Pressed == ID_COLORMENU);
		tb.PressButton(ID_TOOLMENU, m_Pressed == ID_TOOLMENU);

		CRect rBtn;
		tb.GetRect(m_Pressed, rBtn);

		CPoint pt(rBtn.left, rBtn.bottom);
		tb.ClientToScreen(&pt);
		
		if (m_Pressed == ID_COLORMENU)
			ColorMenu(pt);
		else if (m_Pressed == ID_TOOLMENU)
			ToolMenu(pt);
		else 
			return TBDDRET_NODEFAULT;

		m_Pressed = -1;
		return TBDDRET_DEFAULT;
	}

	LRESULT OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ToolMenu(CPoint(lParam));
		return 0;
	}

	LRESULT OnStatusBarClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		EditStatusPane(((LPNMMOUSE)pnmh)->dwItemSpec);
		return 0;
	}

	LRESULT OnEditStatus(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EditStatusPane(0);
		return 0;
	}

	LRESULT OnDateDialog(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		DateDialog();
		return 0;
	}

	LRESULT OnShellDialog(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		ShellDialog();
		return 0;
	}

};
