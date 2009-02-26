// Tooltip.h : simple header of the tooltip window
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace Gdiplus;

class CToolTipWindow 
	: public COffscreenDrawRect<CToolTipWindow>
	, public CWindowImpl<CToolTipWindow>
{
public:
	std::wstring tt_window_caption;
	int x, y;

	BEGIN_MSG_MAP(CToolTipWindow)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP(COffscreenDrawRect<CToolTipWindow>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return TRUE;
	}

	void DoPaint(HDC dc,RECT& r)
	{
		Graphics graphics(dc);

		Pen border (Color(255, 0, 0, 0));
		SolidBrush fill (Color(255, 255, 255, 225));
		SolidBrush textbrush (Color(255, 0, 0, 0)); // Define our text brush.
		Font toolfont (L"Tahoma", 10);

		RectF TextRect;
		graphics.MeasureString(tt_window_caption.c_str(), tt_window_caption.size(), &toolfont, PointF(0.0f, 0.0f), &TextRect);

		SetWindowPos(HWND_TOP, x, y, int(TextRect.GetRight()), int(TextRect.GetBottom()), SWP_SHOWWINDOW|SWP_NOACTIVATE);

		graphics.FillRectangle(&fill, TextRect); // paint the tooltip.
		graphics.DrawString(tt_window_caption.c_str(), -1, &toolfont, PointF(Gdiplus::REAL(TextRect.GetLeft()), Gdiplus::REAL(TextRect.GetTop())), &textbrush); // paint the text on our tooltip.
		
		RectF BorderRect;
		BorderRect.Width = TextRect.GetRight()-2;
		BorderRect.X = TextRect.GetLeft();
		BorderRect.Y = TextRect.GetTop();
		BorderRect.Height = TextRect.GetBottom()-2;

		graphics.DrawRectangle(&border, BorderRect);

		return;
	}
};