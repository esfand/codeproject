// FileControls.h
//
// File selection control classes for mobile and desktop applications
// Copyright (c) Alain Rist 2007
//
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php).
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.

#ifndef _FILECONTROLS_H
#define _FILECONTROLS_H

#pragma once

#ifndef _WTL_VER
	#error atldlgx.h requires the Windows Template Library
#elif _WTL_VER < 0x800
	#error atldlgx.h requires WTL version 8.0 or over
#endif

#if !defined(_WTL_USE_CSTRING) && !defined(__ATLSTR_H__)
	#error atlfsel.h requires ATL::CString or WTL::CString support
#endif

#ifndef __ATLMISC_H__
	#error atlfsel.h requires atlmisc.h to be included first
#endif

#include <atlctrls.h>
#include <atlctrlx.h>


///////////////////////////////////////////////////////////////////////////////
// Macro
#ifndef MESSAGE_ANSWER
	#define MESSAGE_ANSWER(msg, res) \
		if(uMsg == msg) \
		{ \
			lResult = res; \
			return bHandled = TRUE; \
		}
#endif

/////////////////////////////////////////////////////////////////////////////////:
// Common filter classes
//

class CDirFilter
{
public:
	bool operator()(CFindFile& ff)
	{
		return ff.IsDirectory() != FALSE;
	}
};

class CFileFilter
{
public:
	bool operator()(CFindFile& ff)
	{
		return !ff.IsDirectory();
	}
};

template <class TFilter>
class CDirOr : public TFilter
{
public:
	bool operator()(CFindFile& ff)
	{
		return ff.IsDirectory() ? true :  TFilter::operator()(ff);
	}
};

/////////////////////////////////////////////////////////////////////////////////
// CFileTreeCtrlF: File tree control

#define FTCN_SELECT TVN_LAST // File tree control notification code

typedef ATL::CWinTraits<WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | 
	TVS_HASLINES | TVS_LINESATROOT |TVS_SHOWSELALWAYS
	> CFileTreeTraits;

template <class TFilter, class TFinder = WTL::CFindFile>
class CFileTreeCtrlF : public CWindowImpl<CFileTreeCtrlF<TFilter, TFinder>, CTreeViewCtrlEx>
{
public:
	DECLARE_WND_SUPERCLASS(_T("FileTreeCtrl"), GetWndClassName())

	// Constructor
	CFileTreeCtrlF()
	{
		AtlInitCommonControls(ICC_TREEVIEW_CLASSES);
	}

	// CFileItem helper
	class CFileItem : public CTreeItem
	{
	public:
		CFileItem(CTreeItem item) : CTreeItem(item){}
		CFileItem(TVITEM& item, CFileTreeCtrlF* pF) : CTreeItem(item.hItem, pF){}
		CFileItem(HTREEITEM hItem, CFileTreeCtrlF* pF) : CTreeItem(hItem, pF){}

		CString GetItemPath()
		{
			CString sPath;
			CFileItem parent = GetParent();
			if (parent.IsNull())
			{
				GetText(sPath);
				sPath.TrimRight(_T('\\'));
			}
			else
			{
				CString sName;
				GetText(sName);
				sPath = parent.GetItemPath() + _T("\\") + sName;
			}
			return sPath;
		}
	};

	TFilter m_Filter;
	CImageList m_Iml;
	CString m_sPath, m_sFile;

	LPCTSTR GetFileTreePath()
	{
		return m_sPath;
	} 

	LPCTSTR GetFileTreeName()
	{
		return m_sFile;
	}

	static LPTSTR GetRoots()
	{
#ifdef _WIN32_WCE
		return _T("\\\0");
#else
		static TCHAR sRoots[MAX_PATH];
		GetLogicalDriveStrings(MAX_PATH, sRoots);
		return sRoots;
#endif
	}

