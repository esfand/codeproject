// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__E24F06D1_140D_461C_8247_736D6AD9A817__INCLUDED_)
#define AFX_CHILDFRM_H__E24F06D1_140D_461C_8247_736D6AD9A817__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define _USE_NEW_CHILD_MAXIMIZATION_UPDATE_
#define _USE_FOCUS_UPDATE_

class CChildFrame : public CMDIChildWindowImpl<CChildFrame>
{

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MDICHILD)

	CWTLMDIChildMaxView m_view;

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}

// Overrides
public:
#ifdef _USE_NEW_CHILD_MAXIMIZATION_UPDATE_
	// NOTE: CreateEx also calls this (through T*)
	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nMenuID = 0, LPVOID lpCreateParam = NULL)
	{
		// NOTE: hWndParent is going to become m_hWndMDIClient
		//  in CMDIChildWindowImpl::Create
		ATLASSERT(::IsWindow(hWndParent));

		BOOL bMaximized = FALSE;
		HWND hWndOld = (HWND)::SendMessage(hWndParent, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);

		if(bMaximized == TRUE)
		{
			::SendMessage(hWndParent, WM_SETREDRAW, FALSE, 0);
		}

		HWND hWnd = CMDIChildWindowImpl<CChildFrame>::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, nMenuID, lpCreateParam);

		if(bMaximized == TRUE)
		{
			::ShowWindow(hWnd, SW_SHOWMAXIMIZED);

			::SendMessage(hWndParent, WM_SETREDRAW, TRUE, 0);
			::RedrawWindow(hWndParent, NULL, NULL,
				//A little more forceful if necessary: RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
				RDW_INVALIDATE | RDW_ALLCHILDREN);
		}

		return hWnd;
	}
#endif

// Message Handling
public:

	BEGIN_MSG_MAP(CChildFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
#ifdef _USE_FOCUS_UPDATE_
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
#endif
		CHAIN_MSG_MAP(CMDIChildWindowImpl<CChildFrame>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL, WS_EX_CLIENTEDGE);

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPMSG pMsg = (LPMSG)lParam;

		if(CMDIChildWindowImpl<CChildFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

#ifdef _USE_FOCUS_UPDATE_
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_view.SetFocus();
		return 0;
	}
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__E24F06D1_140D_461C_8247_736D6AD9A817__INCLUDED_)
