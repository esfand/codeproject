// ButtonView_TabbedButtons.h : interface of the tabbed button view by templating the button view and adding tab support
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ButtonView.h"
#include "Button.h"

template<class T, class BT>
class ButtonView_Tabbed : public CSplitterImpl<T,true>
{
public:
	std::vector< std::pair<HWND,HWND> > buttonviews;
	std::pair<HWND, HWND> default_view;
	BT m_buttons;
	CHorSnapSplitterWindow m_HorizontalSplitter;
	int current_view;

	T* me() { return static_cast<T*>(this); }

	COLORREF Grad1;
	CXP2003Colors m_colors;

	enum { t_bVertical = 1 };

	void Show_View(int i)
	{
		SetSplitterPanes(m_HorizontalSplitter, buttonviews[i].first); // Sets the vertical splitter to contain the button window on its left and nothing on its right.
		::ShowWindow(buttonviews[i].first, SW_SHOW);

		m_HorizontalSplitter.SetSplitterPanes(buttonviews[i].second, m_buttons); // Sets the horizontal splitter to have nothing on the top and the buttons on the bottom.
		::ShowWindow(buttonviews[i].second, SW_SHOW);

		if (current_view != i)
		{
			::ShowWindow(buttonviews[current_view].first, SW_HIDE);
			::ShowWindow(buttonviews[current_view].second, SW_HIDE);
		}

		current_view = i;
		m_buttons.button_is_selected = i;
		m_buttons.Invalidate();
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

			m_colors.SetColors();
			Grad1 = m_colors.GetMenuBarColor2003();

			SolidBrush splitter(Color(255, GetRValue(Grad1), GetGValue(Grad1), GetBValue(Grad1))); // define the splitter's main color.
			graphics.FillRectangle(&splitter, splitterrect);
		}
	}

	// called only if pane is empty
	void DrawSplitterPane(CDCHandle dc, int nPane)
	{
		RECT rect;
		if (GetSplitterPaneRect(nPane, &rect))
			dc.FillRect(&rect, COLOR_APPWORKSPACE);
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(wParam != SIZE_MINIMIZED)
			SetSplitterRect();

		bHandled = FALSE;
		return 1;
	}

	void SetButtonFont(std::wstring type, Gdiplus::REAL size, INT style)
	{
		m_buttons.SetButtonStyle(type, size, style);
	}

	void RecalculateLimits()
	{
		RECT rect; // Vertical Splitter dimensions
		me()->GetClientRect(&rect);

		m_HorizontalSplitter.SetSnapMax(rect.bottom - ((m_buttons.buttonlimit+1)*m_buttons.button_height));
		m_HorizontalSplitter.SetButtonLimit(m_buttons.buttonlimit);
	}

	void AddButtonWithView(std::wstring caption, std::wstring imagepath, std::wstring imagepath_small, HWND right_side, HWND top_side)
	{
		m_buttons.AddButton(caption, imagepath, imagepath_small); // Do the original button adding.

		// Add our view.
		buttonviews.push_back(std::make_pair(right_side,top_side));

		if (right_side)
			::SetWindowPos(right_side, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

		if (top_side)
			::SetWindowPos(top_side, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

		if (buttonviews.size() == 1)
			Show_View(0);
		else
		{
			::ShowWindow(right_side, SW_HIDE);
			::ShowWindow(top_side, SW_HIDE);
		}

		RecalculateLimits();
	}

	void SetDefaultButtons(int i)
	{
		if(i == m_HorizontalSplitter.m_desired)
			return;
		
		m_HorizontalSplitter.SetDesiredButtons(i);
	}

	void SetPopBack(int pop_back_amount)
	{
		m_HorizontalSplitter.pop_back_y = pop_back_amount;
	}

	void SetButtonHeight(int i)
	{
		m_buttons.Set_Button_Height(i);
		m_HorizontalSplitter.button_height = m_buttons.button_height;
	}

	void SetSmallButtonWidth(int i)
	{
		m_buttons.Set_Small_Button_Width(i);
	}

	typedef CSplitterImpl<T,true> baseClass;
	BEGIN_MSG_MAP(ButtonView_Tabbed)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = false;
		RECT splitter_rect;
		splitter_rect.bottom = 0;
		splitter_rect.right = 0;
		current_view = 0;
		m_HorizontalSplitter.inactivated_buttons = 0;

		// The horizontal splitter that is required by the rollout.
		SetSplitterExtendedStyle(0); // makes it so that our splitter doesn't move when the window is resized.
		m_HorizontalSplitter.Create(*me(), splitter_rect, L"Horizontal Splitter", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		m_HorizontalSplitter.SetSplitterExtendedStyle(SPLIT_BOTTOMALIGNED); // Makes it so that the horizontal bar stays put when the screen resizes.
		
		// Create our actual view of buttons.
		m_buttons.Create(m_HorizontalSplitter, splitter_rect, L"Button View", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		// Set various Splitter settings...
		m_HorizontalSplitter.SetSplitterPanes(NULL, m_buttons); // Sets the horizontal splitter to have nothing on the top and the buttons on the bottom.
		SetSplitterPanes(m_HorizontalSplitter, NULL); // Sets the vertical splitter to contain the button window on its left and nothing on its right.

		RECT rect; // Vertical Splitter dimensions
		me()->GetClientRect(&rect);

		m_HorizontalSplitter.SetSnapSize(m_buttons.button_height); // Set the horizontal splitter to snap at the rate of a button's height.

		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_buttons.tooltipwindow)
			m_buttons.tooltipwindow.DestroyWindow();
		return 1;
	}

	void OnSelect(int i)
	{
		Show_View(i);
	}

	void Swap_Buttons(int i, int j)
	{
		HWND right_side;
		HWND top_side;
		right_side = buttonviews[j].first;
		top_side = buttonviews[j].second;
		buttonviews[j].first = buttonviews[i].first;
		buttonviews[j].second = buttonviews[i].second;
		buttonviews[i].first = right_side;
		buttonviews[i].second = top_side;
	}

	void InactivateButtons()
	{
		RECT rect;
		me()->GetClientRect(&rect);

		int big_inactivated_buttons = 0;
		for (size_t i = 0; i < m_buttons.buttons.size(); i++)
		{
			if (int(i) < m_buttons.button_number && m_buttons.buttons[i].button_is_activated == false)
			{
				big_inactivated_buttons++;
			}
		}

		if (m_buttons.button_number - m_buttons.inactivated_buttons + big_inactivated_buttons < 0)
			m_HorizontalSplitter.SetDesiredButtons(m_buttons.button_number - big_inactivated_buttons + 1);

		m_HorizontalSplitter.inactivated_buttons = m_buttons.inactivated_buttons;
		m_HorizontalSplitter.SetSnapMax(rect.bottom - ((m_buttons.buttonlimit - m_buttons.inactivated_buttons + 1)*m_buttons.button_height) - 4);

		if (m_HorizontalSplitter.GetSplitterPos() < rect.bottom - (m_buttons.buttonlimit - m_buttons.inactivated_buttons + 1)*m_buttons.button_height - 4)
			m_HorizontalSplitter.SetSplitterPos(rect.bottom - (m_buttons.buttonlimit - m_buttons.inactivated_buttons + 1)*m_buttons.button_height - 4);
	}
};

class ButtonView_TabbedWindow;
class ButtonView_Tabbed_cw 
	: public ButtonView_impl<ButtonView_Tabbed_cw>
{
	typedef ButtonView_impl<ButtonView_Tabbed_cw> baseClass;

	BEGIN_MSG_MAP(ButtonView_Tabbed_cw)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP( )

public:
	ButtonView_TabbedWindow* m_owner;

	// overrides
	void OnSelect(int i);
	void Swap_Buttons(int i, int j);
	void InactivateButtons();
}; 


class ButtonView_TabbedWindow 
	: public ButtonView_Tabbed<ButtonView_TabbedWindow, ButtonView_Tabbed_cw>
	, public CWindowImpl<ButtonView_TabbedWindow>
{
	typedef ButtonView_Tabbed<ButtonView_TabbedWindow, ButtonView_Tabbed_cw> baseClass;

	BEGIN_MSG_MAP(ButtonView_TabbedWindow)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	ButtonView_TabbedWindow()
	{
		m_buttons.m_owner=this;
	}
};

inline void ButtonView_Tabbed_cw::OnSelect(int i)
{
	m_owner->OnSelect(i);
}

inline void ButtonView_Tabbed_cw::Swap_Buttons(int i, int j)
{
	m_owner->Swap_Buttons(i, j);
}

inline void ButtonView_Tabbed_cw::InactivateButtons()
{
	m_owner->InactivateButtons();
}