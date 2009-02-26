// TitleTipListBoxHook.h: interface for the CTitleTipListBoxHook class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TITLETIPLISTBOXHOOK_H__AC9A471C_869D_46C9_A804_72614BE32A39__INCLUDED_)
#define AFX_TITLETIPLISTBOXHOOK_H__AC9A471C_869D_46C9_A804_72614BE32A39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "titletiphook.h"

namespace codeproject
{

class CTitleTipListBoxHook : public CTitleTipHook  
{
public:
	int m_iTitleTipItem;
	COLORREF m_crTipTextColor;
	COLORREF m_crTipBkColor;
	COLORREF m_crTipTextColorSelection;
	COLORREF m_crTipBkColorSelection;

private:
	HWND m_hwndToolTipCtrl;

public:
	// c'tor
	CTitleTipListBoxHook(HWND hwndToolTipCtrl, COLORREF crTipTextColor, COLORREF crTipBkColor, COLORREF crTipTextColorSelection, COLORREF crTipBkColorSelection)
		: CTitleTipHook(), m_hwndToolTipCtrl(hwndToolTipCtrl), m_iTitleTipItem(LB_ERR),
		m_crTipTextColor(crTipTextColor), m_crTipBkColor(crTipBkColor), m_crTipTextColorSelection(crTipTextColorSelection), m_crTipBkColorSelection(crTipBkColorSelection)
	{
		m_controlType = CTitleTipHook::ListBoxHook;
	}

	// implementations
protected:
	virtual BOOL ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
	{
		BOOL bHandled = FALSE;
		
		switch(uMsg)
		{
		case WM_NOTIFY:
			{
				LPNMHDR lpnmhdr = (LPNMHDR)lParam;
				switch(lpnmhdr->code)
				{
				case TTN_GETDISPINFO:
				case TTN_SHOW:
					lResult = ::SendMessage(::GetParent(GetHwnd()), WM_NOTIFY, (WPARAM)wParam, (LPARAM)lParam);
					bHandled = TRUE;
					break;
				}
			}
			break;
			
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_MOUSEWHEEL:
			if(LB_ERR < m_iTitleTipItem)
			{
				//	ATLTRACE(_T("TTM_POP (WM_XXBUTTONDOWN)\n"));
				m_iTitleTipItem = LB_ERR;
				::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
			}
			break;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_XBUTTONUP:
			//	ATLTRACE(_T("TTM_POP (WM_XXBUTTONUP)\n"));
			::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
			break;

		case WM_NCHITTEST:
			bHandled = TRUE;
			lResult = DefWindowProc(uMsg, wParam, lParam);
			if(HTCLIENT != lResult)
			{
				//	ATLTRACE(_T("TTM_POP (WM_NCHITTEST(%d))\n"), lResult);
				m_iTitleTipItem = LB_ERR;
				::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
			}
			break;
			
		case WM_MOUSEMOVE:
			if(LB_ERR < m_iTitleTipItem)
			{
				DWORD dwRet = (DWORD)::SendMessage(GetHwnd(), LB_ITEMFROMPOINT, 0, lParam);
				BOOL bOutside = (BOOL)(short)HIWORD(dwRet);
				int iCurItem = (int)(short)LOWORD(dwRet);

				if(bOutside || iCurItem != m_iTitleTipItem)
				{
					//	ATLTRACE(_T("TTM_POP (WM_MOUSEMOVE)\n"));
					m_iTitleTipItem = LB_ERR;
					::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
				}
			}
			break;

		case WM_KEYDOWN:
			if(LB_ERR < m_iTitleTipItem)
			{
				switch(LOWORD(wParam))
				{
				case VK_UP:
				case VK_LEFT:
				case VK_DOWN:
				case VK_RIGHT:
				case VK_PRIOR:	// PAGE_UP
				case VK_NEXT:	// PAGE_DOWN
				case VK_INSERT:
				case VK_DELETE:
				case VK_HOME:
				case VK_END:
				case VK_RETURN:
				case VK_BACK:
					{
						bHandled = TRUE;
						
						int iCurSelOld = ::SendMessage(GetHwnd(), LB_GETCURSEL, 0, 0L);
						lResult = DefWindowProc(uMsg, wParam, lParam);
						int iCurSelNew = ::SendMessage(GetHwnd(), LB_GETCURSEL, 0, 0L);
						
						if(iCurSelOld != iCurSelNew)
						{
							TOOLINFO ti;
							::memset(&ti, 0, sizeof(TOOLINFO));
							ti.cbSize = sizeof(TOOLINFO);
							::SendMessage(m_hwndToolTipCtrl, TTM_GETCURRENTTOOL, 0, (LPARAM)&ti);
							
							if(GetHwnd() == (HWND)ti.uId)
							{
								//	ATLTRACE(_T("TTM_POP (WM_KEYDOWN)\n"));
								m_iTitleTipItem = LB_ERR;
								::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
							}
						}
					}
					break;
				}
			}
			break;
			
		case WM_SYSKEYDOWN:
			if(LB_ERR < m_iTitleTipItem)
			{
				if( HIWORD(lParam) & KF_ALTDOWN )
				{
					switch( LOWORD(wParam) )
					{
					case VK_UP:
					case VK_LEFT:
					case VK_DOWN:
					case VK_RIGHT:
						{
							bHandled = TRUE;
							
							int iCurSelOld = ::SendMessage(GetHwnd(), LB_GETCURSEL, 0, 0L);
							lResult = DefWindowProc(uMsg, wParam, lParam);
							int iCurSelNew = ::SendMessage(GetHwnd(), LB_GETCURSEL, 0, 0L);
							
							if(iCurSelOld != iCurSelNew)
							{
								TOOLINFO ti;
								::memset(&ti, 0, sizeof(TOOLINFO));
								ti.cbSize = sizeof(TOOLINFO);
								::SendMessage(m_hwndToolTipCtrl, TTM_GETCURRENTTOOL, 0, (LPARAM)&ti);
								
								if(GetHwnd() == (HWND)ti.uId)
								{
									//	ATLTRACE(_T("TTM_POP (WM_SYSKEYDOWN)\n"));
									m_iTitleTipItem = LB_ERR;
									::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
								}
							}
						}
						break;
					}
				}
			}
			break;
		}
	
		return bHandled;
	}
};	// class CTitleTipListBoxHook : public CMessageHook  

}	// namespace codeproject

#endif // !defined(AFX_TITLETIPLISTBOXHOOK_H__AC9A471C_869D_46C9_A804_72614BE32A39__INCLUDED_)
