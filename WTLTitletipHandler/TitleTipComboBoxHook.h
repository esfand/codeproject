// TitleTipComboBoxHook.h: interface for the CTitleTipComboBoxHook class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TITLETIPCOMBOBOXHOOK_H__AC9A471C_869D_46C9_A804_72614BE32A39__INCLUDED_)
#define AFX_TITLETIPCOMBOBOXHOOK_H__AC9A471C_869D_46C9_A804_72614BE32A39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "titletiphook.h"

namespace codeproject
{

/***************************************************************
*                           UWM_TITLE_TIP_EDIT_COMBOBOX
* Inputs:
*       WPARAM: (WPARAM)(BOOL)bAdd
*       LPARAM: (LPARAM)(LPTTCB)lpttc
* Result: LRESULT
*	Logically void, 0, always
* Effect:
*       request AddTitleTipEdit() or DelTitleTipEdit() for
*		the specified edit control inside combo box control
***************************************************************/
#define UWM_TITLE_TIP_EDIT_COMBOBOX_MSG _T("UWM_TITLE_TIP_EDIT_COMBOBOX_MSG-{3B470660-8CB1-478b-A1A8-41CBC29A3754}")
static const UINT UWM_TITLE_TIP_EDIT_COMBOBOX = ::RegisterWindowMessage(UWM_TITLE_TIP_EDIT_COMBOBOX_MSG);

/***************************************************************
*                           UWM_TITLE_TIP_LISTBOX_COMBOBOX
* Inputs:
*       WPARAM: (WPARAM)(BOOL)bAdd
*       LPARAM: (LPARAM)(LPTTCB)lpttc
* Result: LRESULT
*	Logically void, 0, always
* Effect:
*       request AddTitleTipListBox() or DelTitleTipListBox() for
*		the specified list box control inside combo box control
***************************************************************/
#define UWM_TITLE_TIP_LISTBOX_COMBOBOX_MSG _T("UWM_TITLE_TIP_LISTBOX_COMBOBOX_MSG-{3B470660-8CB1-478b-A1A8-41CBC29A3754}")
static const UINT UWM_TITLE_TIP_LISTBOX_COMBOBOX = ::RegisterWindowMessage(UWM_TITLE_TIP_LISTBOX_COMBOBOX_MSG);

typedef struct tagTTCB
{
	HWND hWnd;
	COLORREF crTipTextColor;
	COLORREF crTipBkColor;
	COLORREF crTipTextColorSelection;
	COLORREF crTipBkColorSelection;
} TTCB, *LPTTCB;

class CTitleTipComboBoxHook : public CTitleTipHook
{
public:
	COLORREF m_crTipTextColor;
	COLORREF m_crTipBkColor;
	COLORREF m_crTipTextColorSelection;
	COLORREF m_crTipBkColorSelection;

private:
	HWND m_hwndToolTipCtrl;	
	
	HWND m_hwndEdit;
	HWND m_hwndListBox;

public:
	// c'tor
	CTitleTipComboBoxHook(HWND hwndToolTipCtrl, COLORREF crTipTextColor, COLORREF crTipBkColor, COLORREF crTipTextColorSelection, COLORREF crTipBkColorSelection)
		: CTitleTipHook(), m_hwndToolTipCtrl(hwndToolTipCtrl), m_hwndEdit(NULL), m_hwndListBox(NULL),
		m_crTipTextColor(crTipTextColor), m_crTipBkColor(crTipBkColor), m_crTipTextColorSelection(crTipTextColorSelection), m_crTipBkColorSelection(crTipBkColorSelection)
	{
		m_controlType = CTitleTipHook::ComboBoxHook;
	}

	// overrides
public:
	HWND Unhook()
	{
		UnsubclassEditListBox();

		return CMessageHook<CTitleTipHook>::Unhook();
	}

	// implementations
protected:
	void UnsubclassEditListBox()
	{
		if(NULL != m_hwndEdit)
		{
			TTCB ttc;
			::memset(&ttc, 0, sizeof(TTCB));
			ttc.hWnd = m_hwndEdit;
			::SendMessage(::GetParent(GetHwnd()), UWM_TITLE_TIP_EDIT_COMBOBOX, (WPARAM)FALSE, (LPARAM)&ttc);
			m_hwndEdit = NULL;
		}
		if(NULL != m_hwndListBox)
		{
			TTCB ttc;
			::memset(&ttc, 0, sizeof(TTCB));
			ttc.hWnd = m_hwndListBox;
			::SendMessage(::GetParent(GetHwnd()), UWM_TITLE_TIP_LISTBOX_COMBOBOX, (WPARAM)FALSE, (LPARAM)&ttc);
			m_hwndListBox = NULL;
		}
	}

