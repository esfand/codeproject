#pragma once
#include "XP2003Colors.h"

using namespace Gdiplus;

struct CHorSnapSplitterWindow : 
	public CWindowImpl<CHorSnapSplitterWindow>,
	public CSplitterImpl< CHorSnapSplitterWindow, false >
{
public:
	int m_min_y;
	int old_max;
	int m_buttonlimit;
	int m_desired;
	int button_height;
	int pop_back_y;
	int inactivated_buttons;

	COLORREF Grad1;
	COLORREF Grad2;
	CXP2003Colors m_colors;

	enum { t_bVertical = 0 };

	typedef CSplitterImpl<CHorSnapSplitterWindow, false> baseClass;
	typedef CHorSnapSplitterWindow T;

	CHorSnapSplitterWindow() : m_nSnapSize(0),m_min_y(100),m_buttonlimit(0),m_desired(0),pop_back_y(0)
	{
	}
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		OnMouseMoveEx(0,wParam,lParam,bHandled);

		RECT rect; // Horizontal Splitter dimensions
		GetClientRect(&rect);
		m_desired = (rect.bottom - GetSplitterPos() - 4) / button_height;

		return 0;
	}

	bool SetSplitterPos(int xyPos = -1, bool bUpdate = true)
	{
		if(xyPos == -1)   // -1 == middle
			return true; // just return a value so it doesn't set to middle
		else
			return baseClass::SetSplitterPos(xyPos,bUpdate);
	}

	void DrawSplitterBar(CDCHandle dc)
	{
		Graphics graphics(dc);

		RECT rect;
		if(GetSplitterBarRect(&rect))
		{
			RectF splitterrect;
			splitterrect.X = REAL(rect.left);
			splitterrect.Y = REAL(rect.top);
			splitterrect.Width = REAL(rect.right);
			splitterrect.Height = REAL(rect.bottom);

			Image* gripper;
			gripper = new Image(L"RolloutIcons\\Gripper.tif");

			m_colors.SetColors();
			m_colors.GetMenuBarColors2003(Grad2, Grad1);

			LinearGradientBrush splitter(splitterrect, Color(255, GetRValue(Grad1)-60, GetGValue(Grad1)-60, GetBValue(Grad1)-60), Color(255, GetRValue(Grad2)-80, GetGValue(Grad2)-80, GetBValue(Grad2)-80), 90.0); // define the button's gradient.
			graphics.FillRectangle(&splitter, splitterrect);

			int splitterrect_X = int((splitterrect.GetRight()-splitterrect.GetLeft() - gripper->GetWidth())/2);

			graphics.DrawImage(gripper, splitterrect_X, int(splitterrect.Y)+1, 35, 3);
		}
	}

	// called only if pane is empty
	void DrawSplitterPane(CDCHandle dc, int nPane)
	{
		RECT rect;
		if (GetSplitterPaneRect(nPane, &rect))
			dc.FillRect(&rect, COLOR_APPWORKSPACE);
	}

	LRESULT OnMouseMoveEx(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		// because of the way the CSplitterImpl::OnMouseMove is written, I had to copy
		// all of the code from it and add the additional code to make it work with
		// snapping functionality.
		T* pT = static_cast<T*>(this);
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if ((wParam & MK_LBUTTON) && ::GetCapture() == pT->m_hWnd)
		{
			if (yPos < m_min_y || yPos < pop_back_y)
				return 0;

			int xyNewSplitPos = 0;

			if(t_bVertical)
				xyNewSplitPos = xPos - m_rcSplitter.left - m_cxyDragOffset;
			else
				xyNewSplitPos = yPos - m_rcSplitter.top - m_cxyDragOffset;

			if(xyNewSplitPos == -1) // avoid -1, that means middle
				xyNewSplitPos = -2;

			// this is all that is required to make it snappable
			if (static_cast<UINT>(abs( xyNewSplitPos - m_xySplitterPos )) > ( m_nSnapSize / 2 ))
			{   
				// move a snap increment as they have dragged more than half way to the next snap position
				if (xyNewSplitPos < m_xySplitterPos)
					xyNewSplitPos = m_xySplitterPos - m_nSnapSize;
				else
					xyNewSplitPos = m_xySplitterPos + m_nSnapSize;
			}
			else // don't move it at all...they haven't dragged far enough
				xyNewSplitPos = m_xySplitterPos;

			if(m_xySplitterPos != xyNewSplitPos)
			{
				if(m_bFullDrag)
				{
					if(pT->SetSplitterPos(xyNewSplitPos, true))
						pT->UpdateWindow();
				}
				else
				{
					DrawGhostBar();
					pT->SetSplitterPos(xyNewSplitPos, false);
					DrawGhostBar();
				}
			}
		}
		else  // not dragging, just set cursor
		{
			if(IsOverSplitterBar(xPos, yPos))
				::SetCursor(m_hCursor);
			bHandled = FALSE;
		}

		return 0;
	}

	void SetSnapSize( UINT xySnapSize ) 
	{
		m_nSnapSize = xySnapSize;
	}

	UINT GetSnapSize()
	{
		return m_nSnapSize;
	}

	void SetSnapMax(UINT xySnapMax) 
	{
		m_min_y = xySnapMax;
	}

	UINT GetSnapMax()
	{
		return m_min_y;
	}

	void SetButtonLimit(UINT xyButtonLimit) 
	{
		m_buttonlimit = xyButtonLimit;
	}

	void SetDesiredButtons(UINT xyDesButtons) 
	{
		RECT rect;
		GetClientRect(&rect);

		m_desired = xyDesButtons;
		baseClass::SetSplitterPos(rect.bottom - (m_desired)*button_height - 4);
	}

	UINT m_nSnapSize;


	BEGIN_MSG_MAP(thisClass)
		if(IsInteractive())
			MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)

		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(baseClass)
		FORWARD_NOTIFICATIONS()
		CHAIN_MSG_MAP( baseClass )
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_cxyMin = 0;

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;
	}

	void OnSizeEx()
	{
		SetSplitterRect();

		RECT rect; // Horizontal Splitter dimensions
		GetClientRect(&rect);
		m_min_y = rect.bottom - (m_buttonlimit - inactivated_buttons + 1)*button_height - 4; // Prevent the horizontal splitter from snapping past its number of buttons.

		int height = rect.bottom - pop_back_y - 4;
		int buttons = height/button_height;

		if (m_desired < buttons) // if we have the wrong number of buttons...
			buttons = m_desired; // sync them with the desired number.

		SetSplitterPos(rect.bottom - buttons*button_height - 4);
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (wParam != SIZE_MINIMIZED)
		{
			OnSizeEx();
		}

		bHandled = FALSE;
		return 1;
	}

	DECLARE_WND_CLASS_EX(_T("WTL_SnapSplitterWindow"), CS_DBLCLKS, COLOR_WINDOW)

};