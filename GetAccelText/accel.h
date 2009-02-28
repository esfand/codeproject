#pragma once

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

namespace common {
	// Does not clear mapId2AccelText. Be sure to clear it before calling this function if you 
	// expect to have only what this function generates in it
	bool GetAcceleratorTexts(HACCEL hAccel, std::map<UINT, CString>& mapId2AccelText);	
	bool GetAcceleratorTexts(HINSTANCE hInst, LPCTSTR lpszAccelRes, std::map<UINT, CString>& mapId2AccelText);	
	bool GetAcceleratorTexts(HINSTANCE hInst, int nId, std::map<UINT, CString>& mapId2AccelText);
}