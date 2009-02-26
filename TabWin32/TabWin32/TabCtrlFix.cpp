


# include <windows.h>
# include <commctrl.h>
# include "TabCtrlFix.h"

# include <assert.h>


enum ORIENTATION
{ TOP = 0, BOTTOM, LEFT, RIGHT };



VOID DrawThemesXPTabItem( HWND hWnd, HDC dc, INT itemIndex, const RECT& rcItem, UINT flags );
INT DWordAlign(INT n) { const INT rem = n % 4; if(rem) n+=(4-rem); return n; }



VOID DrawCustomTabControl( HWND hWnd, HDC dc )
{
	// Determine orientation. Note: TCS_RIGHT and TCS_BOTTOM have the same value.
	DWORD style = GetWindowLong( hWnd, GWL_STYLE );
	ORIENTATION orientation = (style & TCS_VERTICAL) ? ((style & TCS_RIGHT) ? RIGHT : LEFT) : ((style & TCS_BOTTOM) ? BOTTOM : TOP);

	// Paint the tab body.
	RECT rcPage, rcItem, rcClient;
	GetClientRect( hWnd, &rcClient );
	rcPage = rcClient;
	TabCtrl_AdjustRect( hWnd, FALSE, &rcPage );

	switch( orientation )
	{
		case TOP:		rcClient.top = rcPage.top - 2;			break;
		case BOTTOM:	rcClient.bottom = rcPage.bottom + 3;	break;
		case LEFT:		rcClient.left = rcPage.left - 1;		break;
		case RIGHT:		rcClient.right = rcPage.right + 3;		break;
		default:	OutputDebugString( __FUNCTION__": Invalid orientation!!" ); DebugBreak();
	}
	UINT uVertBottom = orientation & 1 ? 8 : 0;		// 8 = body.
	uVertBottom |= orientation & 2 ? 16 : 0;		// 16 = vertical.
	UINT flags = 1 | uVertBottom;					// 1 = body.
	DrawThemesXPTabItem( hWnd, dc, -1, rcClient, flags );
	INT tabCount = TabCtrl_GetItemCount( hWnd );
	if( tabCount == 0 )
		return;



	// Now paint inactive tabs.
	TCHITTESTINFO hti;
	hti.flags = 0;
	GetCursorPos( &hti.pt );
	ScreenToClient( hWnd, &hti.pt );
	int iHot = TabCtrl_HitTest( hWnd, &hti );
	int iSel = TabCtrl_GetCurSel( hWnd );

	for( INT iTab = 0; iTab < tabCount; iTab++ )
	{
		if( iTab == iSel )
			continue;
		if( FALSE == TabCtrl_GetItemRect( hWnd, iTab, &rcItem ) )
		{ OutputDebugString( __FUNCTION__": TabCtrl_GetItemRect() has failed!" ); DebugBreak(); }
		if( orientation == LEFT )
			rcItem.right += 1;
		flags = uVertBottom | (iTab == iHot ? 4 : 0); // 4 = hot.
		DrawThemesXPTabItem( hWnd, dc, iTab, rcItem, flags );
	}

	// Now paint the active selected tab.
	if( FALSE == TabCtrl_GetItemRect( hWnd, iSel, &rcItem ) )
	{ OutputDebugString( __FUNCTION__": TabCtrl_GetItemRect() has failed!" ); DebugBreak(); }
	InflateRect( &rcItem, 2, 2 );
	if( orientation == TOP )
		rcItem.bottom -= 1;
	flags = uVertBottom | 2; // 2 = selected.
	DrawThemesXPTabItem( hWnd, dc, iSel, rcItem, flags );
}



