#if !defined(_TITLETIPHANDLER_H_INCLUDED_)

#pragma once

/************************************************************************************************************************
 *
 *	CTitleTipHandler
 *	A mix-in class to support TitleTip for Edit, ListBox and ComboBox control
 *
 *	Author: JaeWook Choi
 *	E-mail: zzzzoooo@mail.com
 *
 *	Version 1.01 (10/26/2004)
 *			- Optimized to make it respond faster. (lookup map only once)
 *			- Fixed all the resource leaks found.
 *			- Some bugs fixed. (Edit control related)
 *
 *	Version 1.0 (10/08/2004)
 *			- Initial Release.
 *
 *	This software is provided "as is" without express or implied warranty. Use it at your own risk!
 *
 ************************************************************************************************************************/


#if !defined(_WIN32_IE) || (_WIN32_IE<0x0500)
#error IE 5.0 or later is required (define _WIN32_IE>=0x500)
#endif

#define COLOR_DEFAULT (DWORD)0xffffffff

#pragma warning(push)

#include <yvals.h>

#pragma warning(disable: 4100)

#include <map>

#pragma warning(pop)

#include "TitleTipEditHook.h"
#include "TitleTipListBoxHook.h"
#include "TitleTipComboBoxHook.h"

// refer to BOOL OnEditToolTipNotifyShow();
//	#define _USE_ALT_EM_POSFROM_CHAR_

namespace codeproject
{

template<class T>
class CTitleTipHandler : public CMessageMap
{
public:
	BEGIN_MSG_MAP(CTitleTipHandler)
		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnToolTipNotifyGetDispInfo)
		NOTIFY_CODE_HANDLER(TTN_SHOW, OnToolTipNotifyShow)
		MESSAGE_HANDLER(UWM_TITLE_TIP_EDIT_COMBOBOX, OnTitleTipEditComboBox)
		MESSAGE_HANDLER(UWM_TITLE_TIP_LISTBOX_COMBOBOX, OnTitleTipListBoxComboBox)
	END_MSG_MAP()

private:
	std::map<HWND, CTitleTipHook *> m_mapTitleTip;

	//operations
public:
	BOOL AddTitleTipEdit(HWND hwndEdit, COLORREF crTipTextColor = COLOR_DEFAULT, COLORREF crTipBkColor = COLOR_DEFAULT)
	{
		if(!::IsWindow(hwndEdit))
			return FALSE;

		std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(hwndEdit);
		if(m_mapTitleTip.end() == itFound)
		{
			CTitleTipEditHook *pHook = new CTitleTipEditHook(GetToolTipCtrl(),
				COLOR_DEFAULT == crTipTextColor ? ::GetSysColor(COLOR_WINDOWTEXT) : crTipTextColor,
				COLOR_DEFAULT == crTipBkColor ? ::GetSysColor(COLOR_WINDOW) : crTipBkColor);
			if(!pHook->Hook(hwndEdit))
			{
				pHook->Delete();
				pHook = NULL;

				return FALSE;
			}

			m_mapTitleTip[hwndEdit] = static_cast<CTitleTipHook *>(pHook);

			RECT rcFmt;
			::SendMessage(hwndEdit, EM_GETRECT, 0, (LPARAM)&rcFmt);

			ATLTRACE(_T("AddTitleTipEdit(TOOL(%x), NOTIFIED(%x))\n"), hwndEdit, hwndEdit);
			return AddTitleTip(hwndEdit, hwndEdit, &rcFmt, (LPARAM)pHook);
		}

		return TRUE;
	}

	void DelTitleTipEdit(HWND hwndEdit)
	{
		std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(hwndEdit);
		if(m_mapTitleTip.end() != itFound)
		{
			ATLTRACE(_T("DelTitleTipEdit(TOOL(%x), NOTIFIED(%x))\n"), hwndEdit, hwndEdit);
			DelTitleTip(hwndEdit, hwndEdit);

			CTitleTipEditHook *pHook = static_cast<CTitleTipEditHook *>(itFound->second);
			pHook->Unhook();
			pHook->Delete();
			pHook = NULL;

			m_mapTitleTip.erase(itFound);
		}
	}

	BOOL AddTitleTipListBox(HWND hwndListBox, COLORREF crTipTextColor = COLOR_DEFAULT, COLORREF crTipBkColor = COLOR_DEFAULT, COLORREF crTipTextColorSelection = COLOR_DEFAULT, COLORREF crTipBkColorSelection = COLOR_DEFAULT)
	{
		if(!::IsWindow(hwndListBox))
			return FALSE;

		std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(hwndListBox);
		if(m_mapTitleTip.end() == itFound)
		{
			CTitleTipListBoxHook *pHook = new CTitleTipListBoxHook(GetToolTipCtrl(),
				COLOR_DEFAULT == crTipTextColor ? ::GetSysColor(COLOR_WINDOWTEXT) : crTipTextColor,
				COLOR_DEFAULT == crTipBkColor ? ::GetSysColor(COLOR_WINDOW) : crTipBkColor,
				COLOR_DEFAULT == crTipTextColorSelection ? ::GetSysColor(COLOR_HIGHLIGHTTEXT) : crTipTextColorSelection,
				COLOR_DEFAULT == crTipBkColorSelection ? ::GetSysColor(COLOR_HIGHLIGHT) : crTipBkColorSelection);
			if(!pHook->Hook(hwndListBox))
			{
				pHook->Delete();
				pHook = NULL;

				return FALSE;
			}

			m_mapTitleTip[hwndListBox] = static_cast<CTitleTipHook *>(pHook);

			RECT rcClient;
			if(!::GetClientRect(hwndListBox, &rcClient))
				return FALSE;

			ATLTRACE(_T("AddTitleTipListBox(TOOL(%x), NOTIFIED(%x))\n"), hwndListBox, hwndListBox);
			return AddTitleTip(hwndListBox, hwndListBox, &rcClient, (LPARAM)pHook);
		}

		return TRUE;
	}

