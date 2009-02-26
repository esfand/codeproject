// wtlOutBarMDIDemoView.h : interface of the CWtlOutBarMDIDemoView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CWtlOutBarMDIDemoView : public CDialogImpl<CWtlOutBarMDIDemoView>
{
public:
	enum { IDD = IDD_WTLOUTBARMDIDEMO_FORM };

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(CWtlOutBarMDIDemoView)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
