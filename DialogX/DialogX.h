// DialogX.h
//
// DialogX desktop and mobile sample classes
//
// Copyright (c) Alain Rist 2007
//
// This file should not be compiled out of the DialogX sample applications.


#pragma once

////////////////////////////////////////////////////////////
// CMyToolMenu 

#ifndef _WIN32_WCE
typedef CCellMenu<IDR_MAINFRAME32, 6, 1> CMyToolMenu;

#else
typedef CCellMenu<IDR_MAINFRAME, 6, 1> CMyToolMenu;

#ifdef _WTL_CE_DRA // resolution awareness

const SIZE CMyToolMenu::CellSize()
{
	SIZE size;
	CBitmap bm = AtlLoadBitmap(IDR_MAINFRAME);
	ATLASSERT(!bm.IsNull());

	SIZE sbm;
	bm.GetSize(sbm);
	size.cx = DRA::SCALEX(sbm.cx / (t_nCol * t_nRow)) + 2 * CELL::Cxy();
	size.cy = DRA::SCALEY(sbm.cy) + 2 * CELL::Cxy();
	return size;
}

CImageList& CMyToolMenu::GetImageList()
{
	static CImageList iml;
	if (iml.IsNull())
		iml = DRA::ImageList_LoadImage(ModuleHelper::GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			DRA::UNSCALEX(CELL::Size().cx - 2 * CELL::Cxy()), 0, CLR_DEFAULT, IMAGE_BITMAP, 0);			
	ATLASSERT(!iml.IsNull());
	return iml;
}
#endif // _WTL_CE_DRA
#endif // _WIN32_WCE

////////////////////////////////////////////////////////////
// CMyColorMenu

#ifndef ID_COLORMENU
	#define ID_COLORMENU 1001
#endif 

typedef CCellMenu<ID_COLORMENU, 8, 5> CMyColorMenu;

// Color table
__declspec(selectany) COLORREF colors[] = /* from Chris Maunders */
	{
		RGB(0x00, 0x00, 0x00), RGB(0xA5, 0x2A, 0x00), RGB(0x00, 0x40, 0x40), 
		RGB(0x00, 0x55, 0x00), RGB(0x00, 0x00, 0x5E), RGB(0x00, 0x00, 0x8B),
		RGB(0x4B, 0x00, 0x82), RGB(0x28, 0x28, 0x28),

		RGB(0x8B, 0x00, 0x00), RGB(0xFF, 0x68, 0x20), RGB(0x8B, 0x8B, 0x00),
		RGB(0x00, 0x93, 0x00), RGB(0x38, 0x8E, 0x8E), RGB(0x00, 0x00, 0xFF),
		RGB(0x7B, 0x7B, 0xC0), RGB(0x66, 0x66, 0x66),

		RGB(0xFF, 0x00, 0x00), RGB(0xFF, 0xAD, 0x5B), RGB(0x32, 0xCD, 0x32),
		RGB(0x3C, 0xB3, 0x71), RGB(0x7F, 0xFF, 0xD4), RGB(0x7D, 0x9E, 0xC0),
		RGB(0x80, 0x00, 0x80), RGB(0x7F, 0x7F, 0x7F),

		RGB(0xFF, 0xC0, 0xCB), RGB(0xFF, 0xD7, 0x00), RGB(0xFF, 0xFF, 0x00),
		RGB(0x00, 0xFF, 0x00), RGB(0x40, 0xE0, 0xD0), RGB(0xC0, 0xFF, 0xFF), 
		RGB(0x48, 0x00, 0x48), RGB(0xC0, 0xC0, 0xC0),

		RGB(0xFF, 0xE4, 0xE1), RGB(0xD2, 0xB4, 0x8C), RGB(0xFF, 0xFF, 0xE0),
		RGB(0x98, 0xFB, 0x98), RGB(0xAF, 0xEE, 0xEE), RGB(0x68, 0x83, 0x8B),
		RGB(0xE6, 0xE6, 0xFA), RGB(0xFF, 0xFF, 0xFF)
	};

