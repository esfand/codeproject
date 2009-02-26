/** \page stringcex stringcex.h
 *
 *  \section License License
 *  Copyright (c) 2005 Massimiliano Alberti <xanatos@geocities.com>
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
 *  \li stringcex.h   1.0   May 2005  Initial Release
 *
 *  \section TODO TODO
 *  \li "_l" version of functions (see strcmp_l)
 */

#pragma once

#ifndef __STRINGCEX_H__
#define __STRINGCEX_H__

extern "C" {
/// Compares two strings. Checks if the first is the initial substring of the second ("A" is an initial substring of "AB"). Case sensitive.
/** \param substr First string (the "substring").
 *  \param str Second string (the "string").
 *  \return -1 if substr is a substring of str, 0 if substr == str, 1 otherwise
 */
inline int strcmpsub(const char *substr, const char *str)
{
	while (true) {
		if (!*substr) {
			if (!*str) {
				return 0;
			} else {
				return -1;
			}
		}

		if (!*str) {
			return 1;
		}

		if (*substr != *str) {
			return 1;
		}

		substr++;
		str++;
	}
}

/// Compares two strings. UNICODE. Checks if the first is the initial substring of the second ("A" is an initial substring of "AB"). Case sensitive.
/** \param wsubstr First string (the "substring").
 *  \param wstr Second string (the "string").
 *  \return -1 if wsubstr is a substring of wstr, 0 if substr == str, 1 otherwise
 */
inline int wcscmpsub(const wchar_t *wsubstr, const wchar_t *wstr)
{
	while (true) {
		if (!*wsubstr) {
			if (!*wstr) {
				return 0;
			} else {
				return -1;
			}
		}

		if (!*wstr) {
			return 1;
		}

		if (*wsubstr != *wstr) {
			return 1;
		}

		wsubstr++;
		wstr++;
	}
}

/// Compares two strings. Checks if the first is the initial substring of the second ("A" is an initial substring of "AB"). Case INsensitive.
/** \param substr First string (the "substring").
 *  \param str Second string (the "string").
 *  \return -1 if substr is a substring of str, 0 if substr == str, 1 otherwise
 */
inline int stricmpsub(const char *substr, const char *str)
{
	while (true) {
		if (!*substr) {
			if (!*str) {
				return 0;
			} else {
				return -1;
			}
		}

		if (!*str) {
			return 1;
		}

		if (::tolower(*substr) != ::tolower(*str)) {
			return 1;
		}

		substr++;
		str++;
	}
}

/// Compares two strings. UNICODE. Checks if the first is the initial substring of the second ("A" is an initial substring of "AB"). Case INsensitive.
/** \param wsubstr First string (the "substring").
 *  \param wstr Second string (the "string").
 *  \return -1 if wsubstr is a substring of wstr, 0 if wsubstr == wstr, 1 otherwise
 */
inline int wcsicmpsub(const wchar_t *wsubstr, const wchar_t *wstr)
{
	while (true) {
		if (!*wsubstr) {
			if (!*wstr) {
				return 0;
			} else {
				return -1;
			}
		}

		if (!*wstr) {
			return 1;
		}

		if (::tolower(*wsubstr) != ::tolower(*wstr)) {
			return 1;
		}

		wsubstr++;
		wstr++;
	}
}
}

#ifdef _UNICODE
#define _tcscmpsub wcscmpsub	///< Auto-select macro between ::strcmpsub and ::wcscmpsub
#define _tcsicmpsub wcsicmpsub	///< Auto-select macro between ::stricmpsub and ::wcsicmpsub
#else
#define _tcscmpsub strcmpsub	///< Auto-select macro between ::strcmpsub and ::wcscmpsub
#define _tcsicmpsub stricmpsub	///< Auto-select macro between ::stricmpsub and ::wcsicmpsub
#endif

#endif // __STRINGCEX_H__