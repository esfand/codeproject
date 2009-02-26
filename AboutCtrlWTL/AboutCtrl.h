/****************************************************************/
/*  AboutCtrl.h													*/
/*																*/
/*  Programmed by Pablo van der Meer							*/
/*  Copyright Pablo Software Solutions 2002						*/
/*  http://www.pablovandermeer.nl								*/
/*																*/
/*  Last updated: 28 june 2002									*/
/*																*/
/****************************************************************/

/****************************************************************/
/*  Ported to WTL by Ma Weida  <bolide@vectorstar.net>			*/
/*																*/
/*  Last updated: Sep 4, 2002									*/
/****************************************************************/
#ifndef __WTL_ABOUT_CTRL_H_
#define __WTL_ABOUT_CTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma once

#ifndef __cplusplus
  #error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
  #error AboutCtrl.h requires atlapp.h to be included first
#endif

#ifndef __ATLCTRLS_H__
  #error AboutCtrl.h requires atlctrls.h to be included first
#endif

#include <math.h>

// Extracted from MFC 7.0 source code by Ma Weida
BOOL AfxExtractSubString(CString& rString, LPCTSTR lpszFullString,
	int iSubString, TCHAR chSep)
{
	if (lpszFullString == NULL)
		return FALSE;

	while (iSubString--)
	{
		lpszFullString = _tcschr(lpszFullString, chSep);
		if (lpszFullString == NULL)
		{
			rString.Empty();        // return empty string as well
			return FALSE;
		}
		lpszFullString++;       // point past the separator
	}
	LPCTSTR lpchEnd = _tcschr(lpszFullString, chSep);
	int nLen = (lpchEnd == NULL) ?
		lstrlen(lpszFullString) : (int)(lpchEnd - lpszFullString);
	ATLASSERT(nLen >= 0);
	memcpy(rString.GetBufferSetLength(nLen), lpszFullString, nLen*sizeof(TCHAR));
	return TRUE;
}

// convert degrees to radians
inline double deg2rad(int angle) 
{
	// (angle / 180) * 3.14159265359
	return angle * 0.0174;
}

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CAboutCtrlImpl : public CWindowImpl< T, TBase, TWinTraits >
{
public:
	CAboutCtrlImpl()
	{
		m_hCreditsDC	= NULL;
		m_hLogoDC		= NULL;
		m_hBackgroundDC = NULL;
		m_hMemDC		= NULL;

		m_cxLogo = 0;
		m_cyLogo = 0;
		m_xPos	 = 0;
		m_yPos	 = 0; 

		m_strCredits = "";
	}

	virtual ~CAboutCtrlImpl()
	{
		// clean up
		if (m_hCreditsDC != NULL)
			DeleteDC(m_hCreditsDC);

		if (m_hLogoDC != NULL)
			DeleteDC(m_hLogoDC);
		
		if (m_hBackgroundDC != NULL)
			DeleteDC(m_hBackgroundDC);
		
		if (m_hMemDC != NULL)
			DeleteDC(m_hMemDC);
	}

	// Message map and handlers
	BEGIN_MSG_MAP(CAboutCtrlImpl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

	// Operations
	/********************************************************************/
	/*																	*/
	/* Function name : SubclassWindow									*/
	/* Description   : Subclass the specified window					*/
	/*																	*/
	/********************************************************************/
	BOOL SubclassWindow(HWND hWnd)
	{
		ATLASSERT(m_hWnd == NULL);
		ATLASSERT(::IsWindow(hWnd));

		BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
		if(bRet)
			Initialize();

		return bRet;
	}

	// Implementation

	/********************************************************************/
	/*																	*/
	/* Function name : SetCredits										*/
	/* Description   : Member function to set credits text.				*/
	/*																	*/
	/********************************************************************/
	void SetCredits(LPCTSTR lpszCredits)
	{
		m_strCredits = lpszCredits;
	}

	/********************************************************************/
	/*																	*/
	/* Function name : OnCreate											*/
	/* Description   : Message handler for WM_CREATE					*/
	/*																	*/
	/********************************************************************/
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		Initialize();
		return 0;
	}

	/********************************************************************/
	/*																	*/
	/* Function name : OnTimer											*/
	/* Description   : Update display									*/
	/*																	*/
	/********************************************************************/
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(wParam == 1)
			AnimateLogo();

		return 0;
	}

