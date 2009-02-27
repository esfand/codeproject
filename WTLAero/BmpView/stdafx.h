// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef __STDAFX_H__
#define __STDAFX_H__

// Change these values to use different versions
#ifndef _WIN32_WCE
#define  WINVER		    0x0501
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	    0x0700
#define _RICHEDIT_VER	0x0200
#else
#define WINVER		0x0400
#define _WIN32_IE	0x0400
#endif

#define _SECURE_ATL 1

#ifdef WIN32_PLATFORM_PSPC
#include <aygshell.h>
#endif // WIN32_PLATFORM_PSPC

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlctrls.h>
#include <atlctrlx.h>


#endif // __STDAFX_H__
