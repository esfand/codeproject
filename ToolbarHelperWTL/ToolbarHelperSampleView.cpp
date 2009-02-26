// ToolbarHelperSampleView.cpp : implementation of the CToolbarHelperSampleView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "ToolbarHelperSampleView.h"

CToolbarHelperSampleView::CToolbarHelperSampleView() : m_clr(RGB(0xFF, 0x00, 0x00))
{
}

BOOL CToolbarHelperSampleView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CToolbarHelperSampleView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(&rect, m_clr);
	return 0;
}

LRESULT CToolbarHelperSampleView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return TRUE;
}