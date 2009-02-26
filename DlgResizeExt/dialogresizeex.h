#pragma once

// Extension to WTL CDialogResize allowing persistent dialog size
template <class T>
class CDialogResizeEx : public CDialogResize<T>
{
public:	
	CSize m_size;
	HKEY m_hKeyParent;
	LPCTSTR m_lpszKeyName;

	CDialogResizeEx(void)
		: m_size(0, 0)
		, m_hKeyParent(NULL)
		, m_lpszKeyName(NULL)
	{
	};

	void DlgResize_InitEx(bool  bAddGripper      = true, 
						  bool  bUseMinTrackSize = true, 
						  DWORD dwForceStyle     = WS_CLIPCHILDREN)
	{
		DlgResize_Init(bAddGripper, bUseMinTrackSize, dwForceStyle);
		
		T* pT = static_cast<T*>(this);		
		// Size the dialog and update the control layout
		if (m_size.cx != 0 && m_size.cy != 0)
		{
			pT->SetWindowPos(NULL, 0, 0, m_size.cx, m_size.cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
			
			CRect rectClient;
			pT->GetClientRect(&rectClient);
			DlgResize_UpdateLayout(rectClient.Width(), rectClient.Height());
		}
	}
	
	// Load the dialog size from the registry.  Base the registry
	// value on the dialog ID.
	void LoadSize(HKEY hKeyParent, LPCTSTR lpszKeyName)
	{
		m_hKeyParent = hKeyParent;
		m_lpszKeyName = lpszKeyName;

		ATL::CRegKey reg;
		if (reg.Open(hKeyParent, lpszKeyName, KEY_READ) == ERROR_SUCCESS)
		{
			// Format the value name using the dialog ID
			DWORD dw;
#if (_ATL_VER >= 0x0700)
			if (reg.QueryDWORDValue(FormatWidthValueName(), dw) == ERROR_SUCCESS)
				m_size.cx = dw;
			if (reg.QueryDWORDValue(FormatHeightValueName(), dw) == ERROR_SUCCESS)
				m_size.cy = dw;
#else
			if (reg.QueryValue(dw, FormatWidthValueName()) == ERROR_SUCCESS)
				m_size.cx = dw;
			if (reg.QueryValue(dw, FormatHeightValueName()) == ERROR_SUCCESS)
				m_size.cy = dw;
#endif
		}
	}

	// Save the dialog size to the registry.
	void SaveSize(HKEY hKeyParent, LPCTSTR lpszKeyName) const
	{
		ATL::CRegKey reg;
		if (reg.Create(hKeyParent, lpszKeyName) == ERROR_SUCCESS)
		{
#if (_ATL_VER >= 0x0700)
			reg.SetDWORDValue(FormatWidthValueName(), m_size.cx);
			reg.SetDWORDValue(FormatHeightValueName(), m_size.cy);
#else
			reg.SetValue(m_size.cx, FormatWidthValueName());
			reg.SetValue(m_size.cy, FormatHeightValueName());
#endif
		}
	}

	CString FormatWidthValueName(void) const
	{
		const T* pT = static_cast<const T*>(this);
		CString strValueName;
		strValueName.Format(_T("dialog_%d_cx"), pT->IDD);
		return strValueName;
	}

	CString FormatHeightValueName(void) const
	{
		const T* pT = static_cast<const T*>(this);
		CString strValueName;
		strValueName.Format(_T("dialog_%d_cy"), pT->IDD);
		return strValueName;
	}

	BEGIN_MSG_MAP(CDialogResizeEx)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CDialogResize<T>)
	END_MSG_MAP()

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);

		// Save the window size
		CRect rect;
		pT->GetWindowRect(rect);
		m_size.cx = rect.Width();
		m_size.cy = rect.Height();

		if (m_hKeyParent != NULL && m_lpszKeyName != NULL)
			SaveSize(m_hKeyParent, m_lpszKeyName);

		bHandled = FALSE;
		return 0;
	}
};
