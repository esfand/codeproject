// maindlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINDLG_H__BE0DD3EC_FC72_4B75_BF0F_F8BA63388ED1__INCLUDED_)
#define AFX_MAINDLG_H__BE0DD3EC_FC72_4B75_BF0F_F8BA63388ED1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TitleTipHandler.h"
#include "iconedit.h"

using codeproject::CTitleTipHandler;
using codeproject::CIconEdit;

class CDragListBoxImpl : public CWindowImpl<CDragListBoxImpl, CDragListBox>
{
public:
	typedef CDragListBoxImpl							thisClass;
	typedef CWindowImpl<CDragListBoxImpl, CDragListBox>	baseClass;
	
	BEGIN_MSG_MAP(thisClass)
	END_MSG_MAP()

protected:
	int m_nLast;

public:
	CDragListBoxImpl() : m_nLast(-1)
	{
	}

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		CListBox wndTempListBox;
		HWND hWnd = wndTempListBox.Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
		wndTempListBox.Detach();
		
		if(hWnd)
			SubclassWindow(hWnd);

		return hWnd;
	}

	BOOL SubclassWindow(HWND hWnd)
	{
		ATLASSERT(::IsWindow(hWnd));
		ATLASSERT((::GetWindowLong(hWnd, GWL_STYLE) & (LBS_MULTIPLESEL | LBS_EXTENDEDSEL)) == 0);
		
		if(!::MakeDragList(hWnd))
			return FALSE;

		return baseClass::SubclassWindow(hWnd);
	}

	// Operations
	void DrawInsert(int nItem)
	{
		if (m_nLast != nItem)
		{
			DrawSingle(m_nLast);
			DrawSingle(nItem);
			m_nLast = nItem;
		}
	}

	// Overrides
	BOOL BeginDrag(POINT pt)
	{
		int nIndex = LBItemFromPt(pt);
		return GetCount() - 1 != nIndex;
	}

	void CancelDrag(POINT /*pt*/)
	{
		DrawInsert(-1);
	}

	UINT Dragging(POINT pt)
	{
		int nIndex = LBItemFromPt(pt, FALSE); // don't allow scrolling just yet
		DrawInsert(nIndex);
		LBItemFromPt(pt);
		return (nIndex == LB_ERR) ? DL_STOPCURSOR : DL_MOVECURSOR;
	}

	int Dropped(POINT pt)
	{
		int nSrcIndex = GetCurSel();

		if(GetCount() - 1 == nSrcIndex)
			return LB_ERR;

		ATLASSERT(!(GetStyle() & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE)) ||
			(GetStyle() & LBS_HASSTRINGS));
		
		DrawInsert(-1);
		int nDestIndex = LBItemFromPt(pt);
		
		if (nSrcIndex == -1 || nDestIndex == -1)
			return LB_ERR;
		if (nDestIndex == nSrcIndex || nDestIndex == nSrcIndex+1)
			return LB_ERR; //didn't move
		DWORD dwData;
		int len = GetTextLen(nSrcIndex);
		LPTSTR pstr = (LPTSTR)_alloca((len + 1)*sizeof(TCHAR));
		ATLASSERT(pstr);
		GetText(nSrcIndex, pstr);
		dwData = GetItemData(nSrcIndex);
		DeleteString(nSrcIndex);
		if (nSrcIndex < nDestIndex)
			nDestIndex--;
		nDestIndex = InsertString(nDestIndex, pstr);
		SetItemData(nDestIndex, dwData);
		SetCurSel(nDestIndex);

		return nDestIndex;
	}