void CMyColorMenu::PaintCell(HDC hdc, CELL& cell)
{
	CBrush br = CreateSolidBrush(colors[cell.Index()]);

	CDCHandle dc(hdc);
	CBrushHandle brOld = dc.SelectBrush(br); 

	RECT rPaint = cell.Rect(true);
	dc.Rectangle(&rPaint);

	dc.SelectBrush(brOld);
}

////////////////////////////////////////////////////////////
// CMyDateDialog

#ifndef ID_DATEDIALOG
	#define ID_DATEDIALOG 1000
#endif

typedef CWinTraitsOR<MCS_NOTODAY> CMyDateCtrlTraits;

#ifdef _WIN32_WCE

typedef CStdControlDialog<ID_DATEDIALOG, CMonthCalendarCtrl, CMyDateCtrlTraits> CMyDateDialog;

#else

typedef CControlDialog<ID_DATEDIALOG, CMonthCalendarCtrl, CMyDateCtrlTraits> CMyDateDialog;

bool CMyDateDialog::ControlDialog::Init(LPARAM lParam)
{
	ATLASSERT(!lParam || !IsBadWritePtr((LPVOID)lParam, sizeof(SYSTEMTIME)));
	DefInit(lParam);

	CRect rCtrl;
	m_Ctrl.GetMinReqRect(rCtrl);

	ResizeClient(rCtrl.Width(), rCtrl.Height());
	CenterWindow(GetParent());

	return true;
};

#endif


bool CMyDateDialog::ControlDialog::Notify(int, LPNMHDR pnmh)
{
	if(pnmh->code == MCN_SELECT)
		PostMessage(WM_COMMAND, IDOK);
	return false;
};

bool CMyDateDialog::ControlDialog::Command(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	if((wID == IDOK) && m_Data)
		 m_Ctrl.GetCurSel((LPSYSTEMTIME)m_Data);
	return false;
};

////////////////////////////////////////////////////////////
// CStatusPaneEditor 
//

#define STATUS_LENGTH 127 // Max length of pane text

#ifdef _WIN32_WCE

// CEditPaneCtrl, a CEdit with vertical keys sending IDCANCEL to parent +
//		PPC: 'usual' VK_RETURN handling for WM5 
//		SmartPhone: handling VK_TBACK as VK_BACK
//

class CEditPaneCtrl : 
	public CWindowImpl<CEditPaneCtrl, CEdit>
{
public:
	DECLARE_WND_SUPERCLASS(L"EditCtrl", CEdit::GetWndClassName())

	BEGIN_MSG_MAP(CEditPaneCtrl)
		MESSAGE_RANGE_HANDLER(WM_KEYFIRST, WM_KEYLAST, OnKey)
	END_MSG_MAP()

	LRESULT OnKey(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (uMsg == WM_KEYUP)
			switch (wParam)
			{
			case VK_TUP:
			case VK_TDOWN:
				::PostMessage(GetParent(), WM_COMMAND, IDCANCEL, 0);
				break;
#if defined  WIN32_PLATFORM_PSPC
			case VK_RETURN:
				::PostMessage(GetParent(), WM_COMMAND, IDOK, 0);
				break;
#elif defined WIN32_PLATFORM_WFSP
			case VK_TBACK:
				SendMessage(WM_CHAR, VK_BACK);
#endif
			}
		bHandled = FALSE;
		return 1;
	}
};

typedef CInPlaceEditor<STATUS_LENGTH, CEditPaneCtrl> CPaneEditor;

#ifdef WIN32_PLATFORM_PSPC
bool CPaneEditor::Init(LPARAM lParam)
{
	SHSipPreference(m_hWnd, SIP_UP);
	return CInPlaceEditor::DefInit(lParam);
}
#endif // WIN32_PLATFORM_PSPC

#else

typedef CInPlaceEditor<STATUS_LENGTH> CPaneEditor;

#endif

class CStatusPaneEditor 
{
public:
	CStatusPaneEditor(HWND hWndStatusBar) : m_sb(hWndStatusBar)
	{}

