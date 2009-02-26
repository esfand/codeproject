// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change this value to use different versions
#define WINVER 0x0420
#include <atlbase.h>

#define _WTL_USE_CSTRING
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#if (_WIN32_WCE >= 420)
#include <tpcshell.h>
#endif
#include <aygshell.h>
#pragma comment(lib, "aygshell.lib")
