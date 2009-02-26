# pragma once

/////////////////////////////////////////////////////////////
// Original code by Adi DEDIC
// Converted to pure Win32 by Adel Amro.


// Notes: Subclass any tab control and replace the WM_PAINT handler to call this function.
// See accompanying sample to see how this can be done.
// For some reason, this doesn't look right if you don't manually change the font into the
// default gui font (could work with another font, but not the system default font).
// Also, it won't work correctly except if you create the control with the TCS_OWNERDRAWFIXED
// style. Note that even if you do that, the control will still have tabs of varying sizes.
// I have no idea why you have to enable this style flag, to be frank.
// Bottom line, this is crappy, and I won't be using it in my own projects, but it could be
// just what you need for yours.

// Developed and tested on Visual C++ 2005 Express Edition.


# include <windows.h>

VOID DrawCustomTabControl( HWND hTabControl, HDC dc );