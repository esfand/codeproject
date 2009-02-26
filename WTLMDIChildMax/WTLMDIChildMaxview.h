// WTLMDIChildMaxView.h : interface of the CWTLMDIChildMaxView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WTLMDIChildMaxVIEW_H__43A5CC17_06F8_43C3_BF3C_01E005AB2141__INCLUDED_)
#define AFX_WTLMDIChildMaxVIEW_H__43A5CC17_06F8_43C3_BF3C_01E005AB2141__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CWTLMDIChildMaxView : public CWindowImpl<CWTLMDIChildMaxView, CEdit>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CEdit::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CWTLMDIChildMaxView)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WTLMDIChildMaxVIEW_H__43A5CC17_06F8_43C3_BF3C_01E005AB2141__INCLUDED_)
