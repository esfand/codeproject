/** @file MessageHook.h
 *
 *	@brief Definition of CMessageHook.h
 *
 *	@author JaeWook Choi
 *	@version 1.0
 *
 *	<b><i>This software is provided "as is" without express or implied warranty. Use it at your own risk!</i></b>
 *
 */

#if !defined(MESSAGEHOOK_H_INCLUDED)
#define MESSAGEHOOK_H_INCLUDED

#pragma once

/**
 *	@brief CMessageHook class.
 *
 *	Reference:
 *	1) Thunking WndProcs in ATL By Fritz Onion (http://www.pluralsight.com/fritz/Articles/cpprep0399.htm)
 *	2) A freeware MFC class to support MFC subclassing By PJ Naughter (http://www.naughter.com/hookwnd.html)
 *	3) Pluggable Event Handler By William E. Kempf (http://www.codeproject.com/miscctrl/subclasswnd.asp)
 */
template<class T>
class CMessageHook
{
private:
	/**	@brief The WndProcThunk class
	 *	Wndproc Thunking:
	 *		change the window procedure to accept the C++ class as a parameter instead of
	 *		the window handle by altering incoming HWND parameter on the stack (located at exp+0x04)
	 *		to be the pointer of the C++ class instance associated with that window
	 *		using an assembly language thunk as shown below
	 *
	 *		mov dword ptr [esp+0x4], pThis
	 *		jmp WndProc
	 */
	class WndProcThunk
	{
	public:

#if defined(_M_IX86)

#pragma pack(push, 1)
		struct _WndProcThunk
		{
			DWORD   m_mov;          // mov dword ptr [esp+0x4], pThis (esp+0x4 is hWnd)
			DWORD   m_this;         //
			BYTE    m_jmp;          // jmp WndProc
			DWORD   m_relproc;      // relative jmp
		};
#pragma pack(pop)

#else	// #if defined(_M_IX86)

#error Only X86 supported

#endif	// #if defined(_M_IX86)
		
		_WndProcThunk thunk;
		
		void Init(WNDPROC proc, void* pThis)
		{

#if defined(_M_IX86)

			thunk.m_mov = 0x042444C7;  // C7 44 24 04
			thunk.m_this = (DWORD)pThis;
			thunk.m_jmp = 0xe9;
			thunk.m_relproc = (int)proc - ((int)this + sizeof(_WndProcThunk));

#endif	// #if defined(_M_IX86)

			// write block from data cache and flush from instruction cache
			::FlushInstructionCache(GetCurrentProcess(), &thunk, sizeof(thunk));
		}
	};

public:
	/** The CMessageHook() function
	 *
	 *	c'tor
	 *
	 *	@param bAutoDelete [in] a parameter of type BOOL	Specifies whether or not the class should be automatically deleted on receiving WM_NCDESTROY
	 */
	CMessageHook(BOOL bAutoDelete/* = FALSE*/)
		: m_bAutoDelete(bAutoDelete), m_hWnd(NULL), m_pfnSuperWindowProc(::DefWindowProc), m_bHook(FALSE)
	{
	}
		
	// Attributes
	HWND GetHwnd() const { return m_hWnd; }
	
protected:
	/// d'tor
	virtual ~CMessageHook()
	{
		Unhook();
	}

