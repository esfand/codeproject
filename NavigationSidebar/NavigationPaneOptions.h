// NavigationPaneOptions.h : interface of the CNavigPane class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ChecklistView.h"

using namespace WTL;

typedef std::vector< std::pair<std::wstring,bool> > buttons_t;

class CNavigPane : public CDialogImpl<CNavigPane>
	,public CCustomDraw<CNavigPane>
	,public CDialogResize<CNavigPane>
{
public:
	enum {IDD = IDD_NAVIGATIONOPTIONS};

	CButton MoveUp;
	CButton MoveDown;
	buttons_t buttons;
	buttons_t buttons_default;

	CMyCheckListCtrl m_list;

	BEGIN_MSG_MAP(CNavigPane)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnSaveCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDUP, OnMoveUp)
		COMMAND_ID_HANDLER(IDDOWN, OnMoveDown)
		COMMAND_ID_HANDLER(IDRESET, OnReset)
		NOTIFY_HANDLER(IDC_BUTTONLIST, NM_CLICK, OnSelect)
        CHAIN_MSG_MAP(CCustomDraw<CNavigPane>)
        CHAIN_MSG_MAP(CDialogResize<CNavigPane>)
        REFLECT_NOTIFICATIONS()
	END_MSG_MAP()


// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	void ResetList(bool init)
	{
		buttons_t::iterator begin, end;

		if (init)
		{
			begin = buttons.begin();
			end = buttons.end();
		}
		else
		{
			begin = buttons_default.begin();
			end = buttons_default.end();
		}

		for (int i=0; begin!= end; begin++,i++)
		{
			m_list.InsertItem(i,begin->first.c_str());
			m_list.SetCheckState(i, begin->second);
		}

		if (!init)
		{
			m_list.SelectItem(0);
			m_list.UpdateButtons(0);
			m_list.SetFocus();
		}

		return;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		m_list.SubclassWindow(GetDlgItem(IDC_BUTTONLIST));
		MoveUp = GetDlgItem(IDUP);
		MoveDown = GetDlgItem(IDDOWN);
		
		m_list.InsertColumn (0, _T("Buttons"), LVCFMT_LEFT, 100, 0 );

		ResetList(true);

		m_list.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER);

		m_list.MoveUp = MoveUp;
		m_list.MoveDown = MoveDown;

		m_list.SelectItem(0);
		m_list.UpdateButtons(0);
		m_list.SetFocus();

		return FALSE;
	}

	LRESULT OnSaveCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		buttons_t::iterator p = buttons.begin(), e = buttons.end();
		for (int i = 0; p!=e; p++,i++)
		{
			CString a;
			m_list.GetItemText(i, 0, a);
			p->first = a;
			p->second = m_list.GetCheckState(i) == TRUE;
		}

		EndDialog(wID);
		return 0;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnMoveUp(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
	{
		int i = m_list.GetSelectedIndex();
		if (i > 0 && i < int(buttons.size()))
		{
			// Swap our item data
			CString a,b;
			BOOL CheckStatea = m_list.GetCheckState(i);
			BOOL CheckStateb = m_list.GetCheckState(i-1);
			m_list.GetItemText(i, 0, a);
			m_list.GetItemText(i-1, 0, b);
			m_list.SetItemText(i, 0, b);
			m_list.SetItemText(i-1, 0, a);
			m_list.SetCheckState(i, CheckStateb);
			m_list.SetCheckState(i-1, CheckStatea);

			// Select the new item and update
			m_list.SelectItem(i-1);
			m_list.UpdateButtons(i-1);
			m_list.SetFocus();
		}

		return 0;
	}

	LRESULT OnMoveDown(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
	{
		int i = m_list.GetSelectedIndex();
		if (i >= 0 && i < int(buttons.size()-1))
		{
			// Swap our item data
			CString a,b;
			BOOL CheckStatea = m_list.GetCheckState(i);
			BOOL CheckStateb = m_list.GetCheckState(i+1);
			m_list.GetItemText(i, 0, a);
			m_list.GetItemText(i+1, 0, b);
			m_list.SetItemText(i, 0, b);
			m_list.SetItemText(i+1, 0, a);
			m_list.SetCheckState(i, CheckStateb);
			m_list.SetCheckState(i+1, CheckStatea);
			
			// Select the new item and update
			m_list.SelectItem(i+1);
			m_list.UpdateButtons(i+1);
			m_list.SetFocus();
		}
		
		return 0;
	}

	LRESULT OnReset(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{	
		m_list.DeleteAllItems();

		ResetList(false);
		return 0;
	}

	LRESULT OnSelect(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		m_list.UpdateButtons(m_list.GetSelectedIndex());

		return 0;
	}
};