	CStatusBarCtrl m_sb;

	bool EditPane(int iPane)
	{
		ATLASSERT(m_sb.IsWindow());

		CRect rPane;
		ATLVERIFY(m_sb.GetRect(iPane, rPane));
		rPane.InflateRect(-1,-1);
		rPane.OffsetRect(0,-1);
		m_sb.MapWindowPoints(m_sb.GetParent(), rPane);

		CTempBuffer<TCHAR> sPane(STATUS_LENGTH);
		m_sb.GetText(iPane, sPane);

		bool bRes = CPaneEditor::Edit(rPane, sPane, m_sb.GetParent());

		if (bRes)
			m_sb.SetText(iPane, sPane);
		return bRes;
	}
};


#ifndef _WIN32_WCE
#if (_WIN32_WINNT >= 0x0600) && defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN)

//////////////////////////////////////////////////////////
// CMyVistaShellDialog : 

#ifndef ID_VISTA_SHELLDIALOG
	#ifndef ID_SHELLDIALOG
		#define ID_VISTA_SHELLDIALOG 1000
	#else
		#define ID_VISTA_SHELLDIALOG ID_SHELLDIALOG
	#endif
#endif

typedef CEmptyDlgTemplate<ID_VISTA_SHELLDIALOG, CSplitDlgTraits> CVistaShellDlgTemplate;

class CMyVistaShellDialog : 
	public CEmptyDialogImpl<CMyVistaShellDialog, 
		ID_VISTA_SHELLDIALOG, CVistaShellDlgTemplate>
{
public:
	CMyVistaShellDialog(SIZE size) : EmptyDialog(size, true)
	{}

	CComPtr<IExplorerBrowser> m_pIEB;

	bool Init(LPARAM lParam)
	{
		EmptyDialog::DefInit(lParam);
		// keep only IDOK button
		CToolBarCtrl tb(m_hWndToolBar);
		tb.HideButton(ID_WINDOW_SPLIT, TRUE);
		tb.HideButton(ID_NEXT_PANE, TRUE);
		tb.HideButton(ID_PREV_PANE, TRUE);

        HRESULT hr = CoCreateInstance(CLSID_ExplorerBrowser, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&m_pIEB));
		ATLASSERT(SUCCEEDED(hr));

		m_pIEB->SetOptions(EBO_SHOWFRAMES);

		FOLDERSETTINGS fs = {FVM_THUMBNAIL, FWF_BESTFITWINDOW};
		CRect rect;
		GetClientRect(rect);
		hr = m_pIEB->Initialize(m_hWnd, rect, &fs);
		ATLASSERT(SUCCEEDED(hr));

		// Required for pre-Vista runtime compatibility
		typedef HRESULT (STDAPICALLTYPE *PFN_SHCreateItemFromParsingName)(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv);
		typedef HRESULT (STDAPICALLTYPE *PFN_SHCreateItemInKnownFolder)(REFKNOWNFOLDERID kfid, DWORD dwKFFlags, PCWSTR pszItem, REFIID riid, void **ppv);
		
		HMODULE hShell32 = ::LoadLibrary(_T("Shell32.dll"));
		ATLASSERT(hShell32 != NULL);

		PFN_SHCreateItemFromParsingName pfnSHCreateItemFromParsingName = (PFN_SHCreateItemFromParsingName)::GetProcAddress(hShell32, "SHCreateItemFromParsingName");
		ATLASSERT(pfnSHCreateItemFromParsingName);

		PFN_SHCreateItemInKnownFolder pfnSHCreateItemInKnownFolder = (PFN_SHCreateItemInKnownFolder)::GetProcAddress(hShell32, "SHCreateItemInKnownFolder");
		ATLASSERT(pfnSHCreateItemInKnownFolder);

        CComPtr<IShellItem> pISI;
		CString sInit((LPTSTR)(lParam));

		if (sInit.IsEmpty() && pfnSHCreateItemInKnownFolder)
			hr = pfnSHCreateItemInKnownFolder(FOLDERID_Desktop, 0, NULL, IID_PPV_ARGS(&pISI));
		else if (pfnSHCreateItemFromParsingName)
			hr = pfnSHCreateItemFromParsingName(sInit, NULL, IID_PPV_ARGS(&pISI));  

		::FreeLibrary(hShell32);
		ATLASSERT(SUCCEEDED(hr));

        hr = m_pIEB->BrowseToObject(pISI, 0);
		ATLASSERT(SUCCEEDED(hr));

		return true;
	}

	bool Size(WPARAM /*wParam*/,LPARAM /*lParam*/)
	{
		CRect rClient;
		GetClientRect(rClient);
		m_pIEB->SetRect(NULL, rClient);
		return true;
	}

	void Close(INT iCmd) 
	{
		if (m_Data && (iCmd == IDOK))
		{
			CComPtr<IFolderView2> pIFV;
			HRESULT hr = m_pIEB->GetCurrentView(IID_PPV_ARGS(&pIFV));
			ATLASSERT(SUCCEEDED(hr));

			int iSel;
			hr = pIFV->GetSelectedItem(-1, &iSel);

			if (SUCCEEDED(hr) && iSel != -1)
			{
				CComPtr<IShellItem> pISI;
				hr = pIFV->GetItem(iSel, IID_PPV_ARGS(&pISI));
				ATLASSERT(SUCCEEDED(hr));

				LPWSTR pszName = NULL;

				if (FAILED(hr = pISI->GetDisplayName(SIGDN_FILESYSPATH, &pszName)))
					hr = pISI->GetDisplayName(SIGDN_NORMALDISPLAY, &pszName);

				if (SUCCEEDED(hr))
					SecureHelper::strcpyW_x((LPWSTR)m_Data, MAX_PATH, pszName);

				CoTaskMemFree(pszName);
			}

		}
		ATLVERIFY(SUCCEEDED(m_pIEB->Destroy()));
		ATLVERIFY(EndDialog(iCmd) != FALSE);
	}

};
#endif // (_WIN32_WINNT >= 0x0600) && defined(NTDDI_VERSION) && (NTDDI_VERSION >= NTDDI_LONGHORN)

