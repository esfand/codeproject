#include "stdafx.h"
#include "wtldraw.h"
#include "xp2003colors.h"

int getScreenBitsPerPixel()
{
	HDC hdc = ::GetDC(NULL);
	int ret = ::GetDeviceCaps(hdc, BITSPIXEL);
	::ReleaseDC(NULL, hdc);
	return ret;
}

int numScreenColors()
{
	static int nColors = 0;
	if (!nColors)
	{
		// DC of the desktop
		CClientDC myDC(NULL);
		nColors = myDC.GetDeviceCaps(NUMCOLORS);
		if (nColors == -1)
		{
			nColors = 64000;
		}
	}
	return nColors;
}  


//////////////////////////////
// Draw helper functions

BOOL fillSolidRect(HDC hdc, LPRECT lprect, COLORREF clr)
{
	COLORREF clrBk = ::SetBkColor(hdc, clr);
	BOOL bSuccess = ::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, lprect, NULL, 0, NULL);
	::SetBkColor(hdc, clrBk);
	return bSuccess;
}

void draw3dRect(HDC hdc, LPRECT lprect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	CDCHandle dc = hdc;
	COLORREF clrBk = dc.GetBkColor();
	dc.Draw3dRect(lprect, clrTopLeft, clrBottomRight);
	dc.SetBkColor(clrBk);
}

// Retrieves the bounding rectangle of a polygon
BOOL getPolygonBox(LPRECT lprc, CONST POINT *lpPoints, int nCount)
{
	ATLASSERT(lprc);
	ATLASSERT(lpPoints);
	ATLASSERT(nCount > 0);
	if (!lprc || !lpPoints || (nCount < 1))
		return FALSE;
	::SetRect(lprc, lpPoints[0].x, lpPoints[0].y, lpPoints[0].x, lpPoints[0].y);
	for (int i = 1; i < nCount; i++)
	{
		if (lpPoints[i].x < lprc->left)
			lprc->left = lpPoints[i].x;
		if (lpPoints[i].y < lprc->top)
			lprc->top = lpPoints[i].y;
		if (lpPoints[i].x > lprc->right)
			lprc->right = lpPoints[i].x;
		if (lpPoints[i].y > lprc->bottom)
			lprc->bottom = lpPoints[i].y;
	}
	lprc->right++;
	lprc->bottom++;	
	return TRUE;
}	

#ifndef _ATL_NO_MSIMG