	void DelTitleTipListBox(HWND hwndListBox)
	{
		std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(hwndListBox);
		if(m_mapTitleTip.end() != itFound)
		{
			ATLTRACE(_T("DelTitleTipListBox(TOOL(%x), NOTIFIED(%x))\n"), hwndListBox, hwndListBox);
			DelTitleTip(hwndListBox, hwndListBox);

			CTitleTipListBoxHook *pHook = static_cast<CTitleTipListBoxHook *>(itFound->second);
			pHook->Unhook();
			pHook->Delete();
			pHook = NULL;

			m_mapTitleTip.erase(itFound);
		}
	}

	BOOL AddTitleTipComboBox(HWND hwndComboBox, COLORREF crTipTextColor = COLOR_DEFAULT, COLORREF crTipBkColor = COLOR_DEFAULT, COLORREF crTipTextColorSelection = COLOR_DEFAULT, COLORREF crTipBkColorSelection = COLOR_DEFAULT)
	{
		if(!::IsWindow(hwndComboBox))
			return FALSE;

		std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(hwndComboBox);
		if(m_mapTitleTip.end() == itFound)
		{
			CTitleTipComboBoxHook *pHook = new CTitleTipComboBoxHook(GetToolTipCtrl(),
				COLOR_DEFAULT == crTipTextColor ? ::GetSysColor(COLOR_WINDOWTEXT) : crTipTextColor,
				COLOR_DEFAULT == crTipBkColor ? ::GetSysColor(COLOR_WINDOW) : crTipBkColor,
				COLOR_DEFAULT == crTipTextColorSelection ? ::GetSysColor(COLOR_HIGHLIGHTTEXT) : crTipTextColorSelection,
				COLOR_DEFAULT == crTipBkColorSelection ? ::GetSysColor(COLOR_HIGHLIGHT) : crTipBkColorSelection);
			if(!pHook->Hook(hwndComboBox))
			{
				pHook->Delete();
				pHook = NULL;

				return FALSE;
			}

			m_mapTitleTip[hwndComboBox] = static_cast<CTitleTipHook *>(pHook);

			if(CBS_DROPDOWNLIST == (0x0000000f & ::GetWindowLong(hwndComboBox, GWL_STYLE)))
			{
				T *pT = static_cast<T *>(this);
				ATLTRACE(_T("AddTitleTipComboBox(TOOL(%x), NOTIFIED(%x))\n"), hwndComboBox, pT->m_hWnd);
				return AddTitleTip(hwndComboBox, pT->m_hWnd, NULL, (LPARAM)pHook);
			}
		}

		return TRUE;
	}

	void DelTitleTipComboBox(HWND hwndComboBox)
	{
		std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(hwndComboBox);
		if(m_mapTitleTip.end() != itFound)
		{
			if(CBS_DROPDOWNLIST == (0x0000000f & ::GetWindowLong(hwndComboBox, GWL_STYLE)))
			{
				T *pT = static_cast<T *>(this);
				ATLTRACE(_T("DelTitleTipComboBox(TOOL(%x), NOTIFIED(%x))\n"), hwndComboBox, pT->m_hWnd);
				DelTitleTip(hwndComboBox, pT->m_hWnd);
			}

			CTitleTipComboBoxHook *pHook = static_cast<CTitleTipComboBoxHook *>(itFound->second);
			pHook->Unhook();
			pHook->Delete();
			pHook = NULL;

			m_mapTitleTip.erase(itFound);
		}
	}

	// Implementations
protected:
	LRESULT OnToolTipNotifyGetDispInfo(int nID, LPNMHDR lpnmhdr, BOOL& bHandled)
	{
		//	ATLTRACE(_T("OnToolTipNotifyGetDispInfo()\n"));
		LPNMTTDISPINFO lpnmtdi= reinterpret_cast<LPNMTTDISPINFO>(lpnmhdr);

		HWND hwndTool = reinterpret_cast<HWND>(lpnmtdi->hdr.idFrom);

		ResetToolTipBuf();

		CTitleTipHook *pHook = reinterpret_cast<CTitleTipHook *>(lpnmtdi->lParam);
		if(pHook)
		{
			switch(pHook->m_controlType)
			{
			case CTitleTipHook::EditHook:
				{
					bHandled = TRUE;
					
					// to solve blinking problem when the Combo Box control is drop-down state
					HWND hwndCapture = ::GetCapture();
					if(NULL == hwndCapture || hwndCapture == hwndTool)
						if(!OnEditToolTipNotifyGetDispInfo(nID, lpnmtdi))
							GetToolTipCtrl().PostMessage(TTM_POP, 0, 0);
				}
				break;
				
			case CTitleTipHook::ListBoxHook:
				{
					bHandled = TRUE;
					
					// to solve blinking problem when the Combo Box control is drop-down state
					HWND hwndCapture = ::GetCapture();
					if(NULL == hwndCapture || hwndCapture == hwndTool)
						if(!OnListBoxToolTipNotifyGetDispInfo(nID, lpnmtdi))
							GetToolTipCtrl().PostMessage(TTM_POP, 0, 0);
				}
				break;
				
			case CTitleTipHook::ComboBoxHook:
				{
					bHandled = TRUE;
					
					// to solve blinking problem when the Combo Box control is drop-down state
					HWND hwndCapture = ::GetCapture();
					if(NULL == hwndCapture || hwndCapture == hwndTool)
						if(!OnComboBoxToolTipNotifyGetDispInfo(nID, lpnmtdi))
							GetToolTipCtrl().PostMessage(TTM_POP, 0, 0);
				}
				break;
				
			default:
				bHandled = FALSE;
				break;
			}
		}
		else
			bHandled = FALSE;

		return 0;
	}

