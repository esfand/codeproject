/** \page wtlgdiex wtlgdiex.h
 *
 *  \section License License
 *  Copyright (c) 2005, 2006 Massimiliano Alberti <xanatos@geocities.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject
 *  to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  \section VersionHistory Version History
 *  \li wtlgdiex.h   1.0	May 2006  Initial Release
 *
 */

/** \file
 *  This header contains various extended classes for WTL.
 */

#pragma once

#ifndef __CMEMORYSTRIPDC__
#define __CMEMORYSTRIPDC__

#ifndef __ATLGDI_H__
#error wtlgdiex.h requires atlgdi.h to be included first
#endif

//

namespace WTL
{

//

/// A variant of WTL::CMemoryDC usable to paint "strips" one at a time
/** ~CMemoryStripDC DOESN'T draw the cached DC. You must explicitally draw it with
 *  Draw(). When you want to move the strip you can use Move().
 *  Note that you can Draw(), Move() and Draw() again.
 */
class CMemoryStripDC : public CDC
{
public:
	HDC m_hDCOriginal;	///< The original DC
	RECT m_rcPaint;		///< The rectangle that CMemoryStripDC will cache
	CBitmap m_bmp;		///< The bitmap where the drawing will be cached
	HBITMAP m_hBmpOld;	///< The original bitmap (received from the SelectBitmap)

	/// Constructor
	/** \param hDC The original DC
	 *  \param rcPaint The rectangle that CMemoryStripDC will cache
	 */
	CMemoryStripDC(HDC hDC, RECT& rcPaint) : m_hDCOriginal(hDC), m_hBmpOld(NULL)
	{
		ATLASSERT(hDC != NULL);

		m_rcPaint = rcPaint;

		ATLVERIFY(CreateCompatibleDC(m_hDCOriginal));
		ATLVERIFY(m_bmp.CreateCompatibleBitmap(m_hDCOriginal, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top));

		m_hBmpOld = SelectBitmap(m_bmp);

		ATLVERIFY(SetViewportOrg(-m_rcPaint.left, -m_rcPaint.top));
	}

	/// Destructor
	~CMemoryStripDC()
	{
		SelectBitmap(m_hBmpOld);
	}

	/// BitBlt the cached bitmap to the original DC. The cached bitmap is still valid after drawing
	void Draw()
	{
		ATLVERIFY(::BitBlt(m_hDCOriginal, m_rcPaint.left, m_rcPaint.top, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top, m_hDC, m_rcPaint.left, m_rcPaint.top, SRCCOPY));
	}

	/// Moves the Viewport. The cached bitmap is still valid after calling this method
	/** \param ix New x coordinate of the rectangle (the width can't be changed and will be the same as before)
	 *  \param iy New y coordinate of the rectangle (the height can't be changed and will be the same as before)
	 */
	void Move(int ix, int iy)
	{
		m_rcPaint.bottom = (m_rcPaint.bottom - m_rcPaint.top) + iy;
		m_rcPaint.top = iy;
		m_rcPaint.right = (m_rcPaint.right - m_rcPaint.left) + ix;
		m_rcPaint.left = ix;

		ATLVERIFY(SetViewportOrg(-m_rcPaint.left, -m_rcPaint.top));
	}

	/// Zeroes the cached bitmap (the bitmap will then represent be a black rectangle)
	void Empty()
	{
		ATLVERIFY(::BitBlt(m_hDC, m_rcPaint.left, m_rcPaint.top, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top, NULL, 0, 0, BLACKNESS));
	}
};

}; // namespace WTL

#endif // __CMEMORYSTRIPDC__

