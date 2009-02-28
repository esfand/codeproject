#include "stdafx.h"
#include <common/accel.h>

// Source code license: Public Domain
// Use the code for whatever purpose, except for clauses that can be found further down in this text. 
// I don't care what you do with it. I also disclaim its fitness
// for use in any software. You cannot hold me liable for any damages which have occurred as a
// consequence of using this source code. You have been informed and warned.
//
// Copyright 2004 Jörgen Sigvardsson <jorgen@profitab.com>
//
// LICENSE CLAUSES:
// 1) You may not remove the copyright notice.
// 2) You may not alter the license.
// 


namespace {
	void Capitalize(CString& str) {
		// Don't use MakeLower() or MakeUpper() as they may be wired to the CRT.
		// The CRT versions of MakeUpper and MakeLower doesn't handle char sets
		// other than US ASCII very well. Ä does not work for instance.
		LPTSTR lpszBuf = str.GetBuffer();
		CharUpperBuff(lpszBuf, 1);
		if(str.GetLength() > 1)
			CharLowerBuff(lpszBuf + 1, str.GetLength() - 1);
		str.ReleaseBuffer();
	}

	bool GetKeyName(UINT nVK, CString& str) {
		UINT nScanCode = MapVirtualKeyEx(nVK, 0, GetKeyboardLayout(0));
		switch(nVK) {
			// Keys which are "extended" (except for Return which is Numeric Enter as extended)
			case VK_INSERT:
			case VK_DELETE:
			case VK_HOME:
			case VK_END:
			case VK_NEXT:  // Page down
			case VK_PRIOR: // Page up
			case VK_LEFT:
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
				nScanCode |= 0x100; // Add extended bit
		}	

		// GetKeyNameText() expects the scan code to be on the same format as WM_KEYDOWN
		// Hence the left shift
		BOOL bResult = GetKeyNameText(nScanCode << 16, str.GetBuffer(20), 19); 
		str.ReleaseBuffer();
		return bResult != FALSE;
	}
}

namespace common {
	bool GetAcceleratorTexts(HACCEL hAccel, std::map<UINT, CString>& mapId2AccelText) {
		if(!hAccel)
			return false;
			
		int nAccelEntries = ::CopyAcceleratorTable(hAccel, 0, 0);
		if(!nAccelEntries)
			return true;  // We're done - there were no accelerators (not an error per se)
			
		ACCEL* pAccels = new ACCEL[nAccelEntries];
		if(!::CopyAcceleratorTable(hAccel, pAccels, nAccelEntries)) {
			delete [] pAccels;
			return false;
		}
		
		for(int i = 0; i < nAccelEntries; ++i) {
			CString strAccel;
			CString strTemp;

			if(pAccels[i].fVirt & FSHIFT) {
				GetKeyName(VK_SHIFT, strTemp);
				Capitalize(strTemp);
				strAccel.Append(strTemp);
			} 
			
			if(pAccels[i].fVirt & FCONTROL) {
				if(strAccel.GetLength())
					strAccel.Append(_T("+"));

				GetKeyName(VK_CONTROL, strTemp);
				Capitalize(strTemp);
				strAccel.Append(strTemp);
			}

			if(pAccels[i].fVirt & FALT) {
				if(strAccel.GetLength())
					strAccel.Append(_T("+"));
				
				GetKeyName(VK_MENU, strTemp);
				Capitalize(strTemp);
				strAccel.Append(strTemp);
			}
			
			if(pAccels[i].fVirt & FVIRTKEY) {
				CString strKey;
				if(GetKeyName(pAccels[i].key, strKey)) {
					Capitalize(strKey);
					if(strAccel.GetLength())
						strAccel.Append(_T("+"));
					strAccel.Append(strKey);
					mapId2AccelText.insert(std::map<UINT, CString>::value_type(pAccels[i].cmd, strAccel));
				}
			} else {
				// key field is an ASCII key code. 
				CString strKey;
#ifdef _UNICODE
				char    ca = (char)pAccels[i].key;
				wchar_t cu;

				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &ca, 1, &cu, 1);
				strKey.AppendChar(cu);
#else
				strKey.AppendChar((char)pAccels[i].key); // Do NOT capitalize key as it's case sensitive
#endif
				if(strAccel.GetLength())
					strAccel.Append(_T("+"));
				strAccel.Append(strKey);
				mapId2AccelText.insert(std::map<UINT, CString>::value_type(pAccels[i].cmd, strAccel));
			}
		}

		return true;
	}
	
	bool GetAcceleratorTexts(HINSTANCE hInst, LPCTSTR lpszAccelRes, std::map<UINT, CString>& mapId2AccelText) {
		if(!hInst) 
			return false;
			
		if(!lpszAccelRes) 
			return false;

		HACCEL hAccel = ::LoadAccelerators(hInst, lpszAccelRes);
		if(!hAccel)
			return false;

		bool bRet = GetAcceleratorTexts(hAccel, mapId2AccelText);
		::DestroyAcceleratorTable(hAccel);
		return bRet;
	}
	
	bool GetAcceleratorTexts(HINSTANCE hInst, int nId, std::map<UINT, CString>& mapId2AccelText) {
		return GetAcceleratorTexts(hInst, MAKEINTRESOURCE(nId), mapId2AccelText);
	}
}