//////////////////////////////////////////////////////////
// Shell controls styles

typedef CWinTraitsOR
	<TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | 
	TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS | WS_BORDER | 
	WS_TABSTOP> CShellTreeTraits;

// Common controls V6 compatibility
#ifndef LVS_EX_DOUBLEBUFFER
	#define LVS_EX_DOUBLEBUFFER 0
#endif

typedef CWinTraitsOR
	<LVS_SINGLESEL | LVS_SHOWSELALWAYS | 
    LVS_AUTOARRANGE | LVS_NOCOLUMNHEADER | WS_BORDER | 
    WS_TABSTOP, LVS_EX_DOUBLEBUFFER> CShellListTraits;


//////////////////////////////////////////////////////////
// CMyShellDialog

#if !defined __WTL_AERO_H__

typedef CVSplitDialog<ID_SHELLDIALOG, CShellTreeCtrl, CShellListCtrl, 
	CShellTreeTraits, CShellListTraits> CMyShellDialog;

#else

//////////////////////////////////////////////////////////
// AeroShell controls

class CAeroShellTreeCtrl : 
	public aero::CCtrlImpl<CAeroShellTreeCtrl, CShellTreeCtrl>
{
public:
	CAeroShellTreeCtrl() : CCtrlImpl(VSCLASS_TREEVIEW)
	{}

	void DoPaint(HDC hdc, RECT& rect)
	{
		DefWindowProc(WM_PAINT, (WPARAM) hdc, NULL);
		if (!m_BufferedPaint.IsNull())
			m_BufferedPaint.MakeOpaque(&rect);
	}
};

class CAeroShellListCtrl : 
	public aero::CCtrlImpl<CAeroShellListCtrl, CShellListCtrl>
{
public:
	CAeroShellListCtrl() : CCtrlImpl(VSCLASS_LISTVIEW)
	{}

	void DoPaint(HDC hdc, RECT& rect)
	{
		DefWindowProc(WM_PAINT, (WPARAM) hdc, NULL);
		if (!m_BufferedPaint.IsNull())
			m_BufferedPaint.MakeOpaque(&rect);
	}
};