	HTREEITEM AddRoot(LPCTSTR sRoot)
	{
		return AddFileItem(TVI_ROOT, sRoot, (LPTSTR)sRoot);
	}

	void Init(LPCTSTR sRoot = NULL)
	{
		SHFILEINFO sfi;

		m_Iml = (HIMAGELIST)SHGetFileInfo(_T("*"), 0, &sfi, sizeof(SHFILEINFO), 
			SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

		SetImageList(m_Iml, TVSIL_NORMAL);
		if (sRoot)
			AddRoot(sRoot);
		else
			for (sRoot = GetRoots(); *sRoot; sRoot += lstrlen(sRoot) + 1)
				AddRoot(sRoot);
		SelectItem(GetRootItem());
		}

// implementation
	HTREEITEM AddFileItem(HTREEITEM hParentItem, LPCTSTR sPathName, LPTSTR sFileName)
	{
		SHFILEINFO sfi;
		SHGetFileInfo(sPathName, 0, &sfi, sizeof(SHFILEINFO),  SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ATTRIBUTES);
		TVITEM item = {TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT, 0, 0, 0,
			sFileName, 0, sfi.iIcon, sfi.iIcon, sfi.dwAttributes & SFGAO_FOLDER ? 1 : 0};
#ifdef SHGFI_OPENICON
		SHGetFileInfo(sPathName, 0, &sfi, sizeof(SHFILEINFO),  SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);
		item.iSelectedImage = sfi.iIcon;
#endif
		TVINSERTSTRUCT tvi = {hParentItem, TVI_LAST/*, item*/};
		tvi.item = item;
		return InsertItem(&tvi);
	}

	void ExpandFileItem(TVITEM& item)
	{
		CFileItem fi(item, this);
		CString sPath = fi.GetItemPath();
		int iChildren = 0;
		CWaitCursor wc;

		TFinder ff;
		for (ff.FindFile(sPath + _T("\\*")); ff.m_bFound; ff.FindNextFile())
			if(m_Filter(ff))
			{
				AddFileItem(fi, ff.GetFilePath(), (LPTSTR)(LPCTSTR)ff.GetFileName());
				iChildren++;
			}
			
		if( iChildren != 1)
		{
			item.mask = TVIF_CHILDREN;
			item.cChildren = iChildren;
			SetItem(&item);
		}
	}

    BEGIN_MSG_MAP(CFileTreeCtrlF)
		MESSAGE_ANSWER(WM_GETDLGCODE, DLGC_WANTARROWS)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
#if defined WIN32_PLATFORM_WFSP // SmartPhone tree controls do not handle all keyboard navigation
		MESSAGE_HANDLER(WM_KEYUP, OnKey)
#endif
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnFileSel)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING, OnFileExpand)
    END_MSG_MAP()

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		LRESULT lr = DefWindowProc();
		EnsureVisible(GetSelectedItem());
		return lr;
	}

#if defined WIN32_PLATFORM_WFSP // SmartPhone tree controls do not handle all keyboard navigation
	LRESULT OnKey(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		switch ( wParam)
		{
		case VK_RIGHT:
			GetSelectedItem().Expand(TVE_EXPAND);
			break;
		case VK_LEFT:
			GetSelectedItem().Expand(TVE_COLLAPSE);
			break;
#if (_WIN32_WCE < 0x501) // Mobile 5 handles that
		case VK_DOWN:
			SelectItem(GetSelectedItem().GetNextVisible());
			break;
		case VK_UP:
			SelectItem(GetSelectedItem().GetPrevVisible());
			break;
#endif // _WIN32_WCE < 0x501
		default: return DefWindowProc();
		}
		return 0;
	}