BOOL gradientFillRect(HDC hdc, LPCRECT lpRect, COLORREF clrFirst, COLORREF clrLast, 
					  bool bVertical)
{
	CDCHandle dc = hdc;

	if (clrFirst == clrLast)
	{
		COLORREF clrBk = dc.SetBkColor(clrFirst);
		BOOL bSuccess = dc.ExtTextOut(0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
		dc.SetBkColor(clrBk);
		return bSuccess;
	}

	TRIVERTEX vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = lpRect->left;
	vert [0] .y      = lpRect->top;
	//vert [0] .Red    = MAKEWORD(0, GetRValue(clrFirst));
	vert [0] .Red    = COLOR16(COLOR16(GetRValue(clrFirst))<<8);
	vert [0] .Green  = COLOR16(COLOR16(GetGValue(clrFirst))<<8);
	vert [0] .Blue   = COLOR16(COLOR16(GetBValue(clrFirst))<<8);
	vert [0] .Alpha  = 0x0000;
	vert [1] .x      = lpRect->right;
	vert [1] .y      = lpRect->bottom; 
	vert [1] .Red    = COLOR16(COLOR16(GetRValue(clrLast))<<8);
	vert [1] .Green  = COLOR16(COLOR16(GetGValue(clrLast))<<8);
	vert [1] .Blue   = COLOR16(COLOR16(GetBValue(clrLast))<<8);
	vert [1] .Alpha  = 0x0000;
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	return dc.GradientFill( vert, 2, &gRect, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
}

BOOL gradientFillPolygon(HDC hdc, LPPOINT lpPoints, int nCount, COLORREF clrFirst, 
						 COLORREF clrLast, bool bVertical)
{
	CDCHandle dc = hdc;
	BOOL bSuccess = FALSE;

	if (clrFirst == clrLast)
	{
		CPen pen = ::CreatePen(PS_SOLID,0,clrFirst);
		CBrush brush = ::CreateSolidBrush(clrFirst);
		HPEN hpenOld = dc.SelectPen(pen);
		HBRUSH hbrOld = dc.SelectBrush(brush);
		bSuccess = dc.Polygon(lpPoints, nCount);
		dc.SelectPen(hpenOld);
		dc.SelectBrush(hbrOld);
		return bSuccess;
	}

	HRGN hrgnOld = ::CreateRectRgn(0,0,1,1);
	bool bHasRegion = ::GetClipRgn(hdc, hrgnOld) == 1;
	HRGN hrgnNew = ::CreatePolygonRgn(lpPoints, nCount, ::GetPolyFillMode(hdc));
	HRGN hrgnOldAndNew =  ::CreateRectRgn(0,0,1,1);
	int nret = NULLREGION;
	if (bHasRegion)
	{
		nret = ::CombineRgn(hrgnOldAndNew, hrgnOld, hrgnNew, RGN_AND);
	}
	if (bHasRegion && (nret != NULLREGION)  && (nret != ERROR))
	{
		::SelectClipRgn(hdc, hrgnOldAndNew);
		bSuccess = TRUE;
	}
	else if (!bHasRegion)
	{
		::SelectClipRgn(hdc, hrgnNew);
		bSuccess = TRUE;
	}
	if (bSuccess)
	{
		RECT rc = {0,0,0,0};
		getPolygonBox(&rc, lpPoints, nCount);
		bSuccess = gradientFillRect(hdc, &rc, clrFirst, clrLast, bVertical);
		::SelectClipRgn(hdc, bHasRegion ? hrgnOld : NULL);
	}
	::DeleteObject(hrgnOld);
	::DeleteObject(hrgnNew);
	::DeleteObject(hrgnOldAndNew);
	return bSuccess;	
}

#endif //#ifndef _ATL_NO_MSIMG

COLORREF darkenColorXP(COLORREF color)
{
  return RGB( MulDiv(GetRValue(color),7,10),
              MulDiv(GetGValue(color),7,10),
              MulDiv(GetBValue(color)+55,7,10));
}

// Function splits a color into its RGB components and
// transforms the color using a scale 0..255
COLORREF darkenColor( long lScale, COLORREF lColor)
{ 
  long red   = MulDiv(GetRValue(lColor),(255-lScale),255);
  long green = MulDiv(GetGValue(lColor),(255-lScale),255);
  long blue  = MulDiv(GetBValue(lColor),(255-lScale),255);

  return RGB(red, green, blue); 
}

COLORREF mixedColor(COLORREF colorA,COLORREF colorB)
{
  // ( 86a + 14b ) / 100
  int red   = MulDiv(86,GetRValue(colorA),100) + MulDiv(14,GetRValue(colorB),100);
  int green = MulDiv(86,GetGValue(colorA),100) + MulDiv(14,GetGValue(colorB),100);
  int blue  = MulDiv(86,GetBValue(colorA),100) + MulDiv(14,GetBValue(colorB),100);

  return RGB( red,green,blue);
}

COLORREF midColor(COLORREF colorA,COLORREF colorB)
{ 
  // (7a + 3b)/10
  int red   = MulDiv(7,GetRValue(colorA),10) + MulDiv(3,GetRValue(colorB),10);
  int green = MulDiv(7,GetGValue(colorA),10) + MulDiv(3,GetGValue(colorB),10);
  int blue  = MulDiv(7,GetBValue(colorA),10) + MulDiv(3,GetBValue(colorB),10);

  return RGB( red, green, blue);
}

COLORREF grayColor(COLORREF crColor)
{ 
  int Gray  = (((int)GetRValue(crColor)) + GetGValue(crColor) + GetBValue(crColor))/3;

  return RGB( Gray,Gray,Gray);
}

BOOL isLightColor(COLORREF crColor)
{
  return (((int)GetRValue(crColor)) + GetGValue(crColor) + GetBValue(crColor))>(3*128);
}

// Function splits a color into its RGB components and
// transforms the color using a scale 0..255
COLORREF lightenColor( long lScale, COLORREF lColor)
{ 
  long R = MulDiv(255-GetRValue(lColor),lScale,255)+GetRValue(lColor);
  long G = MulDiv(255-GetGValue(lColor),lScale,255)+GetGValue(lColor);
  long B = MulDiv(255-GetBValue(lColor),lScale,255)+GetBValue(lColor);

  return RGB(R, G, B); 
}

COLORREF bleachColor(int Add, COLORREF color)
{
  return RGB( min (GetRValue(color)+Add, 255),
              min (GetGValue(color)+Add, 255),
              min (GetBValue(color)+Add, 255));
}

COLORREF getAlphaBlendColor(COLORREF blendColor, COLORREF pixelColor,int weighting)
{
  if(pixelColor==CLR_NONE)
  {
    return CLR_NONE;
  }
  // Algorithme for alphablending
  //dest' = ((weighting * source) + ((255-weighting) * dest)) / 256
  DWORD refR = ((weighting * GetRValue(pixelColor)) + ((255-weighting) * GetRValue(blendColor))) / 256;
  DWORD refG = ((weighting * GetGValue(pixelColor)) + ((255-weighting) * GetGValue(blendColor))) / 256;;
  DWORD refB = ((weighting * GetBValue(pixelColor)) + ((255-weighting) * GetBValue(blendColor))) / 256;;

  return RGB(refR,refG,refB);
}

COLORREF getXpHighlightColor()
{
  //if(bHighContrast)
  //{
  //  return GetSysColor(COLOR_HIGHLIGHT);
  //}

  if (numScreenColors() > 256) 
  {
    return midColor(GetSysColor(COLOR_WINDOW),GetSysColor(COLOR_HIGHLIGHT));
  }
  return GetSysColor(COLOR_WINDOW);
}


COLORREF CXP2003Colors::m_colors[eColorTextDisabled+1];

