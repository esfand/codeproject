// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	CAboutCtrl m_AboutCtrl;

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	CAboutDlg()
	{
		CString strCredits = "\tCAboutCtrl Example\n\n"
				"\rProgrammed by:\n"
				"Pablo van der Meer\n\n"
				"\rPorted to WTL by:\n"
				"Ma Weida <bolide@vectorstar.net>\n\n"
				"\rSpecial thanks to:\nRoy Valkenhoff\nRuud van Gilst\nPatrick ten Hove\nLodewijk Wetsteijn\nNicole de Keijzer\nand Petra\n\n"
				"\rCopyright © 2002\n\rPablo Software Solutions\n"
				"\rAll right reserved.\n\n"
				"http:\\www.pablovandermeer.nl\n";

		m_AboutCtrl.SetCredits(strCredits);
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());

		m_AboutCtrl.SubclassWindow(GetDlgItem(IDC_ABOUT_CTRL));

		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}

};
