#if !defined( __DLGS_H_ )
#define __DLGS_H_

class CDlg1 : public CDialogImpl<CDlg1>
{
public:
	enum { IDD = IDD_DIALOG1 };

	BEGIN_MSG_MAP(CDlg1)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CTabCtrl aTab;
		TCITEM   aItem;

		aItem.mask = TCIF_TEXT;
		aTab.Attach(GetDlgItem(IDC_TAB1));
		aItem.pszText = _T("Tab Item 1");
		aTab.InsertItem(0, &aItem);
		aItem.pszText = _T("Tab Item 2");
		aTab.InsertItem(1, &aItem);
		aItem.pszText = _T("Tab Item 3");
		aTab.InsertItem(2, &aItem);
		aTab.Detach();

		CComboBox aComboBox;
		aComboBox.Attach(GetDlgItem(IDC_COMBO1));
		aComboBox.AddString(_T("Item 1"));
		aComboBox.AddString(_T("Item 2"));
		aComboBox.AddString(_T("Item 3"));
		aComboBox.AddString(_T("Item 4"));
		aComboBox.SetCurSel(0);
		aComboBox.Detach();
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
};

class CDlg2 : public CDialogImpl<CDlg2>
{
public:
	enum { IDD = IDD_DIALOG2 };

	BEGIN_MSG_MAP(CDlg2)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
};

class CDlg3 : public CDialogImpl<CDlg3>
{
public:
	enum { IDD = IDD_DIALOG3 };

	BEGIN_MSG_MAP(CDlg3)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
};

class CDlg4 : public CDialogImpl<CDlg4>
{
public:
	enum { IDD = IDD_DIALOG4 };

	BEGIN_MSG_MAP(CDlg4)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
};

#endif // !defined( __DLGS_H_ )