	LRESULT OnToolTipNotifyShow(int nID, LPNMHDR lpnmhdr, BOOL& bHandled)
	{
		//	ATLTRACE(_T("OnToolTipNotifyShow()\n"));
		LRESULT lRes = 0;

		HWND hwndTool = reinterpret_cast<HWND>(lpnmhdr->idFrom);

		std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(hwndTool);
		if(m_mapTitleTip.end() != itFound)
		{
			switch(itFound->second->m_controlType)
			{
			case CTitleTipHook::EditHook:
				{
					bHandled = TRUE;
					
					// disable multi-line tooltips
					GetToolTipCtrl().SetMaxTipWidth(-1);
					
					lRes = OnEditToolTipNotifyShow(nID, lpnmhdr, itFound->second);
					if(0 == lRes)
						GetToolTipCtrl().Pop();
				}
				break;
				
			case CTitleTipHook::ListBoxHook:
				{
					bHandled = TRUE;
					
					// disable multi-line tooltips
					GetToolTipCtrl().SetMaxTipWidth(-1);
					
					lRes = OnListBoxToolTipNotifyShow(nID, lpnmhdr, itFound->second);
					if(0 == lRes)
						GetToolTipCtrl().Pop();
				}
				break;
				
			case CTitleTipHook::ComboBoxHook:
				{
					bHandled = TRUE;
					
					// disable multi-line tooltips
					GetToolTipCtrl().SetMaxTipWidth(-1);
					
					lRes = OnComboBoxToolTipNotifyShow(nID, lpnmhdr, itFound->second);
					if(0 == lRes)
						GetToolTipCtrl().Pop();
				}
				break;
				
			default:
				bHandled = FALSE;
			}
		}
		else
			bHandled = FALSE;

		return lRes;
	}

	LRESULT OnTitleTipEditComboBox(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		uMsg;
		ATLASSERT(UWM_TITLE_TIP_EDIT_COMBOBOX == uMsg);

		LPTTCB lpttc = (LPTTCB)lParam;

		if((BOOL)wParam)
		{
			std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(lpttc->hWnd);
			if(m_mapTitleTip.end() == itFound)
			{
				CTitleTipEditHook *pHook = new CTitleTipEditHook(GetToolTipCtrl(),
					lpttc->crTipTextColor,
					lpttc->crTipBkColor);
				if(!pHook->Hook(lpttc->hWnd))
				{
					pHook->Delete();
					pHook = NULL;
				}
				
				m_mapTitleTip[lpttc->hWnd] = static_cast<CTitleTipHook *>(pHook);
				
				T *pT = static_cast<T *>(this);
				ATLTRACE(_T("AddTitleTipEditInComboBox(TOOL(%x), NOTIFIED(%x))\n"), lpttc->hWnd, pT->m_hWnd);
				// in-Edit control inside ComboBox control must use TTF_IDISHWND | TTF_SUBCLASS flag to get TitleTips support
				AddTitleTip(lpttc->hWnd, pT->m_hWnd, NULL, (LPARAM)pHook);
			}
		}
		else
		{
			std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(lpttc->hWnd);
			if(m_mapTitleTip.end() != itFound)
			{
				T *pT = static_cast<T *>(this);
				ATLTRACE(_T("DelTitleTipEditInComboBox(TOOL(%x), NOTIFIED(%x))\n"), lpttc->hWnd, pT->m_hWnd);
				DelTitleTip(lpttc->hWnd, pT->m_hWnd);
				
				CTitleTipEditHook *pHook = static_cast<CTitleTipEditHook *>(itFound->second);
				pHook->Unhook();
				pHook->Delete();
				pHook = NULL;
				
				m_mapTitleTip.erase(itFound);
			}
		}

		return 0;
	}

	LRESULT OnTitleTipListBoxComboBox(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		uMsg;
		ATLASSERT(UWM_TITLE_TIP_LISTBOX_COMBOBOX == uMsg);

		LPTTCB lpttc = (LPTTCB)lParam;

		if((BOOL)wParam)
		{
			std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(lpttc->hWnd);
			if(m_mapTitleTip.end() == itFound)
			{
				CTitleTipListBoxHook *pHook = new CTitleTipListBoxHook(GetToolTipCtrl(),
					lpttc->crTipTextColor,
					lpttc->crTipBkColor,
					lpttc->crTipTextColorSelection,
					lpttc->crTipBkColorSelection);
				if(!pHook->Hook(lpttc->hWnd))
				{
					pHook->Delete();
					pHook = NULL;
				}
				
				m_mapTitleTip[lpttc->hWnd] = static_cast<CTitleTipHook *>(pHook);
				
				T *pT = static_cast<T *>(this);
				ATLTRACE(_T("AddTitleTipListBoxInComboBox(TOOL(%x), NOTIFIED(%x))\n"), lpttc->hWnd, pT->m_hWnd);
				// in-ListBox control inside ComboBox control must use TTF_IDISHWND | TTF_SUBCLASS flag to get TitleTips support
				AddTitleTip(lpttc->hWnd, pT->m_hWnd, NULL, (LPARAM)pHook);
			}
		}
		else
		{
			std::map<HWND, CTitleTipHook *>::iterator itFound = m_mapTitleTip.find(lpttc->hWnd);
			if(m_mapTitleTip.end() != itFound)
			{
				T *pT = static_cast<T *>(this);
				ATLTRACE(_T("DelTitleTipListBoxInComboBox(TOOL(%x), NOTIFIED(%x))\n"), lpttc->hWnd, pT->m_hWnd);
				DelTitleTip(lpttc->hWnd, pT->m_hWnd);
				
				CTitleTipListBoxHook *pHook = static_cast<CTitleTipListBoxHook *>(itFound->second);
				pHook->Unhook();
				pHook->Delete();
				pHook = NULL;
				
				m_mapTitleTip.erase(itFound);
			}
		}

		return 0;
	}

