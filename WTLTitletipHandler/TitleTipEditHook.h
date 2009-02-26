// TitleTipListBoxHook.h: interface for the CTitleTipEditHook class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TITLETIPEDITHOOK_H__AC9A471C_869D_46C9_A804_72614BE32A39__INCLUDED_)
#define AFX_TITLETIPEDITHOOK_H__AC9A471C_869D_46C9_A804_72614BE32A39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "titletiphook.h"

namespace codeproject
{

class CTitleTipEditHook : public CTitleTipHook 
{
public:
	int m_iTitleTipLine;
	int m_nHTextEnd;
	int m_nVTextEnd;
	COLORREF m_crTipTextColor;
	COLORREF m_crTipBkColor;

private:
	HWND m_hwndToolTipCtrl;

public:
	// c'tor
	CTitleTipEditHook(HWND hwndToolTipCtrl, COLORREF crTipTextColor, COLORREF crTipBkColor)
		: CTitleTipHook(), m_hwndToolTipCtrl(hwndToolTipCtrl), m_iTitleTipLine(-1), m_nHTextEnd(0), m_nVTextEnd(0)
		, m_crTipTextColor(crTipTextColor), m_crTipBkColor(crTipBkColor)
	{
		m_controlType = CTitleTipHook::EditHook;
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
			if(0 <= m_iTitleTipLine)
			{
				//	ATLTRACE(_T("TTM_POP (WM_XXBUTTONDOWN)\n"));
				m_iTitleTipLine = -1;
				::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
			}
			break;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_XBUTTONUP:
			::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
			break;

		case WM_NCHITTEST:
			bHandled = TRUE;
			lResult = DefWindowProc(uMsg, wParam, lParam);
			if(HTCLIENT != lResult)
			{
				m_iTitleTipLine = -1;
				::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
			}
//			else
//			{
//				POINT ptHit = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
//				::ScreenToClient(GetHwnd(), &ptHit);
//
//				RECT rcFmt;
//				::SendMessage(GetHwnd(), EM_GETRECT, 0, (LPARAM)&rcFmt);
//				if(!::PtInRect(&rcFmt, ptHit))
//				{
//					// TODO:
//					m_iTitleTipLine = -1;
//					::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
//				}
//			}
			break;

		case WM_MOUSEMOVE:
			if(0 <= m_iTitleTipLine)
			{
				DWORD dwRet = (DWORD)::SendMessage(GetHwnd(), EM_CHARFROMPOS, 0, lParam);
				int iCurLine = (int)(short)HIWORD(dwRet);
				
				if(iCurLine != m_iTitleTipLine)
				{
					//	ATLTRACE(_T("TTM_POP (WM_MOUSEMOVE)\n"));
					m_iTitleTipLine = -1;
					::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
				}
				else
				{
					if(m_nVTextEnd < GET_Y_LPARAM(lParam) || m_nHTextEnd < GET_X_LPARAM(lParam))
					{
						//	ATLTRACE(_T("TTM_POP (WM_MOUSEMOVE)\n"));
						m_iTitleTipLine = -1;
						::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
					}
				}
			}
			break;
			
		case WM_KEYDOWN:
			{
				TOOLINFO ti;
				::memset(&ti, 0, sizeof(TOOLINFO));
				ti.cbSize = sizeof(TOOLINFO);
				::SendMessage(m_hwndToolTipCtrl, TTM_GETCURRENTTOOL, 0, (LPARAM)&ti);
				
				if(GetHwnd() == (HWND)ti.uId)
				{
					m_iTitleTipLine = -1;
					::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
				}
			}
			break;
			
//		case WM_SYSKEYDOWN:
//			if(0 <= m_iTitleTipLine)
//			{
//				if( HIWORD(lParam) & KF_ALTDOWN )
//				{
//					switch( LOWORD(wParam) )
//					{
//					case VK_UP:
//					case VK_LEFT:
//					case VK_DOWN:
//					case VK_RIGHT:
//						{
//							TOOLINFO ti;
//							ti.cbSize = sizeof(TOOLINFO);
//							::SendMessage(m_hwndToolTipCtrl, TTM_GETCURRENTTOOL, 0, (LPARAM)&ti);
//							
//							if(GetHwnd() == (HWND)ti.uId)
//							{
//								m_iTitleTipLine = -1;
//								::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
//							}
//						}
//						break;
//					}
//				}
//			}
//			break;
		}
		
		return bHandled;
	}
};	// class CTitleTipEditHook : public CMessageHook  

}	// namespace codeproject

#endif // !defined(AFX_TITLETIPEDITHOOK_H__AC9A471C_869D_46C9_A804_72614BE32A39__INCLUDED_)
