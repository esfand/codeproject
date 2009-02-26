#if !defined(_TITLETIPHOOK_H_INCLUDED_)

#pragma once

#include "MessageHook.h"

#ifndef WM_XBUTTONDOWN

#define WM_XBUTTONDOWN                  0x020B
#define WM_XBUTTONUP                    0x020C
#define WM_XBUTTONDBLCLK                0x020D

#endif	// #ifndef WM_XBUTTONUP

#ifndef WM_MOUSEWHEEL

#define WM_MOUSEWHEEL                   0x020A

#endif	// #ifndef WM_MOUSEWHEEL

namespace codeproject
{

class CTitleTipHook : public CMessageHook<CTitleTipHook>
{
public:
	enum HOOKCONTROL
	{
		EditHook		= 0,
		ListBoxHook		= 1,
		ComboBoxHook	= 2,
	} m_controlType;

	CTitleTipHook() : CMessageHook<CTitleTipHook>(TRUE)
	{
	}
};

}	// namespace codeproject

#endif // !defined(_TITLETIPHOOK_H_INCLUDED_)