	BOOL AddTitleTip(HWND hwndTool, HWND hwndNotified, LPRECT lpRect = NULL, LPARAM lUserParam = NULL)
	{
		if(!::IsWindow(hwndTool))
			return FALSE;

		if(!GetToolTipCtrl().IsWindow())
			return FALSE;
		
		UINT uFlags = TTF_SUBCLASS | TTF_TRANSPARENT; 
		if(hwndTool != hwndNotified && NULL == lpRect)
			uFlags |= TTF_IDISHWND;

		CToolInfo ti(uFlags, hwndNotified, (UINT_PTR)hwndTool, lpRect, LPSTR_TEXTCALLBACK, lUserParam);
		
		return GetToolTipCtrl().AddTool(ti);
	}

	void DelTitleTip(HWND hwndTool, HWND hwndNotified)
	{
		if(!GetToolTipCtrl().IsWindow() || !::IsWindow(hwndTool))
			return;

		if(!GetToolTipCtrl().IsWindow())
			return;

		UINT uFlags = TTF_SUBCLASS | TTF_TRANSPARENT; 
		if(hwndTool != hwndNotified)
			uFlags |= TTF_IDISHWND;

		CToolInfo ti(uFlags, hwndNotified, (UINT_PTR)hwndTool);
		GetToolTipCtrl().DelTool(ti);

		if(0 == GetToolTipCtrl().GetToolCount())
			GetToolTipCtrl().DestroyWindow();
	}
	
	BOOL OnEditToolTipNotifyGetDispInfo(int /*nID*/, LPNMTTDISPINFO lpnmtdi)
	{
		lpnmtdi->lpszText[0] = NULL;
		lpnmtdi->szText[0] = NULL;
		
		// lpnmtdi.hdr.idFrom is tool's handle
		CEdit wndTool( reinterpret_cast<HWND>(lpnmtdi->hdr.idFrom) );
		
		if(ES_MULTILINE & wndTool.GetStyle())
		{
			CTitleTipEditHook *pHook = reinterpret_cast<CTitleTipEditHook *>(lpnmtdi->lParam);
			if(NULL == pHook)
				return 0;

			POINT ptTarget;
			if(!::GetCursorPos(&ptTarget))
				return FALSE;

			if(!wndTool.ScreenToClient(&ptTarget))
				return FALSE;

			int iCharPos = wndTool.CharFromPos(ptTarget, &pHook->m_iTitleTipLine);

			int iFirstVisibleLine = wndTool.GetFirstVisibleLine();
			if(iFirstVisibleLine > pHook->m_iTitleTipLine)
				return FALSE;

			int nLen = wndTool.LineLength(iCharPos);

			if(nLen > GetToolTipBufLen())
			{
				// to reallocate memory for private storage of tool tip text only if it is necessary
				if(NULL == ReallocToolTipBuf(nLen))
					return FALSE;
			}

			nLen = wndTool.GetLine(pHook->m_iTitleTipLine, GetToolTipBuf(), nLen + 1);

			// to get edit control's formatting rectangle
			RECT rcFmt;
			wndTool.GetRect(&rcFmt);

			CClientDC dcTool(wndTool);
			CFontHandle oldFont = dcTool.SelectFont(wndTool.GetFont());
			
			SIZE szText;
			int nFit = 0;
			if(!dcTool.GetTextExtentExPoint(GetToolTipBuf(), nLen, &szText, rcFmt.right - rcFmt.left, &nFit))
			{
				dcTool.SelectFont(oldFont.Detach());
				return FALSE;
			}

			dcTool.SelectFont(oldFont.Detach());
			
			pHook->m_nVTextEnd = (wndTool.GetLineCount() - iFirstVisibleLine) * szText.cy;
			if(pHook->m_nVTextEnd < ptTarget.y)
				return FALSE;

			int nHTextBegin = wndTool.PosFromChar(wndTool.LineIndex(pHook->m_iTitleTipLine)).x;
			pHook->m_nHTextEnd = nHTextBegin + szText.cx + 1;
			
			if(ptTarget.x <= pHook->m_nHTextEnd)
			{
				if( nFit <  nLen || (nHTextBegin < rcFmt.left && rcFmt.left < pHook->m_nHTextEnd ) )
				{
					// text is clipped thus set the tool tip text to display
					lpnmtdi->lpszText = GetToolTipBuf();
				}
			}
		}
		else	// if(ES_MULTILINE & wndTool.GetStyle())
		{
			// to retrieve text to display
			int nLen = wndTool.GetWindowTextLength();
			
			if(nLen > GetToolTipBufLen())
			{
				// to reallocate memory for private storage of tool tip text only if it is necessary
				if(NULL == ReallocToolTipBuf(nLen))
					return FALSE;
			}
			
			wndTool.GetWindowText(GetToolTipBuf(), nLen + 1);
			
			// to get edit control's formatting rectangle
			RECT rcFmt;
			wndTool.GetRect(&rcFmt);
			
			CClientDC dcTool(wndTool);
			CFontHandle oldFont = dcTool.SelectFont(wndTool.GetFont());

			SIZE szText;
			int nFit = 0;
			if(!dcTool.GetTextExtentExPoint(GetToolTipBuf(), nLen, &szText, rcFmt.right - rcFmt.left, &nFit))
			{
				dcTool.SelectFont(oldFont.Detach());
				return FALSE;
			}
			
			dcTool.SelectFont(oldFont.Detach());
			
			if(nFit <  nLen)
			{
				// text is clipped thus set the tool tip text to display
				lpnmtdi->lpszText = GetToolTipBuf();
			}
		}
		
		return TRUE;
	}
	