VOID DrawTabItem( HWND hWnd, HDC dc, INT itemIndex, const RECT& itemRect, UINT flags )
{
	TC_ITEM item;
	char textBuffer[ 128 + 4 ];
	item.mask = TCIF_TEXT | TCIF_IMAGE;
	item.pszText = textBuffer;
	item.cchTextMax = 127;
	TabCtrl_GetItem( hWnd, itemIndex, &item );

	BOOL bSel		= flags & 2;
	BOOL bBottom	= flags & 8;

	RECT rcItem = itemRect;
	rcItem.bottom -= bSel ? 1 : 2;
	rcItem.left += bBottom ? 3 : 6;	// Text & icon.
	rcItem.top += bBottom ? 3 : 2 + bSel ? 1 : (bBottom ? 0 : 3);

	INT oldMode = SetBkMode( dc, TRANSPARENT );
	HIMAGELIST imageList = (HIMAGELIST)TabCtrl_GetImageList( hWnd );
	if( imageList && item.iImage >= 0 )
	{
		ImageList_Draw( imageList, item.iImage, dc,
			rcItem.left + bSel ? 2 : 0,
			rcItem.top + bSel ? 0 : -2, ILD_TRANSPARENT );
		rcItem.left += 19;
	}
	else
		OffsetRect( &rcItem, -2, 0 );

	INT slen = (INT)strlen( textBuffer );
	if( slen > 0 )
	{
		HFONT oldFont = (HFONT)SelectObject( dc, (HFONT)SendMessage( hWnd, WM_GETFONT, 0, 0 ) );
		rcItem.right -= 3;
		RECT r; SetRect( &r, 0, 0, rcItem.right - rcItem.left, 20 );
		// TODO: Consider using DT_WORD_ELLIPSIS instead of DT_END_ELLIPSIS.
		DrawText( dc, textBuffer, slen, &r, DT_CALCRECT | DT_SINGLELINE | DT_MODIFYSTRING | DT_END_ELLIPSIS );

		OffsetRect( &rcItem, bBottom ? -1 : 0, bSel ? 1 : -1 );
		DrawText( dc, textBuffer, slen, &rcItem, DT_NOPREFIX | DT_CENTER );
		SelectObject( dc, oldFont );
	}
	SetBkMode( dc, oldMode );
}



HRESULT DrawThemesPart( HDC dc, INT iPartID, INT iStateID, LPCSTR partNameID, LPRECT prcBox )
{
	HINSTANCE hDll = LoadLibrary( "UxTheme.dll" );
	if( !hDll ) return E_FAIL;
	BOOL (pascal* pfnIsThemeActive)();
	UINT hTheme = 0;
	(FARPROC&)pfnIsThemeActive = GetProcAddress( hDll, "IsThemeActive" );
	HRESULT hr = E_FAIL;
	if( pfnIsThemeActive && pfnIsThemeActive() )
	{
		if( strlen( partNameID) > 0 )
		{
			WCHAR swPartName[ 128 ];
			MultiByteToWideChar( CP_ACP, 0, partNameID, -1, swPartName, sizeof( swPartName ) / sizeof( WCHAR ) );
			UINT (pascal* pfnOpenThemeData)( HWND hWnd, LPCWSTR pszClassList );
			(FARPROC&)pfnOpenThemeData = GetProcAddress( hDll, "OpenThemeData" );
			if( pfnOpenThemeData && (hTheme = pfnOpenThemeData( NULL, swPartName ) ) != 0 )
			{
				UINT (PASCAL* pfnDrawThemeBackground)(UINT htheme, HDC hdc, int ipartId, int istateId, const RECT* prcBx, const RECT* prcClip );
				(FARPROC&)pfnDrawThemeBackground = GetProcAddress( hDll, "DrawThemeBackground" );
				if( pfnDrawThemeBackground )
					hr = pfnDrawThemeBackground( hTheme, dc, iPartID, iStateID, prcBox, NULL );
			}
		}
	}
	FreeLibrary( hDll );
	return hr;
}



