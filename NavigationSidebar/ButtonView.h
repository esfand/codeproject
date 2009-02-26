// ButtonView.h : button view template to be used by interface headers
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NavigationPaneOptions.h"
#include "XP2003Colors.h"
#include "Tooltip.h"
#include "Button.h"

using namespace Gdiplus;

template <class T>
class ButtonView_impl 
	: public COffscreenDrawRect<ButtonView_impl<T> >
	, public CWindowImpl<ButtonView_impl<T> >
{
public:
	std::vector<Button> buttons;
	std::vector<Button> buttons_default;
	
	Gdiplus::Rect chevron;
	Gdiplus::Image* chevronimage;
	HMENU hChevronMenu;

	Gdiplus::Rect commandbar;

	CToolTipWindow tooltipwindow;

	// For theming.
	COLORREF Grad1;
	COLORREF Grad2;
	CXP2003Colors m_colors;

	// Integers for assigning which button is hovered, clicked, and/or selected.
	int button_is_hovered;
	int button_is_clicked;
	int button_is_selected;
	
	// Integers necessary for maintaining the button system.
	int buttonlimit;
	int button_number;
	
	// Integers necessary for the specific button parameters.
	int button_height;
	int little_button_width;

	// For inactivation on the Navigation Pane Options.
	int inactivated_buttons;

	// Necessary for the specific button font.
	Gdiplus::REAL big_button_font_size;
	INT big_button_font_style;
	std::wstring big_button_font_type;

	// For iterators.
	buttons_t m_buttons;
	buttons_t m_buttons_default;

	DECLARE_WND_CLASS(NULL)

	T* me() { return dynamic_cast<T*>(this);}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(ButtonView_impl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMouseClick)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseRelease)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(COffscreenDrawRect<ButtonView_impl>)
	END_MSG_MAP()

	void InactivateButtons()
	{
		// implement in subclass
	}

	void OnSizeEx()
	{
		RECT rcClip;
		me()->GetClientRect(&rcClip);

		button_number = (rcClip.bottom - rcClip.top - button_height)/button_height;

		if (button_number < 0) button_number = 0;
		if (button_number > buttonlimit) button_number=buttonlimit;

		commandbar = Rect(0, int(rcClip.bottom - button_height), int(rcClip.right), button_height); // make the actual button here by creating a Rect.
		chevron = Rect(int(rcClip.right - 24.0f), int(rcClip.bottom - button_height), 24, button_height); // make the actual button here by creating a Rect.

		// set the rects of our buttons.
		float buttonincrement = float(buttonlimit - button_number - inactivated_buttons);
		int buttonincrement_big = 0;
		for (int current_button = 0; current_button < buttonlimit; current_button++)
		{
			if (buttons[current_button].button_is_activated)
			{
				if (current_button >= button_number)
				{
					float button_right = rcClip.right - 24.0f - little_button_width*(buttonincrement); // formula for stacking buttons on the command bar.
					if (button_right < 0)
						button_right = -50;

					buttons[current_button].button_rect = Rect(int(button_right), int(rcClip.bottom - button_height), little_button_width, button_height); // make the actual button here by creating a Rect.
					buttonincrement--;
				}
				else
				{
					buttons[current_button].button_rect = Rect(0, button_height*buttonincrement_big, int(rcClip.right), button_height); // make the actual button here by creating a Rect.
					buttonincrement_big++;
				}
			}
			else
			{
				buttons[current_button].button_rect = Rect(-50, int(rcClip.bottom - button_height), little_button_width, button_height); // make the actual button here by creating a Rect.
				
				if (current_button < button_number)
					button_number++;
			}
		}

		me()->Invalidate();
	}

	void SetButtonStyle(std::wstring type, Gdiplus::REAL size, INT style)
	{
		if (!type.empty()) // if null, don't change it.
			big_button_font_type = type;
		if (size)
			big_button_font_size = size;
		if (style)
			big_button_font_style = style;
	}

	void Set_Small_Button_Width(int i)
	{
		little_button_width = i;
	}

	void Set_Button_Height(int i)
	{
		button_height = i;
	}

	void PaintCommandBar(Graphics& graphics, RECT rcClip)
	{
		// Paint the main command bar
		m_colors.SetColors();
		m_colors.GetMenuBarColors2003(Grad2, Grad1);
		LinearGradientBrush not_hovered_grad (commandbar, Color(255, GetRValue(Grad1), GetGValue(Grad1), GetBValue(Grad1)), Color(255, GetRValue(Grad2)-50, GetGValue(Grad2)-50, GetBValue(Grad2)-50), 90.0); // define the button's gradient.
		graphics.FillRectangle(&not_hovered_grad, commandbar); // paint the gradient on the button.
		
		// Paint the chevron
		if (button_is_hovered == buttonlimit+1) // the button is currently hovered upon by the mouse.
		{
			if (button_is_clicked == buttonlimit+1) // the button is currently clicked upon by the mouse.
			{
				LinearGradientBrush clicked_grad (chevron, Color(255, 250, 220, 125), Color(255, 230, 130, 10), 270.0); // define the button's gradient.
				graphics.FillRectangle(&clicked_grad, chevron); // paint the gradient on the button.
			}
			else // the button has not been clicked upon by the mouse.
			{
				LinearGradientBrush hovered_grad (chevron, Color(255, 255, 255, 255), Color(255, 250, 190, 90), 90.0); // define the button's gradient.
				graphics.FillRectangle(&hovered_grad, chevron); // paint the gradient on the button.
			}
		}
		else // the button is not currently hovered or clicked upon by the mouse.
		{
			m_colors.SetColors();
			m_colors.GetMenuBarColors2003(Grad2, Grad1);
			LinearGradientBrush not_hovered_grad (chevron, Color(255, GetRValue(Grad1), GetGValue(Grad1), GetBValue(Grad1)), Color(255, GetRValue(Grad2)-50, GetGValue(Grad2)-50, GetBValue(Grad2)-50), 90.0); // define the button's gradient.
			graphics.FillRectangle(&not_hovered_grad, chevron); // paint the gradient on the button.
		}

		graphics.DrawImage(chevronimage, Gdiplus::REAL(rcClip.right - 21), Gdiplus::REAL(rcClip.bottom - 18 - (button_height - 18) / 2.5), Gdiplus::REAL(18), Gdiplus::REAL(18));
		
		m_colors.SetColors();
		COLORREF bordercolor = m_colors.GetMenuBarColor2003();
		
		Pen border (Color(255, GetRValue(bordercolor)-80, GetGValue(bordercolor)-80, GetBValue(bordercolor)-80)); // Define our border to surround our commandbar with.

		Rect commandbar_border = commandbar;
		commandbar_border.Width -= 1;
		commandbar_border.Height -= 1;

		graphics.DrawRectangle(&border, commandbar_border);
	}

	void DrawCommandBarButton(Graphics& graphics, RECT rcClip, int current_button)
	{
		// command bar buttons
		int button_right = buttons[current_button].button_rect.GetLeft(); //rcClip.right - 24.0f - little_button_width*(buttonincrement); // formula for stacking buttons on the command bar.
		if (button_right > 0)
		{
			graphics.DrawImage(buttons[current_button].imagepath_small, 
				button_right + (little_button_width - buttons[current_button].imagepath_small->GetWidth()) / 2, 
				int(rcClip.bottom - buttons[current_button].imagepath_small->GetHeight() - (button_height - buttons[current_button].imagepath_small->GetHeight()) / 2), 
				buttons[current_button].imagepath_small->GetWidth(), 
				buttons[current_button].imagepath_small->GetHeight());
			
			m_colors.SetColors();
			COLORREF bordercolor = m_colors.GetMenuBarColor2003();
			
			Pen border (Color(255, GetRValue(bordercolor)-80, GetGValue(bordercolor)-80, GetBValue(bordercolor)-80)); // Define our border to surround our buttons with.

			Rect commandbutton_border = buttons[current_button].button_rect;
			commandbutton_border.X = 0;
			commandbutton_border.Width = rcClip.right;
			commandbutton_border.Height -= 1;

			graphics.DrawRectangle(&border, commandbutton_border);
		}
	}

	void DrawBigButton(Graphics& graphics, RECT rcClip, int current_button)
	{
		SolidBrush textbrush (Color(255, 0, 0, 0)); // Define our text brush.
		Font textfont (big_button_font_type.c_str(), big_button_font_size, big_button_font_style); // Define our text font.

		int button_top = buttons[current_button].button_rect.GetTop(); // formula for stacking buttons.
		if (button_top < (rcClip.bottom) - button_height)
		{
			RectF TextRect;
			graphics.MeasureString(buttons[current_button].caption.c_str(), buttons[current_button].caption.size(), &textfont, PointF(0.0f, 0.0f), &TextRect);
			REAL width_setting = REAL(button_height + buttons[current_button].imagepath->GetWidth()) / 2;

			if (buttons[current_button].imagepath->GetWidth() == 0)
				width_setting = 0;

			graphics.DrawString(buttons[current_button].caption.c_str(), -1, &textfont, PointF(width_setting, REAL(button_top + (button_height - (TextRect.GetBottom() - TextRect.GetTop())) / 2)), &textbrush); // paint the text on our buttons.
			
			m_colors.SetColors();
			COLORREF bordercolor = m_colors.GetMenuBarColor2003();
			
			Pen border (Color(255, GetRValue(bordercolor)-80, GetGValue(bordercolor)-80, GetBValue(bordercolor)-80)); // Define our border to surround our buttons with.

			Rect border_rect = buttons[current_button].button_rect;
			border_rect.Width -= 1;

			graphics.DrawRectangle(&border, border_rect); // paint the border around our buttons.
			graphics.DrawImage(buttons[current_button].imagepath, 
				(button_height - buttons[current_button].imagepath->GetWidth()) / 2, 
				int(button_top + (button_height - buttons[current_button].imagepath->GetHeight()) / 2), 
				buttons[current_button].imagepath->GetWidth(), 
				buttons[current_button].imagepath->GetHeight());
		}
	}

	void PaintButton(Graphics& graphics, int current_button)
	{
		if (button_is_hovered == current_button) // the button is currently hovered upon by the mouse.
		{
			if (button_is_clicked == current_button) // the button is currently clicked upon by the mouse.
			{
				LinearGradientBrush clicked_grad (buttons[current_button].button_rect, Color(255, 250, 220, 125), Color(255, 230, 130, 10), 270.0); // define the button's gradient.
				graphics.FillRectangle(&clicked_grad, buttons[current_button].button_rect); // paint the gradient on the button.
			}
			else if (button_is_selected == current_button) // the button is currently selected and hovered upon.
			{
				LinearGradientBrush selected_grad (buttons[current_button].button_rect, Color(255, 250, 230, 150), Color(255, 240, 150, 20), 270.0); // define the button's gradient.
				graphics.FillRectangle(&selected_grad, buttons[current_button].button_rect); // paint the gradient on the button.
			}
			else // the button has not been clicked upon by the mouse.
			{
				LinearGradientBrush hovered_grad (buttons[current_button].button_rect, Color(255, 255, 255, 255), Color(255, 250, 190, 90), 90.0); // define the button's gradient.
				graphics.FillRectangle(&hovered_grad, buttons[current_button].button_rect); // paint the gradient on the button.
			}
		}
		else // the button is not currently hovered or clicked upon by the mouse.
		{
			if (button_is_selected == current_button) // the button is currently selected.
			{
				LinearGradientBrush selected_grad (buttons[current_button].button_rect, Color(255, 250, 230, 150), Color(255, 240, 150, 20), 90.0); // define the button's gradient.
				graphics.FillRectangle(&selected_grad, buttons[current_button].button_rect); // paint the gradient on the button.
			}
			else // the button is not selected either
			{
				m_colors.SetColors();
				m_colors.GetMenuBarColors2003(Grad2, Grad1);
				LinearGradientBrush not_hovered_grad (buttons[current_button].button_rect, Color(255, GetRValue(Grad1), GetGValue(Grad1), GetBValue(Grad1)), Color(255, GetRValue(Grad2)-50, GetGValue(Grad2)-50, GetBValue(Grad2)-50), 90.0); // define the button's gradient.
				graphics.FillRectangle(&not_hovered_grad, buttons[current_button].button_rect); // paint the gradient on the button.
			}
		}
	}

	void DoPaint(HDC dc, RECT& r)
	{
		RECT rcClip;
		GetClientRect(&rcClip);
		Graphics graphics(dc);

		PaintCommandBar(graphics, rcClip); // Paint the Command Bar and Chevron.

		if (r.bottom == rcClip.bottom)
		{
			for (int current_button = 0; current_button < buttonlimit; current_button++) // We make sure to make it add to go through the loop so that the buttons are defined and painted top to bottom.
			{
				if (buttons[current_button].button_is_activated)
				{
					PaintButton(graphics, current_button); // Paint the button's main gradient.

					if (current_button >= button_number)
						DrawCommandBarButton(graphics, rcClip, current_button); // Draw our Command Bar Buttons.
					else
						DrawBigButton(graphics, rcClip, current_button); // Draw our big buttons above the command bar.
				}
			}
		}
		else
		{
			for (int current_button = 0; current_button < buttonlimit; current_button++) // We make sure to make it add to go through the loop so that the buttons are defined and painted top to bottom.
			{
				if (buttons[current_button].button_rect.GetBottom() == r.bottom)
				{
					PaintButton(graphics, current_button); // Paint the button's main gradient.

					if (current_button >= button_number)
						DrawCommandBarButton(graphics, rcClip, current_button); // Draw our Command Bar Buttons.
					else
						DrawBigButton(graphics, rcClip, current_button); // Draw our big buttons above the command bar.
					
					return;
				}
			}
		}
	}

	void AddButton(std::wstring caption, std::wstring imagepath, std::wstring imagepath_small)
	{
		// add the button and set its parameters
		buttons.resize(buttonlimit+1);
		buttons[buttonlimit].button_is_activated = true;
		buttons[buttonlimit].caption = caption;
		buttons[buttonlimit].button_id = buttonlimit;
		buttons[buttonlimit].imagepath = new Image(imagepath.c_str());
		buttons[buttonlimit].imagepath_small = new Image(imagepath_small.c_str());

		// add the button to a default list that the reset button will reset to.
		buttons_default.resize(buttonlimit+1);
		buttons_default[buttonlimit].caption = caption;
		buttons_default[buttonlimit].imagepath = new Image(imagepath.c_str());
		buttons_default[buttonlimit].imagepath_small = new Image(imagepath_small.c_str());

		// add our buttons to the Navigation Pane Options.
		m_buttons.push_back(std::make_pair(buttons[buttonlimit].caption, true));
		m_buttons_default.push_back(std::make_pair(buttons[buttonlimit].caption, true));

		buttonlimit = buttonlimit + 1;
		button_is_hovered = buttonlimit; // recalculate from new buttonlimit.
		button_is_clicked = buttonlimit; // recalculate from new buttonlimit.
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rcClip;
		GetClientRect(&rcClip);

		// Create our tooltipwindow. This is required in order to display tooltips below the command bar buttons.
		tooltipwindow.Create(*me(), NULL, L"Tooltip",WS_POPUP|SWP_NOACTIVATE);

		// Main Parameters
		buttonlimit = 0; // must always be set to zero. Just AddButton for every button you want.

		chevronimage = new Image(L"RolloutIcons\\chevron.tif"); // our chevron image.
		button_number = 0; // initially no buttons are to be painted.
		inactivated_buttons = 0; // initially no buttons are inactivated.

		return 0;
	}

	void Swap_Necessary(int i, int j)
	{
		Button tempbutton;
		tempbutton = buttons[j];
		buttons[j] = buttons[i];
		buttons[i] = tempbutton;

		if (button_is_selected == i)
			button_is_selected = j;
		else if (button_is_selected == j)
			button_is_selected = i;
	}

	// override
	void OnSelect(int i)
	{
		// change in subclass
	}

	void Swap_Buttons(int i, int j)
	{
		// change the rest in subclass
	}

	void SetUpToolTip(int current_button)
	{
		if (current_button >= button_number)
		{
			RECT r;
			me()->GetClientRect(&r);
			me()->ClientToScreen(&r);
			RECT button;
			button.left = buttons[current_button].button_rect.GetLeft();
			me()->ClientToScreen(&button);

			if (tooltipwindow.x != button.left) // Don't do anything to the tooltip window if it's already there!
			{
				tooltipwindow.tt_window_caption = buttons[current_button].caption;
				tooltipwindow.x = button.left;
				tooltipwindow.y = r.bottom;
			
				tooltipwindow.SetWindowPos(HWND_TOP, tooltipwindow.x, tooltipwindow.y, 55, 18, SWP_SHOWWINDOW|SWP_NOACTIVATE);
				me()->RedrawWindow();
			}
		}
	}

	void CheckMousePosition()
	{
		if ((button_is_clicked == buttonlimit) && (button_is_hovered == buttonlimit)) // nothing is clicked or hovered upon
		{
			HCURSOR ArrowPtr = LoadCursor(NULL, IDC_ARROW);
			SetCursor(ArrowPtr);
			tooltipwindow.ShowWindow(SW_HIDE);
			tooltipwindow.x = 0;
			ReleaseCapture();
		}
		else if ((button_is_hovered == buttonlimit+1) || (button_is_hovered < button_number)) // either the chevron or the big buttons are hovered upon
		{
			tooltipwindow.ShowWindow(SW_HIDE);
			tooltipwindow.x = 0;
		}
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int mouse_x = LOWORD(lParam);
		int mouse_y = HIWORD(lParam);
		button_is_hovered = buttonlimit;

		int current_button;
		for (current_button = 0; current_button < buttonlimit; current_button += 1) // We work through the buttons and check if the mouse is currently over them.
		{
			if ((mouse_x > buttons[current_button].button_rect.GetLeft()) 
				&& (mouse_x < buttons[current_button].button_rect.GetRight()) 
				&& (mouse_y > buttons[current_button].button_rect.GetTop()) 
				&& (mouse_y < buttons[current_button].button_rect.GetBottom()))
			{
				HCURSOR Hand = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));
				SetCursor(Hand);
				button_is_hovered = current_button;

				SetUpToolTip(current_button);

				me()->SetCapture();
				break;
			}
			else if ((mouse_x > chevron.GetLeft()) && (mouse_x < chevron.GetRight()) && (mouse_y > chevron.GetTop()) && (mouse_y < chevron.GetBottom()))
			{
				HCURSOR Hand = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));
				SetCursor(Hand);
				button_is_hovered = buttonlimit+1;
				
				me()->SetCapture();
				break;
			}
		}

		CheckMousePosition();
		me()->Invalidate();
		return 0;
	}

	LRESULT OnMouseClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int mouse_x = LOWORD(lParam);
		int mouse_y = HIWORD(lParam);
		RECT client_rect;
		GetClientRect(&client_rect);
		
		if (mouse_y >= client_rect.bottom - (button_number+1)*31)
		{
			button_is_clicked = buttonlimit;

			int current_button;
			for (current_button = 0; current_button < buttonlimit; current_button += 1) // We work through the buttons and check if the mouse is currently over them.
			{
				if ((mouse_x > buttons[current_button].button_rect.GetLeft()) && (mouse_x < buttons[current_button].button_rect.GetRight()) && (mouse_y > buttons[current_button].button_rect.GetTop()) && (mouse_y < buttons[current_button].button_rect.GetBottom()))
				{
					if (button_is_selected != current_button)
						button_is_clicked = current_button;
					me()->SetCapture();
					break;
				}
				else if ((mouse_x > chevron.GetLeft()) && (mouse_x < chevron.GetRight()) && (mouse_y > chevron.GetTop()) && (mouse_y < chevron.GetBottom()))
				{
					button_is_clicked = buttonlimit+1;
					me()->SetCapture();
					break;
				}
			}

			CheckMousePosition();
			me()->Invalidate();
		}

		return 0;
	}

	void SetUpNavigationPaneOptions()
	{
		CNavigPane navigpane;
		navigpane.buttons = m_buttons;
		navigpane.buttons_default = m_buttons_default;

		if (IDOK==navigpane.DoModal())
		{
			m_buttons = navigpane.buttons;
			inactivated_buttons = 0;

			// edit buttons
			buttons_t::iterator p = m_buttons.begin(), e = m_buttons.end();
			for (int i = 0; p!=e; p++,i++)
			{
				if (p->first != buttons[i].caption)
				{
					for (int j = 0; j < buttonlimit; j++)
					{
						if (p->first == buttons[j].caption)
						{
							me()->Swap_Necessary(i, j);
							me()->Swap_Buttons(i, j);
						}
					}
				}
				else if (p->second != buttons[i].button_is_activated)
					buttons[i].button_is_activated = p->second;

				if (!buttons[i].button_is_activated)
					inactivated_buttons++;
			}
			me()->InactivateButtons();
			OnSizeEx();
		}
	}

	void ChevronMenu()
	{
		// create our menu
		hChevronMenu = CreatePopupMenu();

		UINT Navigation_Pane_Options = 999;

		AppendMenu(hChevronMenu, MF_STRING, Navigation_Pane_Options, L"&Navigation Pane Options...");

		// Append the buttons that are off the page and activated still.
		int current_button;
		bool onetimeseparator = true;
		for (current_button = 0; current_button < buttonlimit; current_button++) // We work through the buttons and check if the mouse is currently over them.
		{
			if (current_button >= button_number
				&& buttons[current_button].button_rect.X < 0
				&& buttons[current_button].button_is_activated)
			{
				if (onetimeseparator)
				{
					AppendMenu(hChevronMenu, MF_SEPARATOR, 0, NULL);
					onetimeseparator = false;
				}
				AppendMenu(hChevronMenu, MF_STRING, current_button+1, buttons[current_button].caption.c_str());
				if (current_button == button_is_selected)
					HiliteMenuItem(hChevronMenu, current_button+1, MF_BYCOMMAND | MF_HILITE);
			}
		}

		POINT pt;
		GetCursorPos(&pt);
		DWORD id = TrackPopupMenu(hChevronMenu,
			TPM_LEFTBUTTON|TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,
			pt.x, 
			pt.y, 
			0,
			*me(),
			NULL);
		button_is_hovered = buttonlimit;

		if (id == 999) // We've triggered the Navigation Pane Options.
		{
			SetUpNavigationPaneOptions();
		}
		else if (id != 0) // Something else on the chevron popup was clicked.
		{
			T* me = (T*)this;
			me->OnSelect(id - 1); // id is set to be 1 higher than the real button as it defaults to 0 if nothing is clicked.
			button_is_clicked = buttonlimit;
		}
	}

	LRESULT OnMouseRelease(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rcClip;
		me()->GetClientRect(&rcClip);

		if (button_is_clicked != buttonlimit) // a button is clicked
		{
			if (button_is_clicked == buttonlimit+1) // the chevron was clicked
			{
				button_is_clicked = buttonlimit;
				me()->RedrawWindow(); // redraw to give the proper graphic to the chevron

				ChevronMenu(); // Create our chevron menu.
			}
			else // A button that is not a chevron has been clicked.
			{
				me()->OnSelect(button_is_clicked);
				button_is_clicked = buttonlimit;
			}

			ReleaseCapture();
		}

		CheckMousePosition();
		me()->Invalidate();
		return 0;
	}


	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		OnSizeEx();
		return 0;
	}
};