#endif // defined WIN32_PLATFORM_WFSP

	LRESULT OnFileSel(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		CFileItem fi(((LPNMTREEVIEW)pnmh)->itemNew, this);
		CString sNewPath = fi.GetItemPath();
		if ( sNewPath != m_sPath)
		{
			m_sPath = sNewPath;
			fi.GetText(m_sFile);
			pnmh->code = FTCN_SELECT;
			::SendMessage(GetParent(), WM_NOTIFY, 0,(LPARAM)pnmh);
		}
		return FALSE;
	}

	LRESULT OnFileExpand(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)pnmh;
		if((pnmtv->action == TVE_EXPAND) && !(pnmtv->itemNew.state & TVIS_EXPANDEDONCE))
			ExpandFileItem(pnmtv->itemNew);
		return FALSE;
	}
};
////////////////////////////////////////////////////////////////
// CDirTreeCtrl

typedef CFileTreeCtrlF<CDirFilter> CDirTreeCtrl;

/////////////////////////////////////////////////////////////////////////////////:
// CFileListCtrlF

#define FLCN_SELECT LVM_FIRST - 1 // File list control notification code

#ifndef LVS_EX_GRADIENT // Mobile 2003 and over specific
#define LVS_EX_GRADIENT 0
#endif

typedef ATL::CWinTraits<WS_VISIBLE | WS_CHILD | LVS_AUTOARRANGE | 
	LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS, 
	LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT | LVS_EX_ONECLICKACTIVATE
	> CFileListTraits;

template <class TFilter, class TFinder = WTL::CFindFile>
class CFileListCtrlF : public CSortListViewCtrlImpl<CFileListCtrlF< TFilter, TFinder> >
{
public:
	typedef CSortListViewCtrlImpl< CFileListCtrlF<TFilter, TFinder> > baseSLV;

	DECLARE_WND_SUPERCLASS(_T("FileListCtrl"), GetWndClassName())

	// Constructor
	CFileListCtrlF()
	{
		memset(m_aWidth, 0, 3 * sizeof(int));
		AtlInitCommonControls(ICC_LISTVIEW_CLASSES);
	}

	// Data members
	TFilter m_Filter;
	CString m_sFile;
	CString m_sPath;
	CImageList m_ImlLarge, m_ImlSmall;
	int m_aWidth[3];

// implementation
	void SetColumns()
	{
		static struct _FILECOLUMN
		{
			LPCTSTR		stitle;
			USHORT		uformat;
			INT			isize;
			WORD		wsort;		
		} 
		cols[] = 
		{
			{_T("Name"), LVCFMT_LEFT, LVSCW_AUTOSIZE_USEHEADER, LVCOLSORT_TEXT},
			{_T("Size"), LVCFMT_RIGHT, LVSCW_AUTOSIZE_USEHEADER, LVCOLSORT_LONG},
			{_T("Date"), LVCFMT_LEFT, LVSCW_AUTOSIZE_USEHEADER, LVCOLSORT_DATE}
		};

		for (int i = 0; i < 3; i++)
		{
			InsertColumn(i, cols[i].stitle, cols[i].uformat);
			baseSLV::SetColumnWidth(i, m_aWidth[i] = cols[i].isize);
			SetColumnSortType(i, cols[i].wsort);
		}
	}

	void AdjustColumnsWidth(bool bAll = false)
	{
		int vt = GetViewType();
		if ((vt == LVS_REPORT) || (vt == LVS_LIST))
		{
			SetRedraw(FALSE);
			for (int i = 0; i < (vt == LVS_REPORT ? 3 : 1); i++)
				if ((m_aWidth[i] < 0) || bAll)
					baseSLV::SetColumnWidth(i, m_aWidth[i]);
			SetRedraw(TRUE);
		}
	}

	BOOL SetColumnWidth(int nCol, int cx)
	{
		m_aWidth[nCol] = cx;
		return baseSLV::SetColumnWidth(nCol, cx);
	}

	static CString& FFsize(TFinder& ff, CString& sText)
	{
		sText.Format(_T("%lu"),(ULONG)ff.GetFileSize());
		return sText;
	}

