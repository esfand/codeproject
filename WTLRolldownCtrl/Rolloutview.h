// RolloutView.h : interface of the CRolloutView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROLLOUTVIEW_H__D22F5972_EC0E_4E81_B8B1_B284402C000B__INCLUDED_)
#define AFX_ROLLOUTVIEW_H__D22F5972_EC0E_4E81_B8B1_B284402C000B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CRolloutView : public CWindowImpl<CRolloutView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CRolloutView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);

		//TODO: Add your drawing code here

		return 0;
	}
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ROLLOUTVIEW_H__D22F5972_EC0E_4E81_B8B1_B284402C000B__INCLUDED_)
