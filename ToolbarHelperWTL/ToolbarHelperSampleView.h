// ToolbarHelperSampleView.h : interface of the CToolbarHelperSampleView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CToolbarHelperSampleView : public CWindowImpl<CToolbarHelperSampleView>
{
public:
	DECLARE_WND_CLASS(NULL)

	COLORREF m_clr;

	CToolbarHelperSampleView();

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CToolbarHelperSampleView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