	BOOL OnEditToolTipNotifyShow(int /*nID*/, LPNMHDR lpnmhdr, CTitleTipHook* pEditHook)
	{
		if(!GetToolTipBuf())
			return 0;
		
		CTitleTipEditHook *pHook = static_cast<CTitleTipEditHook *>(pEditHook);
		if(NULL == pHook)
			return 0;
		
		// lpnmhdr->idFrom is tool's handle
		CEdit wndTool( reinterpret_cast<HWND>(lpnmhdr->idFrom) );

		// to get edit control's formatting rectangle
		RECT rcFmt;
		wndTool.GetRect(&rcFmt);
		const int nLMargin = rcFmt.left;
		
		if(!wndTool.ClientToScreen(&rcFmt))
			return 0;
		
		if(ES_MULTILINE & wndTool.GetStyle())
		{
			CClientDC dcTool(wndTool);
			CFontHandle oldFont = dcTool.SelectFont(wndTool.GetFont());
			
			SIZE szExt;
			if(!dcTool.GetTextExtent(_T("0"), 1, &szExt))
			{
				dcTool.SelectFont(oldFont.Detach());
				return FALSE;
			}
			
			dcTool.SelectFont(oldFont.Detach());
			
			rcFmt.bottom = rcFmt.top + szExt.cy;
			const int nShiftDY = szExt.cy * (pHook->m_iTitleTipLine - wndTool.GetFirstVisibleLine());
			
			if(!::OffsetRect(&rcFmt, 0, nShiftDY))
				return 0;
		}
		
#if (_WIN32_IE >= 0x0500)
		
		// to calculate and adjust the position of the tool tip to be superimposed on the tool window
		if(!GetToolTipCtrl().AdjustRect(&rcFmt, TRUE))
			return 0;
		
#else	// #if (_WIN32_IE >= 0x0500)
		
		ATLTRACE(FALSE);
		// The problem with this simple scheme is that you can't position the ToolTip text rectangle directly.
		// Instead, you must position the ToolTip window rectangle just far enough above and to the left of
		// the label text rectangle so that the corners of the two text rectangles coincide. In other words,
		// you need to know the offset between the ToolTip window rectangle and its enclosed text rectangle.
		// In general, there is no simple way to determine this offset
		// reference:
		// http://msdn.microsoft.com/library/en-us/shellcc/platform/commctls/tooltip/usingtooltips.asp?frame=true#tooltip_sample_inplace
		
#endif	// #if (_WIN32_IE >= 0x0500)		
		
		// to calculate the horizontal offset, in pixels, of first character shown in the tool window if scrolled
		// and offset the position of the tool tip accordingly
		if(ES_MULTILINE & wndTool.GetStyle())
		{
			if(!::OffsetRect(&rcFmt, wndTool.PosFromChar(wndTool.LineIndex(pHook->m_iTitleTipLine)).x - nLMargin, 0))
				return 0;
		}
		else
		{
			int nShiftDX = 0;

#ifndef _USE_ALT_EM_POSFROM_CHAR_

			nShiftDX = wndTool.PosFromChar(0).x - nLMargin;
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Bug of Edit control ???
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// I spotted a certain case that EM_POSFROMCHAR didn't return the correct shifting offset
			// in single line Edit control (NOT ES_MULTILINE),
			// it returned some big negative random number instead. (but unfortunately I forgot how to reproduce the behavior)
			// If you happen to see such a situation (Tip is shown at the incorrect location or not seen at all),
			// you can use the following alterantive way of calculating offset by defining _USE_ALT_EM_POSFROM_CHAR_
			// And please report your situation to me so that I can count on it (mailto:zzzzoooo@mail.com)

#else	// #ifndef _USE_ALT_EM_POSFROM_CHAR_

			POINT ptFirstVisible = { nLMargin + 1, 0 };
			int iFirstVisible = wndTool.CharFromPos(ptFirstVisible, NULL);
			ptFirstVisible = wndTool.PosFromChar(iFirstVisible);

			CClientDC dcTool(wndTool);
			CFont oldFont = dcTool.SelectFont(wndTool.GetFont());
			
			SIZE szText = { 0, 0 };
			if(0 < iFirstVisible )
			{
				if(!dcTool.GetTextExtent(GetToolTipBuf(), iFirstVisible, &szText))
				{
					dcTool.SelectFont(oldFont);
					return FALSE;
				}
			}
			
			dcTool.SelectFont(oldFont.Detach());

			nShiftDX = ptFirstVisible.x - nLMargin - szText.cx;

#endif	// #ifndef _USE_ALT_EM_POSFROM_CHAR_

			if(!::OffsetRect(&rcFmt, nShiftDX, 0))
				return 0;
		}
		
		SetTitleTipColor(pHook->m_crTipTextColor, pHook->m_crTipBkColor);
		
		GetToolTipCtrl().SetFont(wndTool.GetFont());
		
		if( !GetToolTipCtrl().SetWindowPos(HWND_TOPMOST, &rcFmt, SWP_NOSIZE | SWP_NOACTIVATE) )
			return 0;

		// return TRUE to use customized tool tip position
		return TRUE;
	}

