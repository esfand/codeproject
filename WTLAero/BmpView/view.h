// View.h : interface of the CBitmapView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __VIEW_H__
#define __VIEW_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#ifndef _WIN32_WCE
class CBitmapView : 
	public aero::CWindowImpl<CBitmapView>, 
	public CScrollImpl<CBitmapView>
{
public:
	DECLARE_WND_CLASS_EX(NULL, 0, -1)
#else // _WIN32_WCE
class CBitmapView :  public CScrollWindowImpl<CBitmapView>
{
public:
	DECLARE_WND_CLASS(NULL)
#endif // _WIN32_WCE

	CBitmap m_bmp;
	SIZE m_size;


	CBitmapView()
	{
		m_size.cx = m_size.cy = 1;
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	void SetBitmap(HBITMAP hBitmap)
	{
		if(!m_bmp.IsNull())
			m_bmp.DeleteObject();

		m_bmp = hBitmap;

		if(!m_bmp.IsNull())
			m_bmp.GetSize(m_size);
		else
			m_size.cx = m_size.cy = 1;

		SetScrollOffset(0, 0, FALSE);
		SetScrollSize(m_size);
	}

#ifndef _WIN32_WCE
	void DoPaint(HDC hdc, RECT& /*rect*/)
	{
		BOOL bHandled = TRUE;

		if(!aero::IsComposing())
			OnEraseBackground(WM_ERASEBKGND, (WPARAM) hdc, NULL, bHandled);

		CScrollImpl<CBitmapView>::OnPaint(WM_PAINT, (WPARAM) hdc, NULL, bHandled);

		if(aero::IsComposing() && !m_bmp.IsNull())
		{
			RECT r = {-m_ptOffset.x, -m_ptOffset.y, m_size.cx, m_size.cy };
			m_BufferedPaint.MakeOpaque(&r);
		}
	}
	
	BEGIN_MSG_MAP(CBitmapView)
		CHAIN_MSG_MAP(aero::CWindowImpl<CBitmapView>) 
		CHAIN_MSG_MAP(CScrollImpl<CBitmapView>) 
#else
	BEGIN_MSG_MAP(CBitmapView)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		CHAIN_MSG_MAP(CScrollWindowImpl<CBitmapView>) 
#endif
	END_MSG_MAP()

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rect;
		GetClientRect(&rect);
		int x = 0;
		int y = 0;
		if(!m_bmp.IsNull())
		{
			x = m_size.cx + 1;
			y = m_size.cy + 1; 
		}
		CDCHandle dc = (HDC)wParam;
		if(rect.right > m_sizeAll.cx)
		{
			RECT rectRight = rect;
			rectRight.left = x;
			rectRight.bottom = y;
			dc.FillRect(&rectRight, COLOR_WINDOW);
		}
		if(rect.bottom > m_sizeAll.cy)
		{
			RECT rectBottom = rect;
			rectBottom.top = y;
			dc.FillRect(&rectBottom, COLOR_WINDOW);
		}
#if !defined(_WIN32_WCE) || (_WIN32_WCE >= 400)
		if(!m_bmp.IsNull())
		{
			dc.MoveTo(m_size.cx, 0);
			dc.LineTo(m_size.cx, m_size.cy);
			dc.LineTo(0, m_size.cy);
		}
#endif //!defined(_WIN32_WCE) || (_WIN32_WCE >= 400)
		return 0;
	}

	void DoPaint(CDCHandle dc)
	{
		if(!m_bmp.IsNull())
		{
			CDC dcMem;
			dcMem.CreateCompatibleDC(dc);
			HBITMAP hBmpOld = dcMem.SelectBitmap(m_bmp);
			dc.BitBlt(0, 0, m_size.cx, m_size.cy, dcMem, 0, 0, SRCCOPY);
			dcMem.SelectBitmap(hBmpOld);
		}
	}
};

#endif // __VIEW_H__
