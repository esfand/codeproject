// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wininet.h>
#include <PrSht.h>
#include <CryptuiAPI.h>
#include "WaitFuncs.h"

// CWaitFuncs exposes some replacement functions for the Win32
// Wait functions that work correctly with modal dialogs.
class CMainDlg : public CDialogImpl<CMainDlg>,
	public CWinDataExchange<CMainDlg>,
	public CWaitFuncs
{
protected:

	// Protected member vars, used for DDX
	CString		m_sHostName;
	CString		m_sRequest;
	CString		m_sUserName;
	CString		m_sPassword;
	bool		m_HttpsFlag;
	CComboBox	m_ComboAction;
	int			m_ActionSelected;
	int			m_Timeout;
	bool		m_CertFlag;
	bool		m_bIgnoreBadCertificates;

	HCERTSTORE		m_hMyStore;
	PCCERT_CONTEXT  m_pHSCertContext;

public:
	enum { IDD = IDD_MAINDLG };
	enum { CREATEHCONNECT, CREATEHREQUEST };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CMainDlg)
		DDX_INT( IDC_TIMEOUT, m_Timeout);
		DDX_RADIO( IDC_POST, m_ActionSelected )
		DDX_CONTROL_HANDLE( IDC_COMBOREQUEST, m_ComboAction )
		DDX_TEXT( IDC_COMBOREQUEST, m_sRequest )
		DDX_TEXT( IDC_HOSTNAME, m_sHostName )
		DDX_TEXT( IDC_USERNAME, m_sUserName )
		DDX_TEXT( IDC_PASSWORD, m_sPassword )
		DDX_CHECK( IDC_SSL, m_HttpsFlag )
		DDX_CHECK( IDC_CERTIFICATE, m_CertFlag);
		DDX_CHECK( IDC_BADCERTIFICATES, m_bIgnoreBadCertificates);
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	CMainDlg();
	~CMainDlg();

private:
	CString		m_sAgent;			// Agent string reported by WinInet
	CString		m_szMemo;			// String to store status memo
	int			m_nCallContext;		// Context of handle being created
	DWORD		m_dwInternetStatus;	// Last received internet status
	DWORD		m_dwError;			// Error returned in method CallBack
	BOOL		m_bExitRequestLoop;

	//root HINTERNET handle
	HINTERNET hOpen, hConnect, hRequest;

	// Internet events for asynchronous operation
	HANDLE hConnectedEvent, hRequestOpenedEvent, hRequestCompleteEvent;

	//*******************************************************************
	//						Function Declarations
	//*******************************************************************

	//functions
	int WINAPI WriteResponse(int, HINTERNET);
	int WINAPI WriteHeader(int, HINTERNET);
	void SendRequest (HINTERNET);

	//This is the wininet callback function.
	//It must be declared static to prevent
	//the compiler from munging it with a "this" pointer.
	static void __stdcall WinInetCallback(HINTERNET, DWORD_PTR , DWORD , LPVOID, DWORD);
	//instance of the callback function
	INTERNET_STATUS_CALLBACK iscCallback;	


	// Get error message is available anywhere, as long as you can pass in a WTL string
	// variable.
	static void GetErrorMessage(DWORD, CString*);
};