	BOOL OnListBoxToolTipNotifyGetDispInfo(int /*nID*/, LPNMTTDISPINFO lpnmtdi)
	{
		lpnmtdi->lpszText[0] = NULL;
		lpnmtdi->szText[0] = NULL;

		// lpnmtdi.hdr.idFrom is tool's handle
		CListBox wndTool( reinterpret_cast<HWND>(lpnmtdi->hdr.idFrom) );
		CTitleTipListBoxHook *pHook = reinterpret_cast<CTitleTipListBoxHook *>(lpnmtdi->lParam);
		if(NULL == pHook)
			return FALSE;
	
		POINT ptTarget = { 0, 0 };
		if(!::GetCursorPos(&ptTarget))
			return FALSE;

		if(HTCLIENT != wndTool.SendMessage(WM_NCHITTEST, 0, MAKELPARAM(ptTarget.x, ptTarget.y)))
			return FALSE;

		if(!wndTool.ScreenToClient(&ptTarget))
			return FALSE;

		BOOL bOutside = TRUE;
		pHook->m_iTitleTipItem = wndTool.ItemFromPoint(ptTarget, bOutside);
		if(bOutside)
			return FALSE;
		
		// retrieve text to display
		int nLen = wndTool.GetTextLen(pHook->m_iTitleTipItem);
		
		if(nLen > GetToolTipBufLen())
		{
			// reallocate memory for private storage of tool tip text only if it is necessary
			if(NULL == ReallocToolTipBuf(nLen))
				return FALSE;
		}
		
		nLen = wndTool.GetText(pHook->m_iTitleTipItem, GetToolTipBuf());
		if(LB_ERR == nLen)
			return FALSE;

		// get target item rectangle
		RECT rcTarget;
		if(LB_ERR == wndTool.GetItemRect(pHook->m_iTitleTipItem, &rcTarget))
			return FALSE;

		if(!::InflateRect(&rcTarget, -2/* * ::GetSystemMetrics(SM_CXBORDER)*/, 0))
			return FALSE;

		CClientDC dcTool(wndTool);
		CFontHandle oldFont = dcTool.SelectFont(wndTool.GetFont());
		
		SIZE szText;
		int nFit = 0;
		if(!dcTool.GetTextExtentExPoint(GetToolTipBuf(), nLen, &szText, rcTarget.right - rcTarget.left, &nFit))
		{
			dcTool.SelectFont(oldFont.Detach());
			return FALSE;
		}
		dcTool.SelectFont(oldFont.Detach());

		if(nFit <  nLen)
		{
			// text is clipped
			// set the tool tip text to display
			lpnmtdi->lpszText = GetToolTipBuf();
		}

		return TRUE;
	}

	BOOL OnListBoxToolTipNotifyShow(int /*nID*/, LPNMHDR lpnmhdr, CTitleTipHook *pListBoxHook)
	{
		if(!GetToolTipBuf())
			return 0;
		
		CTitleTipListBoxHook *pHook = static_cast<CTitleTipListBoxHook *>(pListBoxHook);
		if(NULL == pHook)
			return 0;

		// lpnmhdr->idFrom is tool's handle
		CListBox wndTool( reinterpret_cast<HWND>(lpnmhdr->idFrom) );

		POINT ptTarget = { 0, 0 };
		if(!::GetCursorPos(&ptTarget))
			return 0;

		if(!wndTool.ScreenToClient(&ptTarget))
			return 0;
		
		if(wndTool.GetCurSel() == pHook->m_iTitleTipItem)
			SetTitleTipColor(pHook->m_crTipTextColorSelection, pHook->m_crTipBkColorSelection);
		else
			SetTitleTipColor(pHook->m_crTipTextColor, pHook->m_crTipBkColor);
		
		// get target item rectangle
		RECT rcTarget;
		if(LB_ERR == wndTool.GetItemRect(pHook->m_iTitleTipItem, &rcTarget))
			return 0;
		
		if(!::InflateRect(&rcTarget, -2/* * ::GetSystemMetrics(SM_CXBORDER)*/, 0))
			return 0;
		
		if(!wndTool.ClientToScreen(&rcTarget))
			return 0;
		
#if (_WIN32_IE >= 0x0500)

		// calculate and adjust the position of the tool tip to be superimposed on the tool window
		if(!GetToolTipCtrl().AdjustRect(&rcTarget, TRUE))
			return 0;

#else	// #if (_WIN32_IE >= 0x0500)

		ATLTRACE(FALSE);
		// The problem with this simple scheme is that you can't position the ToolTip text rectangle directly.
		// Instead, you must position the ToolTip window rectangle just far enough above and to the left of
		// the label text rectangle so that the corners of the two text rectangles coincide. In other words,
		// you need to know the offset between the ToolTip window rectangle and its enclosed text rectangle.
		// In general, there is no simple way to determine this offset
		// reference:
		// http://msdn.microsoft.com/library/en-us/shellcc/platform/commctls/tooltip/usingtooltips.asp?frame=true#tooltip_sample_inplace

#endif	// #if (_WIN32_IE >= 0x0500)

		GetToolTipCtrl().SetFont(wndTool.GetFont());

		if( !GetToolTipCtrl().SetWindowPos(HWND_TOPMOST, &rcTarget, SWP_NOSIZE | SWP_NOACTIVATE) )
			return 0;
		
		// return TRUE to use customized tool tip position
		return TRUE;
	}

	BOOL OnComboBoxToolTipNotifyGetDispInfo(int /*nID*/, LPNMTTDISPINFO lpnmtdi)
	{
		lpnmtdi->lpszText[0] = NULL;
		lpnmtdi->szText[0] = NULL;

		// lpnmtdi.hdr.idFrom is tool's handle
		CComboBox wndTool( reinterpret_cast<HWND>(lpnmtdi->hdr.idFrom) );
	
		// retrieve text to display
		int nLen = wndTool.GetWindowTextLength();
		
		if(nLen > GetToolTipBufLen())
		{
			// to reallocate memory for private storage of tool tip text only if it is necessary
			if(NULL == ReallocToolTipBuf(nLen))
				return FALSE;
		}
		
		nLen = wndTool.GetWindowText(GetToolTipBuf(), nLen + 1);
		
		RECT rcClient;
		if(!wndTool.GetClientRect(&rcClient))
			return FALSE;

		const int cxEdge = ::GetSystemMetrics(SM_CXEDGE);
		rcClient.left += cxEdge;
		rcClient.right -= ::GetSystemMetrics(SM_CXVSCROLL) + cxEdge;

		POINT ptTarget = { 0, 0 };
		if(!::GetCursorPos(&ptTarget))
			return FALSE;

		if(!wndTool.ScreenToClient(&ptTarget))
			return FALSE;

		if(rcClient.right < ptTarget.x)
			return FALSE;

		CClientDC dcTool(wndTool);
		CFontHandle oldFont = dcTool.SelectFont(wndTool.GetFont());
		
		SIZE szText;
		int nFit = 0;
		if(!dcTool.GetTextExtentExPoint(GetToolTipBuf(), nLen, &szText, rcClient.right - rcClient.left - 2, &nFit))
		{
			dcTool.SelectFont(oldFont.Detach());
			return FALSE;
		}
		
		dcTool.SelectFont(oldFont.Detach());
		
		if(nFit <  nLen)
		{
			// text is clipped thus set the tool tip text to display
			lpnmtdi->lpszText = GetToolTipBuf();
		}

		return TRUE;
	}

