// TabWin32.cpp : Defines the entry point for the application.
//

# include <windows.h>
# include <commctrl.h>
# include <stdio.h> // for sprintf()
# include "TabCtrlFix.h"

# pragma comment( lib, "comctl32.lib" )



LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	ModifiedTabCtrlWndProc( HWND, UINT, WPARAM, LPARAM );

WNDPROC originalTabCtrlWndProc = NULL;
HWND mainWindow;
HWND tabControl;



INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASSEX wcex;
	RECT cr;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "TAB32FIXTEST";
	wcex.hIconSm		= 0;

	if( !RegisterClassEx(&wcex) )
		return FALSE;

	if( !(mainWindow = CreateWindow( "TAB32FIXTEST", "Test app. Left-click to add tab, right-click to change orientation",
						WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
						400, 300, NULL, NULL, hInstance, NULL) ) )
						return FALSE;
	GetClientRect( mainWindow, &cr );

	// Create the tab control.
	// One of the quirks of this solution is that if you are using visual styles in your project,
	// you will need to enable the TCS_OWNERDRAWFIXED style flag.
	tabControl = CreateWindowEx( 0, WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE | TCS_HOTTRACK | TCS_OWNERDRAWFIXED,
		0, 0, cr.right - cr.left, cr.bottom - cr.top, mainWindow, (HMENU)1000, hInstance, NULL );
	if( !tabControl )
		return 0;

	SendMessage( tabControl, WM_SETFONT, (WPARAM)GetStockObject( DEFAULT_GUI_FONT ), (LPARAM)TRUE );

	// Subclass the tab control so that it draws itself using our custom function instead of its own.
	originalTabCtrlWndProc = (WNDPROC)GetWindowLong( tabControl, GWL_WNDPROC );
	SetWindowLong( tabControl, GWL_WNDPROC, (LONG)ModifiedTabCtrlWndProc );

	// Add a tab to the tab control.
	TCITEM item;
	item.mask = TCIF_TEXT | TCIF_IMAGE;
	item.iImage = -1;
	item.pszText = "Blah1234";
	TabCtrl_InsertItem( tabControl, 0, &item );

	// Main message loop:
	msg.message = WM_NULL;
	while( msg.message != WM_QUIT )
	{
		if( GetMessage( &msg, NULL, 0, 0 ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return (int) msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD style;

	switch (message)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		RECT cr;
		GetClientRect( hWnd, &cr );
		MoveWindow( tabControl, 0, 0, cr.right - cr.left, cr.bottom - cr.top, TRUE );
		break;


	case WM_RBUTTONDOWN:
		// Change the orientation of the tab control.
		style = GetWindowLong( tabControl, GWL_STYLE );
		if( style & TCS_VERTICAL )
		{
			style &= ~TCS_VERTICAL;
			if( style & TCS_RIGHT )
			{
				style &= ~TCS_RIGHT;
				style |= TCS_BOTTOM;
			}
		}
		else
		{
			style |= TCS_VERTICAL;
			if( style & TCS_BOTTOM )
				style &= ~TCS_BOTTOM;
			else
				style |= TCS_RIGHT;
		}
		SetWindowLong( tabControl, GWL_STYLE, style );
		//RedrawWindow( hTabControl, NULL, NULL, RDW_INVALIDATE );
		break;

	case WM_LBUTTONDOWN:
		TCITEM item;
		item.mask = TCIF_TEXT | TCIF_IMAGE;
		item.iImage = -1;
		char itemText[64];
		sprintf( itemText, "Item %d", TabCtrl_GetItemCount( tabControl ) );
		item.pszText = itemText;
		TabCtrl_InsertItem( tabControl, TabCtrl_GetItemCount( tabControl ), &item );
		TabCtrl_SetItemSize( tabControl, 300, 20 );
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


LRESULT CALLBACK ModifiedTabCtrlWndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC dc;
	if( Msg == WM_PAINT )
	{
		dc = BeginPaint( hWnd, &ps );
		if( dc )
			DrawCustomTabControl( hWnd, dc ); // See TabCtrlFix.h
		EndPaint( hWnd, &ps );
		return 0;
	}
	return ::CallWindowProc( originalTabCtrlWndProc, hWnd, Msg, wParam, lParam );
}