//////////////////////////////////////////////////////////
// CAeroShellDialog

typedef class CAeroShellDialog :
	public aero::CSplitDialog<
		/*t_uIDD*/ID_VISTASHELLDIALOG, 
		/*TSplitImpl*/aero::CSplitterImpl<
			/*thisClass*/CAeroShellDialog,
			true
			>, // aero::CSplitterImpl
		CAeroShellTreeCtrl, CAeroShellListCtrl, 
		CShellTreeTraits, CShellListTraits
		> // CAeroEmptyDialogImpl  
{
public:
// Constructor
	CAeroShellDialog(SIZE size) : CSplitDialog(size)
	{}

} CMyShellDialog;

#endif // !defined __WTL_AERO_H__

bool CMyShellDialog::SplitDialog::Init(LPARAM lParam)
{
	ATLASSERT(!lParam || !IsBadWritePtr((LPVOID)lParam, MAX_PATH * sizeof(TCHAR)));

	SplitDialog::DefInit(lParam);

#ifdef _UXTHEME_H_
	if (RunTimeHelper::IsVista())
	{
		::SetWindowTheme(m_Left, L"explorer", NULL);
		::SetWindowTheme(m_Right, L"explorer", NULL);
	}
#endif
	m_Right.SetExtendedListViewStyle(CShellListTraits::GetWndExStyle(0));
	m_Left.Populate();
	SetActivePane(SPLIT_PANE_LEFT);
	SetSplitterPosPct(40);

	CToolBarCtrl tb(m_hWndToolBar);
	tb.CheckButton(ID_WINDOW_SPLIT, TRUE);

	return true;
}

bool CMyShellDialog::SplitDialog::Command(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	if (PaneCommand(wID))
	{ 
		if (wID == ID_WINDOW_SPLIT)
			CToolBarCtrl(m_hWndToolBar).CheckButton(ID_WINDOW_SPLIT, GetSinglePaneMode() == SPLIT_PANE_NONE);
		return true;
	}
	else if ((wID == IDOK) && m_Data)
	{
		int iItem = m_Right.GetSelectedIndex();
		if (!m_Right.GetItemPath(iItem, (LPTSTR)m_Data))
			m_Right.GetItemText(iItem, 0, (LPTSTR)m_Data, MAX_PATH);
	}
	return false;
}

bool CMyShellDialog::SplitDialog::Notify(int idCtrl, LPNMHDR pnmh)
{
	switch(pnmh->code)
	{
	case NM_SETFOCUS:
		SetDefaultActivePane(pnmh->hwndFrom);
		{
			CToolBarCtrl tb(m_hWndToolBar);
			tb.EnableButton(ID_NEXT_PANE, GetDefaultActivePane() != SPLIT_PANE_RIGHT);
			tb.EnableButton(ID_PREV_PANE, GetDefaultActivePane() != SPLIT_PANE_LEFT);
		}
		break;
	case NM_CLICK:
		if (idCtrl != ATL_IDM_WINDOW_LAST)
			return false;
		else
		{
			LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)pnmh;
			CPidl pidl;
			m_Right.GetItemPidl(pnmlv->iItem, &pidl);
			m_Left.SelectPidl(pidl);
			break;
		}
	case TVN_SELCHANGED:
		{
			CWaitCursor cursor;
			CPidl pidl;
			m_Left.GetItemPidl(((LPNMTREEVIEW)pnmh)->itemNew.hItem, &pidl);
			m_Right.Populate(pidl);
			m_Right.SelectItem(0);
		}
		break;
	default:
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////
// CBjarkeShellDialog

class CBjarkeShellDialog : public CMyShellDialog
{
public:
	CBjarkeShellDialog(SIZE sz) : CMyShellDialog(sz)
	{}

	CExplorerMenu   m_menu;

	BEGIN_MSG_MAP(CBjarkeShellDialog)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		CHAIN_MSG_MAP_MEMBER(m_menu)
		CHAIN_MSG_MAP(CMyShellDialog)
	END_MSG_MAP()

   LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      if (::GetFocus() == m_Right) 
	  {
		  CPidl pidl;
		  if (m_Right.GetItemPidl(m_Right.GetSelectedIndex(), &pidl) != FALSE) 
			  m_menu.TrackPopupMenu(pidl, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), m_hWnd);
	  }
      return 0;
   }

};
#else