	BOOL OnComboBoxToolTipNotifyShow(int /*nID*/, LPNMHDR lpnmhdr, CTitleTipHook *pComboBoxHook)
	{
		if(!GetToolTipBuf())
			return 0;
	
		CTitleTipComboBoxHook *pHook = static_cast<CTitleTipComboBoxHook *>(pComboBoxHook);
		if(NULL == pHook)
			return 0;

		// lpnmhdr->idFrom is tool's handle
		CComboBox wndTool( reinterpret_cast<HWND>(lpnmhdr->idFrom) );

		RECT rcItem;

#if(WINVER>=0x0500)

		COMBOBOXINFO cbi;
		cbi.cbSize = sizeof(COMBOBOXINFO);
		if(!::GetComboBoxInfo(wndTool, &cbi))
			return 0;

		if(!::CopyRect(&rcItem, &cbi.rcItem))
			return 0;

#else	// #if(WINVER>=0x0500)

		if(!wndTool.GetClientRect(&rcItem))
			return 0;

		if(!::InflateRect(&rcItem, -1 * (::GetSystemMetrics(SM_CXEDGE) + 1), -1 * (::GetSystemMetrics(SM_CYEDGE) + 1)))
			return 0;

		rcItem.right -= ::GetSystemMetrics(SM_CXVSCROLL);

#endif	// #if(WINVER>=0x0500)
		
		if(!::InflateRect(&rcItem, -1, -1))
			return 0;
		
		if(!wndTool.ClientToScreen(&rcItem))
			return 0;
		
#if (_WIN32_IE >= 0x0500)

		// to calculate and adjust the position of the tool tip to be superimposed on the tool window
		if(!GetToolTipCtrl().AdjustRect(&rcItem, TRUE))
			return 0;

#else	// #if (_WIN32_IE >= 0x0500)

		ATLTRACE(FALSE);
		// The problem with this simple scheme is that you can't position the ToolTip text rectangle directly.
		// Instead, you must position the ToolTip window rectangle just far enough above and to the left of
		// the label text rectangle so that the corners of the two text rectangles coincide. In other words,
		// you need to know the offset between the ToolTip window rectangle and its enclosed text rectangle.
		// In general, there is no simple way to determine this offset
		// reference:
		// http://msdn.microsoft.com/library/en-us/shellcc/platform/commctls/tooltip/usingtooltips.asp?frame=true#tooltip_sample_inplace

#endif	// #if (_WIN32_IE >= 0x0500)		

		if(::GetFocus() == wndTool)
			SetTitleTipColor(pHook->m_crTipTextColorSelection, pHook->m_crTipBkColorSelection);
		else
			SetTitleTipColor(pHook->m_crTipTextColor, pHook->m_crTipBkColor);

		GetToolTipCtrl().SetFont(wndTool.GetFont());

		if( !GetToolTipCtrl().SetWindowPos(HWND_TOPMOST, &rcItem, SWP_NOSIZE | SWP_NOACTIVATE) )
			return 0;
		
		// return TRUE to use customized tool tip position
		return TRUE;
	}
	
