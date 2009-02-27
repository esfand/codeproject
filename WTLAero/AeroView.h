// AeroView.h : interface of the CAeroView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CAeroView : public aero::CWindowImpl<CAeroView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	void Paint(CDCHandle dc, RECT& rClient, RECT& rView, RECT& rDest)
	{
		CLogFont lf;

		if (IsTheming())
		{
			GetThemeSysFont(TMT_MSGBOXFONT, &lf);
			if (!aero::IsSupported())
				DrawThemeBackground(dc, 1, 1, &rClient, &rDest);
		}
		else
		{
			NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
			SystemParametersInfo ( SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, false );
			lf = ncm.lfMessageFont;
			dc.FillSolidRect(&rClient, GetSysColor(COLOR_WINDOW));
		}

		lf.lfHeight *= 3;
		CFont f = lf.CreateFontIndirect();
		HFONT hfOld = dc.SelectFont(f);
		DrawText(dc, L"Hello Aero", &rClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		dc.SelectFont(hfOld);
	}

	BEGIN_MSG_MAP(CAeroView)
		CHAIN_MSG_MAP(aero::CWindowImpl<CAeroView>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};

