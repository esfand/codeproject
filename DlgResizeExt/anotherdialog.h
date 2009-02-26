#pragma once

#include "dialogresizeex.h"

class CAnotherDialog :
	public CDialogImpl<CAnotherDialog>,
	public CDialogResizeEx<CAnotherDialog>
{
public:
	enum { IDD = IDD_DIALOG2 };

	BEGIN_DLGRESIZE_MAP(CAnotherDialog)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_EDIT1, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT2, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LIST1, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CAnotherDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnClose)
		COMMAND_ID_HANDLER(IDCANCEL, OnClose)
		CHAIN_MSG_MAP(CDialogResizeEx<CAnotherDialog>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_InitEx();
		CenterWindow();
		return TRUE;
	}

	LRESULT OnClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
};