	void SetTitleTipColor(COLORREF crTipTextColor, COLORREF crTipBkColor)
	{
		if(!GetToolTipCtrl().IsWindow())
			return;

		GetToolTipCtrl().SetTipTextColor( crTipTextColor );
		GetToolTipCtrl().SetTipBkColor( crTipBkColor );
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CToolTipCtrl window and private storage for tool tip text
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	inline CToolTipCtrl &GetToolTipCtrl()
	{
		return CSSTip::ToolTipCtrl();
	}

	inline LPTSTR GetToolTipBuf()
	{
		return CSSTip::ToolTipBuf();
	}
	
	inline int GetToolTipBufLen() const
	{
		return CSSTip::ToolTipBufLen();
	}

	inline void ResetToolTipBuf()
	{
		CSSTip::ResetToolTipBuf();
	}
	
	inline LPTSTR ReallocToolTipBuf(int nLen)
	{
		return CSSTip::ReallocToolTipBuf(nLen);
	}
};	// template<class T> class CTitleTipHandler : public CMessageMap

class CToolTipCtrlHook : public CMessageHook<CToolTipCtrlHook>
{
public:
	// c'tor
	CToolTipCtrlHook(BOOL bAutoDelete) : CMessageHook<CToolTipCtrlHook>(bAutoDelete) { }

protected:
	// implementations
protected:
	virtual BOOL ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
	{
		BOOL bHandled = FALSE;
		
		switch(uMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
			{
				////////////////////////////////////////////////////////////////////////////////////////////////
				// added since TTF_TRANSPARENT flag doesn't work well especially in WinXP Common Control 6
				////////////////////////////////////////////////////////////////////////////////////////////////

				//	ATLTRACE(_T("CTitleTipHandler:WM_XXMOUSEDOWN\n"));

				// Get current tool info (TOOLINFO.uId is tool window's handle)
				TOOLINFO ti;
				ti.cbSize = sizeof(TOOLINFO);
				::SendMessage(GetHwnd(), TTM_GETCURRENTTOOL, 0, (LPARAM)&ti);

				// pop (hide) current tip
				::SendMessage(GetHwnd(), TTM_POP, 0, 0);

				// convert the current cursor position from tip window coordinate to tool window coordinate
				POINT ptTarget = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				::ClientToScreen(GetHwnd(), &ptTarget);
				::ScreenToClient((HWND)ti.uId, &ptTarget);

				// re-direct the mouse message to tool window
				lResult = ::SendMessage((HWND)ti.uId, uMsg, wParam, MAKELPARAM(ptTarget.x, ptTarget.y));

				bHandled = TRUE;
			}
			break;
		}
		
		return bHandled;
	}
};	// class CToolTipCtrlHook : public CTitleTipHook

#define TTDT_INITIAL_DEFAULT	500
#define TTDT_AUTOPOP_DEFAULT	5000
#define TTDT_RESHOW_DEFAULT		500

/*
 *	The SmartSingleton class
 *
 *	1) one and only one instance in a global address space of a process (OOP friendly global variable)
 *	2) lazy instantiation (intantiated only if it is used first time)
 *	3) auto delete(clean-up)
 *
 */
class CSSTip
{
	enum { INIT_BUF_SIZE = 128 };
public:
	static CToolTipCtrl &ToolTipCtrl()
	{
		if(!CSSTip::Instance().m_toolTip.IsWindow())
		{
			// must specify TTS_ALWAYSTIP otherwise in-Edit control and in-ListBox control inside ComboBox control won't have TitleTips
			CSSTip::Instance().m_toolTip.Create(NULL, NULL, NULL, TTS_NOPREFIX | TTS_ALWAYSTIP);
			
			// to support a better TTF_TRANSPARENT behavior
			CToolTipCtrlHook *pHook = new CToolTipCtrlHook(TRUE);
			pHook->Hook(CSSTip::Instance().m_toolTip.m_hWnd);
			
			/*
			UINT nDblClk = ::GetDoubleClickTime();
			CSSTip::Instance().m_toolTip.SetDelayTime(TTDT_INITIAL, nDblClk);		// the length of time the pointer must remain stationary within a tool's bounding rectangle before the ToolTip window appears
			CSSTip::Instance().m_toolTip.SetDelayTime(TTDT_AUTOPOP, nDblClk * 10);	// the length of time the tool tip window remains visible
			CSSTip::Instance().m_toolTip.SetDelayTime(TTDT_RESHOW, nDblClk / 5);	// the length of time it takes for subsequent ToolTip windows to appear as the pointer moves from one tool to another.
			*/
			CSSTip::Instance().m_toolTip.SetDelayTime(TTDT_INITIAL,	TTDT_INITIAL_DEFAULT);	// ***NOTE*** It's not a good idea to set TTDT_INITIAL value to 0 (set it to more than 10 mSec)
			CSSTip::Instance().m_toolTip.SetDelayTime(TTDT_AUTOPOP,	TTDT_AUTOPOP_DEFAULT);
			CSSTip::Instance().m_toolTip.SetDelayTime(TTDT_RESHOW,	TTDT_RESHOW_DEFAULT);
		}
		
		ATLASSERT(CSSTip::Instance().m_toolTip.IsWindow());
		
		return CSSTip::Instance().m_toolTip;
	}

	static LPTSTR ToolTipBuf()
	{
		return CSSTip::Instance().m_szToolTipBuf;
	}

	static int ToolTipBufLen()
	{
		return CSSTip::Instance().m_nToolTipBufLen;
	}

	static void ResetToolTipBuf()
	{
		if(CSSTip::Instance().m_szToolTipBuf)
		{
			CSSTip::Instance().m_szToolTipBuf[0] = NULL;
			//	::_tcsnset(CSSTip::Instance().m_szToolTipBuf, NULL, CSSTip::Instance().m_nToolTipBufLen + 1);
		}
	}

	static LPTSTR ReallocToolTipBuf(int nLen)
	{
		delete [] CSSTip::Instance().m_szToolTipBuf;
		CSSTip::Instance().m_szToolTipBuf = new TCHAR[nLen + 1];
		if(NULL == CSSTip::Instance().m_szToolTipBuf)
		{
			CSSTip::Instance().m_nToolTipBufLen = 0;
			return NULL;
		}
		
		CSSTip::Instance().m_nToolTipBufLen = nLen;
		
		return CSSTip::Instance().m_szToolTipBuf;
	}
	
protected:
	inline static CSSTip &Instance()
	{
		static CSSTip instance;
		return instance;
	}
	
public:
	// d'tor, smart clean-up
	virtual ~CSSTip()
	{
		if(m_toolTip.IsWindow())
			m_toolTip.DestroyWindow();
		
		delete [] m_szToolTipBuf;
		m_szToolTipBuf = NULL;
		m_nToolTipBufLen = 0;
	}
	
protected:
	// c'tor
	CSSTip() : m_szToolTipBuf(NULL), m_nToolTipBufLen(0)
	{
		m_szToolTipBuf = new TCHAR[INIT_BUF_SIZE + 1];
		m_szToolTipBuf[INIT_BUF_SIZE] = NULL;

		if(m_szToolTipBuf)
			m_nToolTipBufLen = INIT_BUF_SIZE;
	}

	// copy c'tor
	CSSTip(CSSTip &/*rRhs*/)
	{
	}

private:
	CToolTipCtrl m_toolTip;
	
	// private storage for tip text
	LPTSTR m_szToolTipBuf;
	int m_nToolTipBufLen;
};	// class CSSTip

}	// namespace codeproject

#endif // !defined(_TITLETIPHANDLER_H_INCLUDED_)