	static CString& FFtime(TFinder& ff, CString& sText)
	{
		FILETIME ft;
		SYSTEMTIME st;
		ff.GetLastWriteTime(&ft);
		::FileTimeToSystemTime(&ft, &st);
		::GetDateFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, sText.GetBuffer(40), 40);
		sText.ReleaseBuffer();
		return sText;
	}

	int AddFile(TFinder& ff)
	{
		SHFILEINFO sfi;
		SHGetFileInfo(ff.GetFilePath(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

		CString sText = ff.GetFileName();
		int i = baseSLV::AddItem( 0, 0, sText, sfi.iIcon);
		ATLASSERT(i != -1);

		SetItemText(i, 1, sText = FFsize(ff, sText));
		SetItemText(i, 2, sText = FFtime(ff, sText));
		return i;
	}

	LPCTSTR GetFileListPath()
	{
		return  m_sPath;
	} 

	LPCTSTR GetFileListName()
	{
		return m_sFile;
	}

	void GetFileListFullName(CString& sFullName)
	{
		sFullName = m_sPath + _T("\\") + m_sFile;
	}

	void Init(DWORD dwViewType = LVS_REPORT)
	{
		SHFILEINFO sfi;
		m_ImlLarge = (HIMAGELIST)SHGetFileInfo(_T("*"), 0, &sfi, sizeof(SHFILEINFO), 
			SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
		SetImageList(m_ImlLarge, LVSIL_NORMAL);

		m_ImlSmall = (HIMAGELIST)SHGetFileInfo(_T("*"), 0, &sfi, sizeof(SHFILEINFO), 
			SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
		SetImageList(m_ImlSmall, LVSIL_SMALL);

		SetViewType(LVS_REPORT);
		SetColumns();
		if (dwViewType != LVS_REPORT)
			SetViewType(dwViewType);
	}

	void SetFiles(LPCTSTR sPath, bool bKeepSelection = false)
	{
		CString s;
		TFinder ff;
		int i = 0;

		CWaitCursor wc;
		m_sPath =sPath;
		SetRedraw(FALSE);

		DeleteAllItems();
		for (ff.FindFile(m_sPath  + _T("\\*")); ff.m_bFound; ff.FindNextFile())
			if(m_Filter(ff))
				AddFile(ff);
		if (GetViewType() == LVS_REPORT)
			AdjustColumnsWidth();
		if (int iCol = GetSortColumn() != -1)
			SortItems(iCol);
		SetRedraw(TRUE);

		if( !m_sFile.IsEmpty() && bKeepSelection)
		{
			LVFINDINFO lvfi = {LVFI_STRING, m_sFile};
			if ((i = FindItem( &lvfi, -1)) != -1 && SelectItem(i))
				return;
		}

		SelectItem(0);
	}

	BEGIN_MSG_MAP(CFileListCtrlF)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED , OnItemChanged)
		REFLECTED_NOTIFY_CODE_HANDLER(SLVN_SORTCHANGED, OnSortChanged)
		CHAIN_MSG_MAP(baseSLV)
	END_MSG_MAP()

	LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)pnmh;
		if (pnmlv->uNewState & LVIS_FOCUSED)
		{
			GetItemText(pnmlv->iItem, 0, m_sFile);
			// Notify parent
			pnmh->code = FLCN_SELECT;
			::SendMessage(GetParent(), WM_NOTIFY, 0,(LPARAM)pnmh);
		}
		return FALSE;
	}

	LRESULT OnSortChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		EnsureVisible(GetSelectedIndex(),FALSE);
		return TRUE;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		LRESULT lr = DefWindowProc();
		EnsureVisible(GetSelectedIndex(),FALSE);
		return lr;
	}
};

////////////////////////////////////////////////////////////////
// CFileListCtrl

typedef CFileListCtrlF<CFileFilter> CFileListCtrl;

////////////////////////////////////////////////////////////////

#endif // _FILECONTROLS_H