	virtual BOOL ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
	{
		BOOL bHandled = FALSE;
		
		switch(uMsg)
		{
		case WM_NCHITTEST:
			bHandled = TRUE;
			if(HTCLIENT == (lResult = DefWindowProc(uMsg, wParam, lParam)))
			{
				RECT rcButton;

#if(WINVER>=0x0500)
				
				COMBOBOXINFO cbi;
				cbi.cbSize = sizeof(COMBOBOXINFO);
				::GetComboBoxInfo(GetHwnd(), &cbi);
				
				::CopyRect(&rcButton, &cbi.rcButton);
				
#else	// #if(WINVER>=0x0500)
				
				::GetClientRect(GetHwnd(), &rcButton);
//				::InflateRect(&rcButton, -1 * ::GetSystemMetrics(SM_CXEDGE), -1 * ::GetSystemMetrics(SM_CYEDGE));
//				rcButton.left = rcButton.right - ::GetSystemMetrics(SM_CXVSCROLL);
				rcButton.left = rcButton.right - ::GetSystemMetrics(SM_CXVSCROLL) - ::GetSystemMetrics(SM_CXEDGE);
				
#endif	// #if(WINVER>=0x0500)

				POINT ptHit = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				::ScreenToClient(GetHwnd(), &ptHit);

				if(ptHit.x >= rcButton.left)
				{
					//	ATLTRACE(_T("TTM_POP (WM_NCHITTEXT:COMBOBOX:BUTTON)\n"));
					::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
					return HTNOWHERE;
				}
			}
			break;

		case WM_CTLCOLOREDIT:
			if(NULL == m_hwndEdit)
			{
				// How To Subclass CListBox and CEdit Inside of CComboBox
				// http://support.microsoft.com/default.aspx?scid=kb;EN-US;174667
				//	ATLTRACE(_T("WM_CTLCOLOREDIT\n"));
				TTCB ttc;
				::memset(&ttc, 0, sizeof(TTCB));
				ttc.hWnd = (HWND)lParam;
				ttc.crTipTextColor = m_crTipTextColor;
				ttc.crTipBkColor = m_crTipBkColor;
				::SendMessage(::GetParent(GetHwnd()), UWM_TITLE_TIP_EDIT_COMBOBOX, (WPARAM)TRUE, (LPARAM)&ttc);
			}
			break;

		case WM_CTLCOLORLISTBOX:
			if(NULL == m_hwndListBox)
			{
				// How To Subclass CListBox and CEdit Inside of CComboBox
				// http://support.microsoft.com/default.aspx?scid=kb;EN-US;174667
				//	ATLTRACE(_T("WM_CTLCOLORLISTBOX\n"));
				TTCB ttc;
				::memset(&ttc, 0, sizeof(TTCB));
				ttc.hWnd = (HWND)lParam;
				ttc.crTipTextColor = m_crTipTextColor;
				ttc.crTipBkColor = m_crTipBkColor;
				ttc.crTipTextColorSelection = m_crTipTextColorSelection;
				ttc.crTipBkColorSelection = m_crTipBkColorSelection;
				::SendMessage(::GetParent(GetHwnd()), UWM_TITLE_TIP_LISTBOX_COMBOBOX, (WPARAM)TRUE, (LPARAM)&ttc);
			}
			break;

		case WM_DESTROY:
			if(NULL != m_hwndEdit)
			{
				TTCB ttc;
				::memset(&ttc, 0, sizeof(TTCB));
				ttc.hWnd = m_hwndEdit;
				::SendMessage(::GetParent(GetHwnd()), UWM_TITLE_TIP_EDIT_COMBOBOX, (WPARAM)FALSE, (LPARAM)&ttc);
				m_hwndEdit = NULL;
			}

			if(NULL != m_hwndListBox)
			{
				TTCB ttc;
				::memset(&ttc, 0, sizeof(TTCB));
				ttc.hWnd = m_hwndListBox;
				::SendMessage(::GetParent(GetHwnd()), UWM_TITLE_TIP_LISTBOX_COMBOBOX, (WPARAM)FALSE, (LPARAM)&ttc);
				m_hwndListBox = NULL;
			}
			break;

		case WM_KEYDOWN:
			switch(LOWORD(wParam))
			{
			case VK_UP:
			case VK_LEFT:
			case VK_DOWN:
			case VK_RIGHT:
			case VK_PRIOR:	// PAGE_UP
			case VK_NEXT:	// PAGE_DOWN
			case VK_HOME:
			case VK_END:
				{
					bHandled = TRUE;
					
					int iCurSelOld = ::SendMessage(GetHwnd(), CB_GETCURSEL, 0, 0L);
					lResult = DefWindowProc(uMsg, wParam, lParam);
					int iCurSelNew = ::SendMessage(GetHwnd(), CB_GETCURSEL, 0, 0L);
					
					if(iCurSelOld != iCurSelNew)
					{
						TOOLINFO ti;
						::memset(&ti, 0, sizeof(TOOLINFO));
						ti.cbSize = sizeof(TOOLINFO);
						::SendMessage(m_hwndToolTipCtrl, TTM_GETCURRENTTOOL, 0, (LPARAM)&ti);
						
						if(GetHwnd() == (HWND)ti.uId)
							::SendMessage(m_hwndToolTipCtrl, TTM_POP, 0, 0);
					}
				}
				break;
			}
			break;
		}
	
		return bHandled;
	}
};	// class CTitleTipComboBoxHook : public CMessageHook  

}	// namespace codeproject

#endif // !defined(AFX_TITLETIPCOMBOBOXHOOK_H__AC9A471C_869D_46C9_A804_72614BE32A39__INCLUDED_)
