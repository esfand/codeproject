// Custom styles for our check list ctrl. Note that when subclassing an existing
// control, only the list extended styles (LVS_EX_*) are used, so that's why
// the first two params here are 0.

typedef CCheckListViewCtrlImplTraits<
    0, 0, LVS_EX_CHECKBOXES | LVS_EX_UNDERLINEHOT |
            LVS_EX_ONECLICKACTIVATE> CMyCheckListTraits;

typedef std::vector< std::pair<std::wstring,bool> > buttons_t;

class CMyCheckListCtrl : 
    public CCheckListViewCtrlImpl<CMyCheckListCtrl, CListViewCtrl, CMyCheckListTraits>
{
private:
    typedef CCheckListViewCtrlImpl<CMyCheckListCtrl, CListViewCtrl, CMyCheckListTraits> baseClass;
public:
	CButton MoveUp; // Remove the two CButtons if you're using
	CButton MoveDown; // this in a program that doesn't use them.

	DECLARE_WND_SUPERCLASS(_T("WTL_CheckListView"), GetWndClassName())

    BEGIN_MSG_MAP(CMyCheckListCtrl)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        CHAIN_MSG_MAP(baseClass)
    END_MSG_MAP()

	void UpdateButtons(int selected_item)
	{
		if (selected_item == 0) // top-most value
		{
			MoveUp.EnableWindow(0);
			selected_item = 0;
		}
		else
			MoveUp.EnableWindow(1);

		if (selected_item == GetItemCount()-1) // bottom-most value
		{
			MoveDown.EnableWindow(0);
			selected_item = GetItemCount()-1;
		}
		else
			MoveDown.EnableWindow(1);
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int selected_item = GetSelectedIndex();

		if (wParam == VK_UP && selected_item > 0)
			SelectItem(selected_item-1);
		else if (wParam == VK_DOWN && selected_item < GetItemCount()-1)
			SelectItem(selected_item+1);

		UpdateButtons(GetSelectedIndex());
		return 0;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT ptMsg = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		LVHITTESTINFO lvh = { 0 };
		lvh.pt = ptMsg;
		
		if(HitTest(&lvh) != -1 && (lvh.flags == LVHT_ONITEMSTATEICON) && ::GetKeyState(VK_CONTROL) >= 0)
		{
			CheckSelectedItems(lvh.iItem);
			SelectItem(lvh.iItem);
		}

		bHandled = FALSE;
		return 1;
	}
};