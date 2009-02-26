// WtlDraw.h
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __WTLDRAW_H__
#define __WTLDRAW_H__

#pragma once

int getScreenBitsPerPixel();

int numScreenColors();



//////////////////////////////
// Draw helper functions

BOOL fillSolidRect(HDC hdc, LPRECT lprect, COLORREF clr);

void draw3dRect(HDC hdc, LPRECT lprect, COLORREF clrTopLeft, COLORREF clrBottomRight);

// Retrieves the bounding rectangle of a polygon
BOOL getPolygonBox(LPRECT lprc, CONST POINT *lpPoints, int nCount);


BOOL gradientFillRect(HDC hdc, LPCRECT lpRect, COLORREF clrFirst, COLORREF clrLast, 
					  bool bVertical);

BOOL gradientFillPolygon(HDC hdc, LPPOINT lpPoints, int nCount, COLORREF clrFirst, 
						 COLORREF clrLast, bool bVertical = true);

COLORREF darkenColorXP(COLORREF color);

// Function splits a color into its RGB components and
// transforms the color using a scale 0..255
COLORREF darkenColor( long lScale, COLORREF lColor);

COLORREF mixedColor(COLORREF colorA,COLORREF colorB);


COLORREF midColor(COLORREF colorA,COLORREF colorB);

COLORREF grayColor(COLORREF crColor);

BOOL isLightColor(COLORREF crColor);

// Function splits a color into its RGB components and
// transforms the color using a scale 0..255
COLORREF lightenColor( long lScale, COLORREF lColor);

COLORREF bleachColor(int Add, COLORREF color);

COLORREF getAlphaBlendColor(COLORREF blendColor, COLORREF pixelColor,int weighting);


COLORREF getXpHighlightColor();


#endif // __WTLDRAW_H__