// Draws themes tab control parts.
VOID DrawThemesXPTabItem( HWND hWnd, HDC dc, INT itemIndex, const RECT& rcItem, UINT flags )
{
	BOOL bBody		= flags & 1;
	BOOL bSel		= flags & 2;
	BOOL bHot		= flags & 4;
	BOOL bBottom	= flags & 8;
	BOOL bVertical	= flags & 16;
	BOOL bLeftTab	= !bBottom && bVertical && !bBody;

	INT width = rcItem.right - rcItem.left;
	INT height = rcItem.bottom - rcItem.top;
	if( bVertical ) // then swap width and height.
		{ UINT t = width; width = height; height = t; }

	// FIRST: Draw the background.
	HDC dcMem = CreateCompatibleDC( dc );
	HBITMAP bmpMem = CreateCompatibleBitmap( dc, width, height );
	assert( dcMem != NULL );
	assert( bmpMem != NULL );
	HBITMAP oldBmp = (HBITMAP)SelectObject( dcMem, bmpMem );
	RECT rcMem;
	SetRect( &rcMem, 0, 0, width, height );
	if( bSel )
		rcMem.bottom += 1;
	// TABP_PANE = 9, 0, "TAB"
	// TABP_TABITEM = 1, TIS_SELECTED = 3 : TIS_HOT = 2 : TIS_NORMAL = 1, "TAB"
	if( bBody )
		DrawThemesPart( dcMem, 9, 0, "TAB", &rcMem );
	else
		DrawThemesPart( dcMem, 1, bSel ? 3 : (bHot ? 2 : 1), "TAB", &rcMem );

	// SECOND: Init some extra parameters.
	BITMAPINFO bmiOut;
	BITMAPINFOHEADER& bmihOut = bmiOut.bmiHeader;
	ZeroMemory( &bmiOut, sizeof( BITMAPINFO ) );
	bmihOut.biSize = sizeof( BITMAPINFOHEADER );
	bmihOut.biCompression = BI_RGB;
	bmihOut.biPlanes = 1;
	bmihOut.biBitCount = 24;
	bmihOut.biWidth = width;
	bmihOut.biHeight = height;

	INT bmpWidthPS = DWordAlign( width * 3 );
	INT nBufferSize = ((bmpWidthPS * height)/8 + 2) * 8; // beats me!
	LPBYTE pixels = NULL;
	if( bBottom || bVertical )
	{
		pixels = new BYTE[ nBufferSize ];
		assert( pixels );
	}
	INT start = 0, lenSub = 0;
	// If bottom-oriented, flip the body contents (no shadows were flipped).
	if( bBody && bBottom && !bVertical )
		start = 3, lenSub = 4;

	// THIRD: if it is a left-oriented tab, draw the tab context before mirroring or rotating (before GetDIBits).
	if( bVertical )
	{
		if( bBody || !bBottom )
			bmihOut.biHeight = -height;
		if( !bBottom && !bBody && itemIndex >= 0 )
		{
			if( bSel )
				rcMem.bottom -= 1;
			DrawTabItem( hWnd, dcMem, itemIndex, rcMem, flags );
			itemIndex = -1;
		}
	}

	// FOURTH: Call GetBits for rotation and mirroring.
	if( bVertical || bBottom )
	{
		GetDIBits( dc, bmpMem, start, height - lenSub, pixels, &bmiOut, DIB_RGB_COLORS ); 
		if( bBottom )
		{
			bmihOut.biHeight = -height; // To mirror, just use a negative height between Get/SetDIBits.
			SetDIBits( dc, bmpMem, start, height-lenSub, pixels, &bmiOut, DIB_RGB_COLORS );
			if( bBody && bVertical )
			{
				// When it is right-oriented, flip body twice, first flip border shadows, then flip shaded inside body again.
				start = 2; lenSub = 4;
				bmihOut.biHeight = height;
				GetDIBits( dc, bmpMem, start, height - lenSub, pixels, &bmiOut, DIB_RGB_COLORS );
				bmihOut.biHeight = -height;
				SetDIBits( dc, bmpMem, start, height - lenSub, pixels, &bmiOut, DIB_RGB_COLORS );
			}
		}
	}

	// FIFTH: if it is bottom- or right-oriented, draw after mirroring background.
	if( !bBody && itemIndex >= 0 )
	{
		if( bSel )
			rcMem.bottom -= 1;
		DrawTabItem( hWnd, dcMem, itemIndex, rcMem, flags );
		if( bVertical )
		{
			bmihOut.biHeight = -height;
			GetDIBits( dc, bmpMem, start, height - lenSub, pixels, &bmiOut, DIB_RGB_COLORS );
		}
	}

	// SIXTH: rotate, if needed.
	if( bVertical )
	{
		// Swap width and height.
		INT temp = width; width = height; height = temp;

		INT bmpWidthPD = DWordAlign( width * 3 );
		INT padding = bmpWidthPD - width * 3;
		INT bufferSize = ((bmpWidthPD * height)/8 + 2)*8;
		LPBYTE rotatedPixels = new BYTE[ bufferSize ];
		assert( rotatedPixels );
		INT nWidth, nHeight = height, nHeight1 = nHeight - 1;

# define __USE_ASM__

# ifndef __USE_ASM__
		// For some reason, this is flawed, and causes an access violation error.
		LPBYTE pixelsS = pixels;		// Source pixels.
		LPBYTE pixelsD = rotatedPixels;	// Destination pixels.
		INT heightIndex = 0;
		BOOL last = FALSE;
		while( heightIndex < nHeight )
		{
			nWidth = width;
			if( heightIndex == nHeight1 )
			{
				last = TRUE;
				nWidth--;
			}
			while( nWidth-- )
			{
				*(PDWORD)pixelsD = *(PDWORD)pixelsS;
				pixelsS += bmpWidthPS; // Increment source in padded source lines.
				pixelsD += 3;			// Increment destination in RGB triplets.
			}
			if( last )
				for( INT c=3; c; c-- )
					*pixelsD++ = *pixelsS++; // Only last three bytes available, so can't read/write as a DWORD.
			else
			{
				heightIndex++;
				pixelsD += padding;
				pixelsS = pixels + heightIndex*3;	// Set source to the beginning of next vertical line.
			}
		}
# else // __USE_ASM__
		bmpWidthPS -= 4;
		nWidth = width;
		__asm
		{
				mov		esi, pixels			// source index
				mov		edi, rotatedPixels	// destination index
				xor		ebx, ebx			// vertical counter
			loop_height:
				mov		ecx, nWidth			// horizontal counter
				cmp		ebx, nHeight1		// check is it the last line
				jne		loop_width
				dec		ecx					// if it is decremnt for the last pixel

			loop_width:
				movsd						// copies 4 bytes and increments source and destination by 4 (we need only 3 bytes copied 'one pixel' RGB triplet)
				dec		edi					// adjust edi to 'as incremented by 3'
				add		esi,bmpWidthPS		// adjust esi to the next source line
				loop	loop_width			// loop one hotizontal destination line 

				cmp		ebx, nHeight1		// check is it the last line
				je		do_last				// if not last, do incrementing here
											
				inc		ebx					// increment vertical counter
				add		edi, padding		// adjust destination index by possible padding to DWORD
				mov		esi, ebx			// reset the source index: add vertical counter * 3 
				shl		esi, 1				// (is the same as * 2 +1*)
				add		esi, ebx			// +1*
				add		esi, pixels			// add to the beginning of the source
				jmp		loop_height			// loop whole height

			do_last:						// the last pixel is done by
				movsw						// moving first two bytes
				movsb						// and than by moving the very last byte
		}
# endif

		// Still in if( bVertical )
		SelectObject( dcMem, oldBmp ); // Recreate rotated bitmap.
		DeleteObject( bmpMem );
		bmpMem = CreateCompatibleBitmap( dc, width, height );
		SelectObject( dcMem, bmpMem );
		bmihOut.biWidth = width;
		bmihOut.biHeight = bBody ? -height : height;
		SetDIBits( dc, bmpMem, 0, height, rotatedPixels, &bmiOut, DIB_RGB_COLORS );
		delete [] rotatedPixels; // Original had a leak??
	}
	if( pixels )
		delete [] pixels;

	// SEVENTH, and FINALLY, blit mirrored/rotated image to the screen.
	BitBlt( dc, rcItem.left, rcItem.top, width, height, dcMem, 0, 0, SRCCOPY );
	SelectObject( dcMem, oldBmp );

	DeleteObject( bmpMem );
	DeleteDC( dcMem );
}