////////////////////////////////////////////////////////////
// CMyFileDialog

#if _WIN32_WCE < 0x500 && defined(WIN32_PLATFORM_WFSP)
// No WS_TABSTOP for SmartPhone 2003 controls
typedef CFileTreeTraits CFileDialogTreeTraits;
typedef CFileListTraits CFileDialogListTraits;
#else
typedef CWinTraitsOR<WS_TABSTOP, 0, CFileTreeTraits> CFileDialogTreeTraits;
typedef CWinTraitsOR<WS_TABSTOP, 0, CFileListTraits> CFileDialogListTraits;
#endif

typedef CStdHSplitDialog<ID_FILEDIALOG, CDirTreeCtrl, CFileListCtrl, 
	CFileDialogTreeTraits, CFileDialogListTraits> CMyFileDialog;

#ifdef WIN32_PLATFORM_PSPC
bool CMyFileDialog::EmptyDialog::Init(LPARAM lParam)
{
	if (Device::Is2003())
		return DefInit(lParam, ID_FILEDIALOG2003, 4);
	else
		return DefInit(lParam);
}
#endif

bool CMyFileDialog::SplitDialog::Init(LPARAM lParam)
{
	ATLASSERT(!lParam || !IsBadWritePtr((LPVOID)lParam, MAX_PATH * sizeof(TCHAR)));

	SplitDialog::DefInit(lParam);
	m_Right.SetExtendedListViewStyle(CFileDialogListTraits::GetWndExStyle(0));
	m_Right.Init(Device::IsSmartPhone() ? LVS_SMALLICON : LVS_REPORT);
	m_Left.Init((LPCTSTR)lParam);
	SetActivePane(SPLIT_PANE_LEFT);
	return false;
}

bool CMyFileDialog::SplitDialog::Notify(int idCtrl, LPNMHDR pnmhdr)
{
	switch(pnmhdr->code)
	{
	case NM_SETFOCUS:
		SetDefaultActivePane(pnmhdr->hwndFrom);
		if (Device::Is2003() && !Device::IsSmartPhone())
		{ // PPC2003 MenuBar
			CMenuBarCtrl mb(::SHFindMenuBar(m_hWnd));
			mb.HideButton(ID_NEXT_PANE, GetDefaultActivePane() == SPLIT_PANE_RIGHT);
			mb.HideButton(ID_PREV_PANE, GetDefaultActivePane() == SPLIT_PANE_LEFT);
			mb.PressButton(ID_WINDOW_SPLIT, GetSinglePaneMode() == SPLIT_PANE_NONE);
		}
		else
			SetMenuBarCommand(ID_NEXT_PANE, ID_NEXT_PANE, 
				GetDefaultActivePane() == SPLIT_PANE_RIGHT ? L"Folders" : L"Files");
		break;
	case NM_RETURN: // PPC2005 will do that
	case NM_DBLCLK:
		PostMessage(WM_COMMAND, IDOK);
		break;
	case FTCN_SELECT:
		m_Right.SetFiles(m_Left.GetFileTreePath());
		break;
	default: 
		return false;
	}
	return true;
}

bool CMyFileDialog::SplitDialog::Command(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	if(wID == IDOK)
	{
		if (m_Data)
		{
			CString sPath;
			m_Right.GetFileListFullName(sPath);
			SecureHelper::strcpyW_x((LPTSTR)m_Data, MAX_PATH, sPath);
		}
		return false;
	}
	return SplitDialog::PaneCommand(wID);
};

#endif // _WIN32_WCE
