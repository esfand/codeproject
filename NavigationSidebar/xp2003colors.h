// XP2003Colors.h :	interface for class CXP2003Colors
//				
/////////////////////////////////////////////////////////////////////////////

#ifndef __XP2003COLORS_H__
#define __XP2003COLORS_H__

#pragma once

enum
{
	eColorMenuBarLeft,
	eColorMenuBarRight,
	eColorTop,
	eColorMiddle,
	eColorBottom,
	eColorDarkLine,
	eColorGripper,
	eColorBorder,
	eColorHotTop,
	eColorHotBottom,
	eColorSelTop,
	eColorSelBottom,
	eColorChecked,
	eColorText,
	eColorTextDisabled
};

//Helper class for drawing XP2003 items
class CXP2003Colors
{
private:
	static COLORREF m_colors[eColorTextDisabled+1];

public:
	
	BOOL IsThemeActive()
	{
		CTheme theme;
		if (theme.IsThemingSupported())
			return ::IsThemeActive();
		return FALSE;
	}
	
	void GetMenuBarColors2003(COLORREF& color1, COLORREF& color2, BOOL bBackgroundColor = TRUE)
	{
		if(IsThemeActive())
		{
			COLORREF colorWindow = darkenColor(10,GetSysColor(COLOR_WINDOW));
			COLORREF colorCaption = ::GetSysColor(COLOR_ACTIVECAPTION);
			CClientDC myDC(NULL);
			COLORREF nearColor = myDC.GetNearestColor(midColor(colorWindow,colorCaption));
			// some colorscheme corrections (Andreas Schärer)
			if (nearColor == 15779244) //standartblau 
			{ //entspricht (haar-)genau office 2003
				color1 = RGB(163,194,245);
			}
			else if (nearColor == 15132390) //standartsilber
			{
				color1 = RGB(215,215,229);
			}
			else if (nearColor == 13425878) //olivgrün
			{
				color1 = RGB(218,218,170);
			}
			else
			{
				color1 = nearColor;
			}

			if (bBackgroundColor)
			{
				color2 = lightenColor(100,color1);
			}
			else
			{
				color2 = lightenColor(200,color1);
				color1 = darkenColor(20,color1);
			}
		}
		else
		{
			color1 = ::GetSysColor(COLOR_MENU); 
			color2 = ::GetSysColor(COLOR_WINDOW);
			color2 =  getAlphaBlendColor(color1,color2,220);
		}
	}

	COLORREF GetMenuBarColor2003()
	{
		COLORREF colorLeft,colorRight;
		GetMenuBarColors2003(colorLeft,colorRight);
		return colorLeft;
	}	

	void SetColors()
	{
		if(IsThemeActive())
		{
			GetMenuBarColors2003(m_colors[eColorMenuBarLeft], m_colors[eColorMenuBarRight]);
			switch(m_colors[eColorMenuBarLeft])
			{
				case RGB(163,194,245)://blue
				{
					m_colors[eColorTop] = RGB(221,236,254);
					m_colors[eColorMiddle] = RGB(196, 219,249);
					m_colors[eColorBottom] = RGB(129,169,226);
					m_colors[eColorDarkLine] = RGB(59,97,156);
					break;
				}
				case RGB(215,215,229)://silver
				{
					m_colors[eColorTop] = RGB(243,244,250);
					m_colors[eColorMiddle] = RGB(225,226,236);
					m_colors[eColorBottom] = RGB(153,151,181);
					m_colors[eColorDarkLine] = RGB(124,124,148);
					break;
				}
				case RGB(218,218,170)://olive
				{
					m_colors[eColorTop] = RGB(244,247,222);
					m_colors[eColorMiddle] = RGB(209,222,172);
					m_colors[eColorBottom] = RGB(183,198,145);
					m_colors[eColorDarkLine] = RGB(96,128,88);
					break;
				}
				default:
				{     
					m_colors[eColorTop] = lightenColor(140, m_colors[eColorMenuBarLeft]);
					m_colors[eColorMiddle] = lightenColor(115, m_colors[eColorMenuBarLeft]);
					m_colors[eColorBottom] = darkenColor(40, m_colors[eColorMenuBarLeft]);
					m_colors[eColorDarkLine] = darkenColor(110, m_colors[eColorMenuBarLeft]);
					break;
				}
			}//switch(menuColor)
			m_colors[eColorBorder] = RGB(4,2,132);
			m_colors[eColorSelTop] = RGB(242,151,107);
			m_colors[eColorSelBottom] = RGB(249,202,163);
			m_colors[eColorHotTop] = RGB(250,239,219);
			m_colors[eColorHotBottom] = RGB(255,212,151);
			m_colors[eColorChecked] = RGB(255,238,194);
		}//if(isThemeActive())
		else
		{
			GetMenuBarColors2003(m_colors[eColorMenuBarLeft], m_colors[eColorMenuBarRight], FALSE);
			m_colors[eColorTop] = m_colors[eColorMenuBarRight];
			m_colors[eColorBottom] = m_colors[eColorMenuBarLeft];
			m_colors[eColorMiddle] = getAlphaBlendColor(m_colors[eColorBottom], m_colors[eColorTop], 100);
			m_colors[eColorDarkLine] = ::GetSysColor(COLOR_3DSHADOW);//CLR_NONE
			m_colors[eColorSelTop] = getXpHighlightColor();
			m_colors[eColorSelTop] = m_colors[eColorSelBottom] = darkenColor(20, m_colors[eColorSelTop]);
			m_colors[eColorBorder] = ::GetSysColor(COLOR_HIGHLIGHT);
			m_colors[eColorHotTop] = m_colors[eColorHotBottom] = m_colors[eColorChecked]= getXpHighlightColor();
		}
		m_colors[eColorGripper] = 	darkenColor(10, ::GetSysColor(COLOR_ACTIVECAPTION));
		m_colors[eColorText] = ::GetSysColor(COLOR_WINDOWTEXT);
		m_colors[eColorTextDisabled] = ::GetSysColor(COLOR_GRAYTEXT);
	}	

	HBRUSH CreateXP2003MenuBarBrush()
	{
		COLORREF barColor;
		COLORREF barColor2;
		GetMenuBarColors2003(barColor, barColor2);
		HDC hdcDesktop = ::GetWindowDC(0);
		CDC clientDC;
		clientDC.CreateCompatibleDC(hdcDesktop); 
		RECT rect = {0,0, (::GetSystemMetrics(SM_CXFULLSCREEN)+16)&~7, 20};		
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(hdcDesktop, rect.right - rect.left, rect.bottom - rect.top);
		HBITMAP hbmOld = clientDC.SelectBitmap(bitmap);
		int nRight = rect.right;
		if(rect.right>700)
		{
			rect.right  = 700;
			gradientFillRect(clientDC, &rect, barColor, barColor2, false);
			rect.left = rect.right;
			rect.right = nRight;
			fillSolidRect(clientDC, &rect, barColor2);
		}
		else
		{
			gradientFillRect(clientDC, &rect, barColor, barColor2, false);
		}
		clientDC.SelectBitmap(hbmOld);

		// Release the desktopdc
		::ReleaseDC(0,hdcDesktop);

		return ::CreatePatternBrush(bitmap);
	}

	COLORREF GetColor(int eColor)
	{
		return m_colors[eColor];
	}

};


#endif // __XP2003COLORS_H__