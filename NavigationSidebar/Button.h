// Defines a button.

#pragma once 
#include "stdafx.h"

struct Button
{
	int button_id;
	bool button_is_activated;
	Rect button_rect;
	std::wstring caption;
	Gdiplus::Image* imagepath;
	Gdiplus::Image* imagepath_small;

	void operator=(const Button& button)
	{
		button_is_activated = button.button_is_activated;
		caption = button.caption;
		imagepath = button.imagepath;
		imagepath_small = button.imagepath_small;
	}

	Button(const Button& button)
	{
		*this = button;
	}

	Button(){}
};