protected:
	/********************************************************************/
	/*																	*/
	/* Function name : Initialize										*/
	/* Description   : Initialize some stuff							*/
	/*																	*/
	/********************************************************************/
	void Initialize()
	{
		int cx, cy;

		// First time calling , do some init (loading pictures and create's some Hdc
		m_xAngle = 180;	// logo x angle
		m_yAngle = 60;	// logo y angle
		m_nSpeed = 6;	// spin speed
			
		m_cxData = 220;
		m_cyData = 300;

		CRect rect;
		GetClientRect(rect);

		CClientDC dc(m_hWnd);

		// create credits dc
		LoadCredits(m_hCreditsDC, m_cxData, m_cyData, dc);
		
		// Load logo and creates logo dc
		LoadPicture(IDB_LOGO, m_hLogoDC, m_cxLogo, m_cyLogo, dc);
		
		// Load Backgroundpicture and creates background DC
		LoadPicture(IDB_BACKGROUND, m_hBackgroundDC, cx, cy, dc); 
		
		// create work area
		LoadPicture(0, m_hMemDC, cx, cy, dc);

		// set scroll counter
		m_nCounter = rect.Height();

		// start animation
		SetTimer(1, 40);
	}

	/********************************************************************/
	/*																	*/
	/* Function name : AnimateLogo										*/
	/* Description   : Animate 'flying' logo							*/
	/*																	*/
	/********************************************************************/
	void AnimateLogo()
	{
		CRect rect;
		GetClientRect(rect);

		// move from left to right
		m_xPos = m_xPos + 2;
		if (m_xPos > rect.Width())
		{	
			m_xPos = -m_cxLogo / 2;
		}

		// move from top to bottom
		m_yPos = m_yPos + 1;
		if (m_yPos > rect.Height())
		{
			m_yPos = -m_cyLogo;
		}

		// copy background in memory bitmap
		BitBlt(m_hMemDC, 0, 0, rect.Width(), rect.Height(), m_hBackgroundDC, 0, 0, SRCCOPY);

		for (int i = 1; i < m_cxLogo; i++)
		{
			// copy logo with sinus effect in memory dc
			BitBlt(m_hMemDC, (int)(cos(deg2rad(m_xAngle + i)) * (m_cxLogo / 4.25) + m_xPos), 
									 (int)(sin(deg2rad(m_yAngle + i)) * 10 + 2.5 + m_yPos), 
									 1, m_cyLogo, m_hLogoDC, i, 0, SRCAND);
		}
    
		int yPos = m_cyData - m_nCounter - 100;

		// copy logo with sinus effect in memory dc
		if (m_nCounter > (rect.Height() - 30))
		{
			yPos = m_cyData - m_nCounter - 100;
		}

		// copy credits in memory dc
		BitBlt(m_hMemDC, 194, m_nCounter--, m_cxData, yPos, m_hCreditsDC, 0, 0, SRCAND);

		if (m_nCounter< -m_cyData)
		{
			m_nCounter = rect.Height();
		}

		CClientDC dc(m_hWnd);

		// and finally, copy memory bitmap to screen
		BitBlt(dc, 0, 0, rect.Width(), rect.Height(), m_hMemDC, 0, 0, SRCCOPY);

		// any calculations follows
		m_xAngle = m_xAngle + (int)(m_nSpeed * 0.5);	// rotate logo x
		m_yAngle = m_yAngle + m_nSpeed * 2;				// rotate logo y
   
		// did full rotation ?
		if (m_xAngle >= 360)
		{
			// reset angle
			m_xAngle = 0;	
		}

		// did full rotation ?
		if (m_xAngle <= -180) 
		{
			m_nSpeed = m_nSpeed * -1;
		}

		if (m_yAngle >= 360)
		{
			m_yAngle = 0;
		}
	}

	/********************************************************************/
	/*																	*/
	/* Function name : LoadPicture										*/
	/* Description   : Load picture from resource into device context	*/
	/*																	*/
	/********************************************************************/
	void LoadPicture(int nResourceID, HDC &hDestinationDC, int &nWidth, int &nHeight, HDC hDC)
	{
		HDC hMemDC;
		HDC hdcCompatible;
		HBITMAP hbmScreen;
 
		if (nResourceID != 0)
		{
			// if resourceid is given, load bitmap
			HBITMAP hPicture = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(nResourceID));
			
			BITMAP bm;
			GetObject(hPicture, sizeof (BITMAP), (LPSTR)&bm);

			hMemDC = CreateCompatibleDC(hDC);
			
			HBITMAP hOldBMP = (HBITMAP)SelectObject(hMemDC, hPicture);

			nWidth = bm.bmWidth;
			nHeight = bm.bmHeight;
			
			// Create the DC
			hdcCompatible = CreateCompatibleDC(hDC);    
			// Temporary memory bitmap
			hbmScreen = CreateCompatibleBitmap(hDC, nWidth, nHeight);

			// select bitmap into dc
			if (SelectObject(hdcCompatible, hbmScreen) == NULL)
			{
				// return null
				hDestinationDC = NULL;                                        
			}
			else 
			{
				// return the DC
				hDestinationDC = hdcCompatible;
			}
					
			if (hDestinationDC)
				BitBlt(hDestinationDC, 0, 0, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);

			SelectObject(hMemDC, hOldBMP);

			// Release temporary stuff
			DeleteDC(hMemDC);
			DeleteObject(hbmScreen);
			DeleteObject(hPicture);
		}
		else	// if no resourceid is given, create empty DC with specified width and height
		{
			// create the DC
			hdcCompatible = CreateCompatibleDC(hDC);			
			// temporary memory bitmap
			hbmScreen = CreateCompatibleBitmap(hDC, nWidth, nHeight);		
			// if the function fails
			if (SelectObject(hdcCompatible, hbmScreen) == NULL)
			{
				// return null
				hDestinationDC = NULL;
			}
			else
			{
				 // if it succeeds, return the DC
				hDestinationDC = hdcCompatible;                                     
			}
			DeleteObject(hbmScreen);
		}
	}
		
	/********************************************************************/
	/*																	*/
	/* Function name : LoadCredits										*/
	/* Description   : Create credits picture into device context		*/
	/*																	*/
	/********************************************************************/
	void LoadCredits(HDC &hDestinationDC, int nWidth, int nHeight, HDC hDC)
	{
		HDC hdcCompatible;
		HBITMAP hbmScreen;
 
		// Create the DC
		hdcCompatible = CreateCompatibleDC(hDC);			
		// Temporary bitmap
		hbmScreen = CreateCompatibleBitmap(hDC, nWidth, nHeight);		
		// if the function fails
		if (SelectObject(hdcCompatible, hbmScreen) == NULL)
		{
			// return null
			hDC = NULL;
		}
		else
		{
			 // if it succeeds, return the DC
			hDestinationDC = hdcCompatible;
			RECT rc;

			rc.top = 0;
			rc.left = 0;
			rc.bottom = nHeight;
			rc.right = nWidth;

			HFONT pOldFont;
			HFONT hFontTahoma, hFontBold, hFontNormal;
			FillRect(hDestinationDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

			// create a bunch of fonts
			hFontTahoma = CreateFont(20, 0, 0, 0, 
 									FW_BOLD, FALSE, FALSE, 0, 
   									ANSI_CHARSET,
                   		OUT_DEFAULT_PRECIS,
                   		CLIP_DEFAULT_PRECIS,
                   		PROOF_QUALITY,
                   		VARIABLE_PITCH | 0x04 | FF_DONTCARE,
                   		(LPSTR)"Arial");

			hFontBold = CreateFont(14, 0, 0, 0, 
 									FW_BOLD, FALSE, FALSE, 0, 
   									ANSI_CHARSET,
                   		OUT_DEFAULT_PRECIS,
                   		CLIP_DEFAULT_PRECIS,
                   		PROOF_QUALITY,
                   		VARIABLE_PITCH | 0x04 | FF_DONTCARE,
                   		(LPSTR)"Arial");

			hFontNormal = CreateFont(14, 0, 0, 0, 
 									FALSE, FALSE, FALSE, 0, 
   									ANSI_CHARSET,
                   		OUT_DEFAULT_PRECIS,
                   		CLIP_DEFAULT_PRECIS,
                   		PROOF_QUALITY,
                   		VARIABLE_PITCH | 0x04 | FF_DONTCARE,
                   		(LPSTR)"Arial");
			

			CString strSub;
			int nCount=0;
			// draw each line, based on specified type
			while(AfxExtractSubString(strSub, m_strCredits, nCount++, '\n'))
			{
				TCHAR nType = 0;
				COLORREF oldColor;

				if (!strSub.IsEmpty())
					nType = strSub.GetAt(0);

				switch(nType)
				{
					case '\t':	// title
						oldColor = SetTextColor(hDestinationDC, RGB(16,140,231));
						pOldFont = (HFONT)SelectObject(hDestinationDC, hFontTahoma);
						strSub.TrimLeft('\t');
						DrawText(hDestinationDC, strSub, strSub.GetLength(), &rc, DT_TOP|DT_LEFT|DT_NOPREFIX | DT_SINGLELINE);
						break;
					case '\r':	// bold
						oldColor = SetTextColor(hDestinationDC, RGB(0,0,0));
						pOldFont = (HFONT)SelectObject(hDestinationDC, hFontBold);
						strSub.TrimLeft('\r');
						DrawText(hDestinationDC, strSub, strSub.GetLength(), &rc, DT_TOP|DT_LEFT|DT_NOPREFIX | DT_SINGLELINE);
						break;
					default:	// normal
						oldColor = SetTextColor(hDestinationDC, RGB(0,0,0));
						pOldFont = (HFONT)SelectObject(hDestinationDC, hFontNormal);
						DrawText(hDestinationDC, strSub, strSub.GetLength(), &rc, DT_TOP|DT_LEFT|DT_NOPREFIX | DT_SINGLELINE);
						break;
				}
				// next line
				TEXTMETRIC tm;  
				GetTextMetrics(hDestinationDC, &tm);
				rc.top += tm.tmHeight;
				
				// set back old values
				SetTextColor(hDestinationDC, oldColor);
				SelectObject(hDestinationDC, pOldFont);
			}
			// clean up
			DeleteObject(hFontBold);
			DeleteObject(hFontNormal);
			DeleteObject(hFontTahoma);
		}
		DeleteObject(hbmScreen);
	}

// Attributes
protected:
	HDC m_hLogoDC; 
	HDC m_hBackgroundDC; 
	HDC m_hMemDC; 
	HDC m_hCreditsDC; 

	// logo dimensions
	int m_cxLogo, m_cyLogo;
	
	// logo angle
	int m_xAngle, m_yAngle;
	
	// logo 'fly' speed
	int m_nSpeed;
	
	// logo position
	int m_xPos, m_yPos; 

	int m_cxData;
	int m_cyData;

	int m_nCounter;

	CString m_strCredits;
};

class CAboutCtrl : public CAboutCtrlImpl<CAboutCtrl>
{
public:
	DECLARE_WND_CLASS(_T("WTL_AboutCtrl"))
};

#endif // __WTL_ABOUT_CTRL_