// Implementation
public:
	void DrawSingle(int nIndex)
	{
		if (nIndex == -1)
			return;

		CBrushHandle brush = CDC::GetHalftoneBrush();
		RECT rect;
		GetClientRect(&rect);

		CRgn rgn;
		rgn.CreateRectRgnIndirect(&rect);
		
		CDC dc = GetDC();
		// prevent drawing outside of listbox
		// this can happen at the top of the listbox since the listbox's DC is the
		// parent's DC
		dc.SelectClipRgn(rgn.m_hRgn);
		
		GetItemRect(nIndex, &rect);
		rect.bottom = rect.top+2;
		rect.top -= 2;
		CBrushHandle brushOld = dc.SelectBrush(brush.m_hBrush);
		//draw main line
		dc.PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
		
		dc.SelectBrush(brushOld);

		brush.DeleteObject();
		ReleaseDC(dc);		
	}	
};

class CListBoxImpl : public CWindowImpl<CListBoxImpl, CListBox>
{
public:
	BEGIN_MSG_MAP(CListBoxImpl)
	END_MSG_MAP()

	// NOP
};

class CMainDlg : public CDialogImpl<CMainDlg>, public CTitleTipHandler<CMainDlg>, public CDragListNotifyImpl<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CMainDlg)
		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnToolTipNotifyGetDispInfo)
		CHAIN_MSG_MAP(CTitleTipHandler<CMainDlg>)
		// chain to default message map in CDragListNotifyImpl<T>
		CHAIN_MSG_MAP(CDragListNotifyImpl<CMainDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		//	REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	CIconEdit c_edEdit1;
	CEdit c_edEdit2;
	CEdit c_edEdit3;

	CListBox c_lbList1;
	CDragListBoxImpl c_lbList2;
	CListBoxImpl c_lbList3;

	CComboBox c_cbCombo1;
	CComboBox c_cbCombo2;
	CComboBox c_cbCombo3;

public:
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LRESULT OnToolTipNotifyGetDispInfo(int /*nID*/, LPNMHDR lpnmhdr, BOOL& bHandled)
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Custom ToolTip notification handler for IDC_EDIT3 edit control
		// (share CTitleTipHandler's ToolTip control window and private storage)
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		LPNMTTDISPINFO lpnmtdi= reinterpret_cast<LPNMTTDISPINFO>(lpnmhdr);

		HWND hwndTool = reinterpret_cast<HWND>(lpnmtdi->hdr.idFrom);

		if(IDC_EDIT3 == ::GetDlgCtrlID(hwndTool))
		{
			bHandled = TRUE;

			lpnmtdi->lpszText = NULL;

			LPCTSTR szCustomToolTip = _T("This ToolTip is not handled by CTitleTipHandler but handled by custom\r\nTTN_GETDISPINFO notification handler in CMainDlg.\r\nHowever this custom handler is sharing ToolTip control window and private storage\r\nfor ToolTip text with CTitleTipHandler.");
			const int nLen = ::_tcslen(szCustomToolTip);
			if(nLen > GetToolTipBufLen())
			{
				if(NULL == ReallocToolTipBuf(nLen))
					return 0;
			}

			::_tcscpy(GetToolTipBuf(), szCustomToolTip);

			lpnmtdi->lpszText = GetToolTipBuf();

			// enable multi-line tooltips
			GetToolTipCtrl().SetMaxTipWidth(500);
			GetToolTipCtrl().SetTipTextColor(::GetSysColor(COLOR_INFOTEXT));
			GetToolTipCtrl().SetTipBkColor(::GetSysColor(COLOR_INFOBK));

			return 0;
		}
		else
			bHandled = FALSE;

		return 0;
	}

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

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// CUSTOM ToolTip (ToolTip which is not handled by CTitleTipHandler but it is using
		// the CTitleTipHandler's ToolTip control window and private storage)
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		c_edEdit3.Attach(GetDlgItem(IDC_EDIT3));

		const UINT uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT; 
		CToolInfo ti(uFlags, m_hWnd, (UINT_PTR)(HWND)c_edEdit3);
		GetToolTipCtrl().AddTool(&ti);

		c_edEdit3.SetWindowText(_T("Pick me!"));

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// EDIT Controls
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		c_edEdit1.SubclassWindow(GetDlgItem(IDC_EDIT1));	// ICON EDIT
		c_edEdit2.Attach(GetDlgItem(IDC_EDIT2));			// Multi-line EDIT

		c_edEdit1.SetWindowText(_T("0. The Really Quick Brown Fox Jumps Over The Very Lazy Dog."));
		c_edEdit1.SetIcon(IDR_MAINFRAME);
		
		c_edEdit2.SetWindowText(_T("0. The Really Quick Brown Fox Jumps Over The Very Lazy Dog.\r\n")
		_T("1. Hello World!\r\n")
		_T("2. Hello World!\r\n")
		_T("3. The Quick Brown Fox Jumps Over The Lazy Dog.\r\n")
		_T("4. The Really Quick Brown Fox Jumps Over The Very Lazy Dog."));

		AddTitleTipEdit(c_edEdit1);
		AddTitleTipEdit(c_edEdit2, RGB(255, 255, 255), RGB(255, 0, 0));

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// LISTBOX Controls
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		c_lbList1.Attach(GetDlgItem(IDC_LIST1));			// simple LISTBOX
		c_lbList2.SubclassWindow(GetDlgItem(IDC_LIST2));	// DRAG LISTBOX

		c_lbList1.AddString(_T("0. The Quick Brown Fox Jumps Over The Lazy Dog."));
		c_lbList1.AddString(_T("1. Hello World!"));
		c_lbList1.AddString(_T("2. Hello World!"));
		c_lbList1.AddString(_T("3. The Quick Brown Fox Jumps Over The Lazy Dog."));
		c_lbList1.AddString(_T("4. The Quick Brown Fox Jumps Over The Lazy Dog."));

		c_lbList2.AddString(_T("0. The Quick Brown Fox Jumps Over The Lazy Dog."));
		c_lbList2.AddString(_T("1. Hello World!"));
		c_lbList2.AddString(_T("2. Hello World!"));
		c_lbList2.AddString(_T("3. The Quick Brown Fox Jumps Over The Lazy Dog."));
		c_lbList2.AddString(_T("4. The Quick Brown Fox Jumps Over The Lazy Dog."));

		c_lbList1.SetCurSel(0);
		
		AddTitleTipListBox(c_lbList1, COLOR_DEFAULT, COLOR_DEFAULT, RGB(255, 255, 0), RGB(0, 0, 255));
		AddTitleTipListBox(c_lbList2);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Both ATL::SubclassWindow()/ATL::UnsubclassWindow() and AddTitleTipXXX()/DelTitleTipXXX() use instance subclassing technique
		// therefore their installation and removal order are very important to make it work correctly.
		// When an application subclasses a subclassed window, the subclasses must be removed in reverse of the order
		// in which they were performed.
		// reference:
		// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnwui/html/msdn_subclas3.asp
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		HWND hwndList3 = NULL;
		{	// instance subclassing level 1 BEGIN
			c_lbList3.SubclassWindow(GetDlgItem(IDC_LIST3));
			hwndList3 = c_lbList3.UnsubclassWindow();
		}	// instance subclassing level 1 END
		
		{	// instance subclassing level 1 BEGIN
			c_lbList3.SubclassWindow(hwndList3);
			
			c_lbList3.AddString(_T("0. The Quick Brown Fox Jumps Over The Lazy Dog."));
			c_lbList3.AddString(_T("1. Hello World!"));
			c_lbList3.AddString(_T("2. Hello World!"));

			{	// instance subclassing level 2 BEGIN
				AddTitleTipListBox(c_lbList3);
				DelTitleTipListBox(c_lbList3);
			}	// instance subclassing level 2 END
			
			hwndList3 = c_lbList3.UnsubclassWindow();
		}	// instance subclassing level 1 END

		{	// instance subclassing level 1 BEGIN
			AddTitleTipListBox(hwndList3);
			{	// instance subclassing level 2 BEGIN
				c_lbList3.SubclassWindow(hwndList3);
				c_lbList3.AddString(_T("3. The Quick Brown Fox Jumps Over The Lazy Dog."));
				c_lbList3.AddString(_T("4. The Quick Brown Fox Jumps Over The Lazy Dog."));
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// COMBOBOX Controls
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		c_cbCombo1.Attach(GetDlgItem(IDC_COMBO1));	// SIMPLE type
		c_cbCombo2.Attach(GetDlgItem(IDC_COMBO2));	// DROPDOWN type
		c_cbCombo3.Attach(GetDlgItem(IDC_COMBO3));	// DROP LIST type

		c_cbCombo1.AddString(_T("0. The Quick Brown Fox Jumps Over The Lazy Dog."));
		c_cbCombo1.AddString(_T("1. Hello World!"));
		c_cbCombo1.AddString(_T("2. Hello World!"));
		c_cbCombo1.AddString(_T("3. The Quick Brown Fox Jumps Over The Lazy Dog."));
		c_cbCombo1.AddString(_T("4. The Quick Brown Fox Jumps Over The Lazy Dog."));

		c_cbCombo2.AddString(_T("0. The Quick Brown Fox Jumps Over The Lazy Dog."));
		c_cbCombo2.AddString(_T("1. Hello World!"));
		c_cbCombo2.AddString(_T("2. Hello World!"));
		c_cbCombo2.AddString(_T("3. The Quick Brown Fox Jumps Over The Lazy Dog."));
		c_cbCombo2.AddString(_T("4. The Quick Brown Fox Jumps Over The Lazy Dog."));

		c_cbCombo3.AddString(_T("0. The Quick Brown Fox Jumps Over The Lazy Dog."));
		c_cbCombo3.AddString(_T("1. Hello World & World!"));
		c_cbCombo3.AddString(_T("2. Hello World & Worlds!"));
		c_cbCombo3.AddString(_T("3. The Quick Brown Fox Jumps Over The Lazy Dog."));
		c_cbCombo3.AddString(_T("4. The Quick Brown Fox Jumps Over The Lazy Dog."));

		c_cbCombo1.SetCurSel(CB_ERR);
		c_cbCombo2.SetCurSel(0);
		c_cbCombo3.SetCurSel(1);

		AddTitleTipComboBox(c_cbCombo1, RGB(255, 255, 0), RGB(0, 0, 0), RGB(255, 255, 0), RGB(0, 0, 0));
		AddTitleTipComboBox(c_cbCombo2, RGB(0, 0, 0), RGB(192, 192, 192));
		AddTitleTipComboBox(c_cbCombo3, RGB(0, 255, 255), RGB(0, 0, 0), RGB(255, 255, 0), RGB(0, 0, 0));

		DelTitleTipComboBox(c_cbCombo3);
		AddTitleTipComboBox(c_cbCombo3);


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// TitleTip(CToolTipCtrl) options
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		GetToolTipCtrl().SetDelayTime(TTDT_INITIAL, 50);		// the length of time the pointer must remain stationary within a tool's bounding rectangle before the ToolTip window appears
		GetToolTipCtrl().SetDelayTime(TTDT_AUTOPOP, 5000);	// the length of time the tool tip window remains visible
		GetToolTipCtrl().SetDelayTime(TTDT_RESHOW, 50);			// the length of time it takes for subsequent ToolTip windows to appear as the pointer moves from one tool to another.

		return TRUE;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: Add validation code 
		EndDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}

	// Overrides for CDragListNotifyImpl
	BOOL OnBeginDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
	{
		return c_lbList2.BeginDrag(ptCursor);
	}

	void OnCancelDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
	{
		c_lbList2.CancelDrag(ptCursor);
	}

	int OnDragging(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
	{
		return c_lbList2.Dragging(ptCursor);
	}

	void OnDropped(int /*nCtlID*/, HWND /*hWndDragList*/, POINT ptCursor)
	{
		c_lbList2.Dropped(ptCursor);
	}
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDLG_H__BE0DD3EC_FC72_4B75_BF0F_F8BA63388ED1__INCLUDED_)
