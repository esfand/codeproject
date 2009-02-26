// Sample_Window.h : simple header of the sample window for views
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "XP2003Colors.h"

class CSampleWindow 
	: public CWindowImpl<CSampleWindow>
	, public COffscreenDrawRect<CSampleWindow>
{
public:
	std::wstring window_caption;
	
	COLORREF Grad1;
	CXP2003Colors m_colors;
	bool is_horizontal;
	int pop_back_amount;

	BEGIN_MSG_MAP(CSampleWindow)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(COffscreenDrawRect<CSampleWindow>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return TRUE;
	}

	//LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	void DoPaint(HDC dc,RECT& r)
	{
		RECT window;
		GetClientRect(&window);
		Graphics graphics(dc);

		SolidBrush fill (Color(255, 255, 255, 255));
		SolidBrush textbrush (Color(255, 0, 0, 0)); // Define our text brush.
		Font toolfont (L"Tahoma", 10);

		RectF window_rect;
		window_rect.X = REAL(window.left);
		window_rect.Y = REAL(window.top);
		window_rect.Width = REAL(window.right);
		window_rect.Height = REAL(window.bottom);

		graphics.FillRectangle(&fill, window_rect);
		graphics.DrawString(window_caption.c_str(), -1, &toolfont, PointF(Gdiplus::REAL(window.left), Gdiplus::REAL(window.top)), &textbrush); // paint the text on our tooltip.
		if (is_horizontal)
			graphics.DrawString(L"-The buttons will pop back at this point-", -1, &toolfont, PointF(Gdiplus::REAL(window.left), Gdiplus::REAL(window.top) + pop_back_amount - 15), &textbrush); // paint the text on our tooltip.

		m_colors.SetColors();
		COLORREF bordercolor = m_colors.GetMenuBarColor2003();
		
		Pen border (Color(255, GetRValue(bordercolor)-80, GetGValue(bordercolor)-80, GetBValue(bordercolor)-80)); // Define our border to surround our commandbar with.

		RectF window_border = window_rect;
		window_border.Width -= 1;
		window_border.Height -= 1;

		graphics.DrawRectangle(&border, window_border);
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT window;
		GetClientRect(&window);

		return 0;
	}
};