// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// _WIN32_WINNT is set by most SDKs but not all. Uncomment next line if needed
//#define _WIN32_WINNT	0x0501

#define _RICHEDIT_VER	0x0200

#define _SECURE_ATL 1

#if _MSC_VER >= 1400 // may be a VC Express edition
	#include <atldef.h>
	// Support for VS2005 Express & SDK ATL
	#if _ATL_VER <= 0x700 
		#define _WTL_SUPPORT_SDK_ATL3
		#define _CRT_SECURE_NO_DEPRECATE
		#pragma conform(forScope, off)
		#pragma comment(linker, "/NODEFAULTLIB:atlthunk.lib")
	#endif
#endif

#include <atlbase.h>

// Support for VS2005 Express & SDK ATL
#ifdef _WTL_SUPPORT_SDK_ATL3
  namespace ATL
  {
	inline void * __stdcall __AllocStdCallThunk()
	{
		return ::HeapAlloc(::GetProcessHeap(), 0, sizeof(_stdcallthunk));
	}

	inline void __stdcall __FreeStdCallThunk(void *p)
	{
		::HeapFree(::GetProcessHeap(), 0, p);
	}
  };
#endif // _WTL_SUPPORT_SDK_ATL3

#include <atlapp.h>

// Support for Bjarke Viksøe's shell extensions and controls
#include "atlshellextbase.h"

class CShellAppModule : public CAppModule
{
public:
	CShellMalloc m_Allocator;

	CShellAppModule()
	{
	   m_Allocator.Init();
	}

	~CShellAppModule()
	{
	   m_Allocator.Term();
	}	
} extern  _Module;

#include "atlshellext.h"

#include <atlwin.h>