	// Operations
public:
	/**
	 *	Hook()
	 *
	 *	@param HWND hWnd [in] specifies the window handle which is being hooked (subclassed)
	 *
	 *	@return BOOL TRUE if the hook was installed successfully
	 *
	 */
	BOOL Hook(HWND hWnd)
	{
		if (hWnd == NULL || (hWnd && !::IsWindow(hWnd)))
		{
			//	TRACE(_T("warning: %s (%d) m_hWnd (%x) is not null or hWnd(%x) is not valid window\n"), __FILE__, __LINE__);
			return FALSE;
		}
		
		m_thunk.Init(WindowProc, this);
		WNDPROC pProc = (WNDPROC)& (m_thunk.thunk);
		WNDPROC pfnWndProc = (WNDPROC)::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)pProc);
		if (pfnWndProc == NULL)
			return FALSE;
		
		m_pfnSuperWindowProc = pfnWndProc;
		m_hWnd = hWnd;
		
		return ( m_bHook = TRUE );
	}

	/**
	 *	Unhook()
	 *
	 *
	 *	@return HWND the window handle from which the hook was being uninstalled
	 *
	 */
	HWND Unhook()
	{
		HWND hWnd = m_hWnd;
		
		// When an application subclasses a subclassed window, it must remove the subclasses in the reverse order they were performed.
		// If the removal order is not reversed, an unrecoverable system error may occur.
		if (!IsHooked())
		{
			//	TRACE(_T("\nCMessageHook::Unhook() - Unsubclassing a unsubclassed window or Unsubclassing of the multiple subclasssed window is not in order.\n\n"));
			// leave WNDPROC address unchanged as it was assigned from subclassing ( call to hook() ) thus our object (CMessageHook) will be lasting 
			// till window's terminiation (either on exiting out of scope or on receiving WM_NCDESTROY with m_bAutoDelete being set)
			// As a result, any other C++ object that may points and/or stores our m_thunk.thunk address by successive subclassing will
			// behavior valid therefore any message directed to our m_thunk.thunk will be correctly redirected to m_pfnSuperWindowProc
			// (original WNDPROC address of the subclassed window)
		}
		else
		{
			::SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)m_pfnSuperWindowProc);
			m_pfnSuperWindowProc = ::DefWindowProc;
			m_hWnd = NULL;
		}
		
		m_bHook = FALSE;
		
		return hWnd;
	}

	/**	@brief The CMessageHook::IsHooked() function
	 *	Check whether or not the hook had been installed by this CMessageHook or -derived object
	 *	
	 *
	 *	@return BOOL TRUE if the hook had been installed by this CMessageHook or -derived object
	 *
	 */
	BOOL IsHooked() const
	{
		if(m_hWnd && ::IsWindow(m_hWnd))
		{
			WNDPROC pProc = (WNDPROC)& (m_thunk.thunk);
			WNDPROC pActiveProc = (WNDPROC)::GetWindowLong(m_hWnd, GWL_WNDPROC);
			
			return (pProc == pActiveProc);
		}
		
		return FALSE;
	}
	
	inline void Delete()
	{
		if(::DefWindowProc == m_pfnSuperWindowProc)
			delete this;
//		else if(!m_bAutoDelete)
//			TRACE(_T("\nCMessageHook::Delete() - Memory Leaks!\n\n"));
	}
	
	// Overrides
protected:
	virtual BOOL ProcessWindowMessage(UINT nMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult) = 0;
	
	// Implementation
protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	inline LRESULT DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam) { return ::CallWindowProc(m_pfnSuperWindowProc, m_hWnd, nMsg, wParam, lParam); }
	
private:
	WndProcThunk m_thunk;
	WNDPROC m_pfnSuperWindowProc;
	BOOL m_bAutoDelete;
	HWND m_hWnd;
	BOOL m_bHook;
};

/**	@brief The CMessageHook::WindowProc() function
 *	
 *	@param	hWnd [in]	a parameter of type HWND Altered 
 *	@param	nMsg [in]	a parameter of type UINT 
 *	@param	wParam [in]	a parameter of type WPARAM 
 *	@param	lParam [in]	a parameter of type LPARAM 
 *
 *	@return LRESULT CALLBACK
 */
template<class T>
LRESULT CALLBACK CMessageHook<T>::WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
 	CMessageHook* pThis = (CMessageHook *)hWnd;
	//	ASSERT(!::IsBadReadPtr(pThis, sizeof(CMessageHook)));
	//	ASSERT(pThis);
	
	// pass to the message map to process
	LRESULT lResult = 0;
	BOOL bReturn = FALSE;
	if(pThis->m_bHook)
		bReturn = pThis->ProcessWindowMessage(nMsg, wParam, lParam, lResult);
	
	// do the default processing if message was not handled
	if (!bReturn)
	{
		lResult = pThis->DefWindowProc(nMsg, wParam, lParam);

		if (WM_NCDESTROY == nMsg && pThis->m_bAutoDelete)
			delete pThis;
	}

	return lResult;
}

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(MESSAGEHOOK_H_INCLUDED)
